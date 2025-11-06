#include "core/services/OrthogonalMprService.h"

#include <QLoggingCategory>

#if USE_VTK
// VTK
#  include <vtkAutoInit.h>
#  include <vtkCommand.h>
#  include <vtkImageData.h>
#  include <vtkRenderWindow.h>
#  include <vtkRenderWindowInteractor.h>
#  include <vtkResliceImageViewer.h>
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2)
#endif

// 
#include "core/mpr/mprAssembly.h"
#include "core/mpr/mprState.h"
#include "core/mpr/mprInteractionRouter.h"
#include "core/render/renderService.h"
#include "core/io/volumeIOServiceVtk.h"
#include "core/data/volumeModel.h"

Q_LOGGING_CATEGORY(lcOrthogonalMpr, "core.services.OrthogonalMprService")

namespace core::services {
    struct OrthogonalMprService::Impl {
        bool hasData = false;
        core::data::VolumeModel cachedModel;

#if USE_VTK
        std::unique_ptr<core::mpr::MprState>             state = std::make_unique<core::mpr::MprState>();
        std::unique_ptr<core::mpr::MprAssembly>          assembly = std::make_unique<core::mpr::MprAssembly>();
        std::unique_ptr<core::mpr::MprInteractionRouter> router = std::make_unique<core::mpr::MprInteractionRouter>();
        std::unique_ptr<core::render::RenderService>     render = std::make_unique<core::render::RenderService>();
#endif
    };

#if USE_VTK
    // 三视图联动时用的回调
    namespace {
        class ResliceCursorCallback final : public vtkCommand {
        public:
            static ResliceCursorCallback* New() { return new ResliceCursorCallback(); }
            vtkTypeMacro(ResliceCursorCallback, vtkCommand);
            void Execute(vtkObject*, unsigned long, void*) override {
                // 需要补充：联动滚片时刷新 3D 体渲染等
            }
        };
    } // anonymous namespace
#endif


    OrthogonalMprService::OrthogonalMprService()
        : impl_(std::make_unique<Impl>()) {
    }

    OrthogonalMprService::~OrthogonalMprService() = default;

    // =========================== Public API ===========================
    bool OrthogonalMprService::hasData() const {
        return impl_->hasData;
    }

    void OrthogonalMprService::detach()
    {
#if USE_VTK
        if (!impl_->assembly) return;
        if (impl_->router) impl_->router->unwire();
        impl_->assembly->detach();
#endif
    }

    bool OrthogonalMprService::loadSeries(const QString& directory, QString* errorMessage)
    {
#if !USE_VTK
        if (errorMessage) *errorMessage = QStringLiteral("VTK 未启用");
        return false;
#else
        core::io::VolumeIOServiceVtk io;
        auto result = io.loadDicomDir(directory);
        if (!result.ok()) {
            if (errorMessage) *errorMessage = result.message;
            impl_->hasData = false;
            return false;
        }

        impl_->cachedModel = result.value;
        impl_->hasData = true;
        if (errorMessage) *errorMessage = result.message;

        // 绑定图像到状态（供 MPR 管线使用）
        if (impl_->state) {
            impl_->state->bindImage(impl_->cachedModel.image());
            impl_->state->resetToCenter();
        }
        return true;
#endif
    }

    bool OrthogonalMprService::initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor)
    {
#if !USE_VTK
        Q_UNUSED(axialWindow); Q_UNUSED(axialInteractor);
        Q_UNUSED(sagittalWindow); Q_UNUSED(sagittalInteractor);
        Q_UNUSED(coronalWindow); Q_UNUSED(coronalInteractor);
        Q_UNUSED(volumeWindow); Q_UNUSED(volumeInteractor);
        return false;
#else
        if (!impl_->hasData) return false;

        // 解除旧连线，重新布线
        if (impl_->router) impl_->router->unwire();

        // 用 Raw 接口绑定现有窗口/交互器
        impl_->assembly->attachRaw(
            axialWindow, axialInteractor,
            coronalWindow, coronalInteractor,
            sagittalWindow, sagittalInteractor,
            volumeWindow, volumeInteractor);

        impl_->assembly->setState(impl_->state.get());
        impl_->assembly->buildPipelines();

        // 同步 WL 到 2D，应用默认/记忆的预设到体渲染
        if (impl_->render) {
            impl_->render->syncWLTo2D(
                impl_->assembly->axialViewer(),
                impl_->assembly->coronalViewer(),
                impl_->assembly->sagittalViewer());
            // 如需默认预设，可在此调用：
            // impl_->render->applyPreset(QStringLiteral("SoftTissue"),
            //     impl_->assembly->axialViewer(),
            //     impl_->assembly->coronalViewer(),
            //     impl_->assembly->sagittalViewer(),
            //     impl_->assembly->volumeProperty());
        }

        // 居中并刷新
        impl_->state->resetToCenter();
        impl_->assembly->refreshAll();

        // 重新连线（事件路由）
        if (impl_->router) impl_->router->wire();

        return true;
#endif
    }

    void OrthogonalMprService::resetCursorToCenter()
    {
#if USE_VTK
        if (!impl_->hasData) return;
        impl_->state->resetToCenter();
        impl_->assembly->refreshAll();
#endif
    }

    void OrthogonalMprService::setSliceIndex(int axial, int coronal, int sagittal)
    {
#if USE_VTK
        if (!impl_->hasData) return;
        impl_->state->setIndices(axial, coronal, sagittal);
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(axial); Q_UNUSED(coronal); Q_UNUSED(sagittal);
#endif
    }

    void OrthogonalMprService::setWindowLevel(double window, double level, bool allViews)
    {
#if USE_VTK
        Q_UNUSED(allViews);
        if (!impl_->hasData) return;
        impl_->render->setWL(window, level);
        impl_->render->syncWLTo2D(
            impl_->assembly->axialViewer(),
            impl_->assembly->coronalViewer(),
            impl_->assembly->sagittalViewer());
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(window); Q_UNUSED(level); Q_UNUSED(allViews);
#endif
    }

    void OrthogonalMprService::applyPreset(const QString& name)
    {
#if USE_VTK
        if (!impl_->hasData) return;
        impl_->render->applyPreset(
            name,
            impl_->assembly->axialViewer(),
            impl_->assembly->coronalViewer(),
            impl_->assembly->sagittalViewer(),
            impl_->assembly->volumeProperty());
        impl_->assembly->refreshAll();
#else
        Q_UNUSED(name);
#endif
    }

} // namespace core::services
