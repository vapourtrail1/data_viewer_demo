#include "core/services/OrthogonalMprService.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

#if USE_VTK
#include <vtkSmartPointer.h>
#include <vtkDICOMImageReader.h>
#endif

namespace core::services {

    OrthogonalMprService::OrthogonalMprService()
#if USE_VTK
        : impl_(new Impl)
#else
        : impl_(nullptr)
#endif
    {
    }

    OrthogonalMprService::~OrthogonalMprService()
    {
#if USE_VTK
        delete impl_;
        impl_ = nullptr;
#endif
    }

    bool OrthogonalMprService::loadSeries(const QString& directory, QString* errorMessage)
    {
#if USE_VTK
        // ---- 获取实际使用的 DICOM 目录：允许为空时回退到默认路径 ----
        const QString actualDirectory = directory.isEmpty()
            ? QStringLiteral("F:/data/xor_test")
            : directory;

        // ---- 参数校验：确保指定的目录有效 ----
        QDir dir(actualDirectory);
        if (!dir.exists()) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("指定的 DICOM 目录不存在：%1").arg(actualDirectory);
            }
            return false;
        }

        // ---- 使用 vtkDICOMImageReader 读取序列 ----
        impl_->reader = vtkSmartPointer<vtkDICOMImageReader>::New();
        impl_->reader->SetDirectoryName(actualDirectory.toUtf8().constData());
        impl_->reader->Update();

        if (impl_->reader->GetOutput() == nullptr) {
            if (errorMessage) {
                *errorMessage = QStringLiteral("读取 DICOM 目录失败，请确认目录内容有效。");
            }
            impl_->hasData = false;
            return false;
        }

        impl_->hasData = true;
        return true;
#else
        Q_UNUSED(directory);
        if (errorMessage) {
            *errorMessage = QStringLiteral("当前构建未启用 VTK，无法加载 DICOM 数据。");
        }
        return false;
#endif
    }

    bool OrthogonalMprService::hasData() const
    {
#if USE_VTK
        return impl_ && impl_->hasData;
#else
        return false;
#endif
    }

#if USE_VTK

#include <array>
#include <utility>

#include <vtkCommand.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorLineRepresentation.h>
#include <vtkResliceCursorWidget.h>
#include <vtkResliceImageViewer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

    namespace {

        /**
         * @brief 回调：在任意一个重切片视图交互时刷新所有视图。
         */
        class ResliceCursorCallback final : public vtkCommand
        {
        public:
            static ResliceCursorCallback* New()
            {
                return new ResliceCursorCallback();
            }

            vtkTypeMacro(ResliceCursorCallback, vtkCommand);

            void SetViewers(const std::array<vtkResliceImageViewer*, 3>& viewers)
            {
                viewers_ = viewers;
            }

            void SetVolumeWindow(vtkRenderWindow* window)
            {
                volumeWindow_ = window;
            }

            void Execute(vtkObject* caller, unsigned long eventId, void*) override
            {
                (void)caller;
                (void)eventId;

                for (auto* viewer : viewers_) {
                    if (viewer) {
                        viewer->Render();
                    }
                }

                if (volumeWindow_) {
                    volumeWindow_->Render();
                }
            }

        private:
            std::array<vtkResliceImageViewer*, 3> viewers_{};
            vtkRenderWindow* volumeWindow_ = nullptr;
        };

    } // namespace

    struct OrthogonalMprService::Impl
    {
        vtkSmartPointer<vtkDICOMImageReader> reader;
        vtkSmartPointer<vtkResliceCursor> resliceCursor;
        std::array<vtkSmartPointer<vtkResliceImageViewer>, 3> viewers;
        std::array<vtkSmartPointer<vtkResliceCursorWidget>, 3> widgets;
        vtkSmartPointer<ResliceCursorCallback> callback;

        vtkSmartPointer<vtkRenderer> volumeRenderer;
        vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper;
        vtkSmartPointer<vtkVolume> volume;
        vtkSmartPointer<vtkVolumeProperty> volumeProperty;
        vtkSmartPointer<vtkColorTransferFunction> volumeColor;
        vtkSmartPointer<vtkPiecewiseFunction> volumeOpacity;

        vtkRenderWindow* linkedVolumeWindow = nullptr;
        bool hasData = false;
    };

    bool OrthogonalMprService::initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor)
    {
        // ---- 基本校验：必须先成功加载数据 ----
        if (!hasData()) {
            qWarning() << "[OrthogonalMprService] initializeViewers called before loadSeries.";
            return false;
        }

        if (!axialWindow || !sagittalWindow || !coronalWindow || !volumeWindow) {
            qWarning() << "[OrthogonalMprService] 无效的渲染窗口指针。";
            return false;
        }

        if (!axialInteractor || !sagittalInteractor || !coronalInteractor || !volumeInteractor) {
            qWarning() << "[OrthogonalMprService] 无效的交互器指针。";
            return false;
        }

        auto* image = impl_->reader->GetOutput();
        if (!image) {
            qWarning() << "[OrthogonalMprService] DICOM 输出图像为空。";
            return false;
        }

        // ---- 初始化重切片光标（共享同一实例确保联动） ----
        impl_->resliceCursor = vtkSmartPointer<vtkResliceCursor>::New();
        impl_->resliceCursor->SetImage(image);
        impl_->resliceCursor->SetThickMode(0);

        double range[2] = { 0.0, 1.0 };
        image->GetScalarRange(range);

        // ---- 三向视图初始化 ----
        const std::array<std::pair<vtkRenderWindow*, vtkRenderWindowInteractor*>, 3> viewConfig = {
            std::make_pair(axialWindow, axialInteractor),
            std::make_pair(sagittalWindow, sagittalInteractor),
            std::make_pair(coronalWindow, coronalInteractor)
        };

        for (int i = 0; i < 3; ++i) {
            impl_->viewers[i] = vtkSmartPointer<vtkResliceImageViewer>::New();
            impl_->viewers[i]->SetInputData(image);
            impl_->viewers[i]->SetResliceCursor(impl_->resliceCursor);
            impl_->viewers[i]->SetSliceOrientation(i);
            impl_->viewers[i]->SetRenderWindow(viewConfig[i].first);
            impl_->viewers[i]->SetupInteractor(viewConfig[i].second);
            impl_->viewers[i]->SetColorWindow(range[1] - range[0]);
            impl_->viewers[i]->SetColorLevel((range[0] + range[1]) * 0.5);
            impl_->viewers[i]->GetRenderer()->ResetCamera();

            impl_->widgets[i] = vtkSmartPointer<vtkResliceCursorWidget>::New();
            impl_->widgets[i]->SetInteractor(viewConfig[i].second);
            impl_->widgets[i]->SetDefaultRenderer(impl_->viewers[i]->GetRenderer());
            impl_->widgets[i]->SetResliceCursor(impl_->resliceCursor);
            impl_->widgets[i]->SetEnabled(1);
        }

        // ---- 建立回调，使得交互时刷新全部窗口 ----
        impl_->callback = vtkSmartPointer<ResliceCursorCallback>::New();
        impl_->callback->SetViewers({
            impl_->viewers[0].GetPointer(),
            impl_->viewers[1].GetPointer(),
            impl_->viewers[2].GetPointer()
            });
        impl_->callback->SetVolumeWindow(volumeWindow);

        for (auto& widget : impl_->widgets) {
            widget->AddObserver(vtkCommand::InteractionEvent, impl_->callback);
            widget->AddObserver(vtkResliceCursorWidget::ResliceAxesChangedEvent, impl_->callback);
        }

        // ---- 构建原始 DICOM 的体渲染视图 ----
        impl_->linkedVolumeWindow = volumeWindow;

        impl_->volumeRenderer = vtkSmartPointer<vtkRenderer>::New();
        impl_->volumeRenderer->SetBackground(0.1, 0.1, 0.1);

        volumeWindow->AddRenderer(impl_->volumeRenderer);
        volumeInteractor->SetRenderWindow(volumeWindow);

        impl_->volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
        impl_->volumeMapper->SetInputData(image);

        impl_->volumeColor = vtkSmartPointer<vtkColorTransferFunction>::New();
        impl_->volumeColor->AddRGBPoint(range[0], 0.0, 0.0, 0.0);
        impl_->volumeColor->AddRGBPoint(range[1], 1.0, 1.0, 1.0);

        impl_->volumeOpacity = vtkSmartPointer<vtkPiecewiseFunction>::New();
        impl_->volumeOpacity->AddPoint(range[0], 0.0);
        impl_->volumeOpacity->AddPoint(range[1], 1.0);

        impl_->volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
        impl_->volumeProperty->SetColor(impl_->volumeColor);
        impl_->volumeProperty->SetScalarOpacity(impl_->volumeOpacity);
        impl_->volumeProperty->SetInterpolationTypeToLinear();
        impl_->volumeProperty->ShadeOff();

        impl_->volume = vtkSmartPointer<vtkVolume>::New();
        impl_->volume->SetMapper(impl_->volumeMapper);
        impl_->volume->SetProperty(impl_->volumeProperty);

        impl_->volumeRenderer->AddVolume(impl_->volume);
        impl_->volumeRenderer->ResetCamera();

        // ---- 首次渲染，确保视图可见 ----
        axialWindow->Render();
        sagittalWindow->Render();
        coronalWindow->Render();
        volumeWindow->Render();

        return true;
    }

#endif // USE_VTK

} // namespace core::servicess