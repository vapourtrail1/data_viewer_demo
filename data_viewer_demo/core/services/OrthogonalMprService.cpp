#include "core/services/OrthogonalMprService.h"

#include <vtkAutoInit.h>
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);

Q_LOGGING_CATEGORY(lcOrthogonalMpr, "core.services.OrthogonalMprService");


namespace core::services {

#if USE_VTK
    namespace {

        /**
         *  在查看器之间传播切片交互事件，并保持体积视图更新
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

void OrthogonalMprService::detach()
{
    m_router->unwire();
    if (m_assembly) {
        m_assembly->detach();
    }
}

bool OrthogonalMprService::setInput(const core::data::VolumeModel& model)
{
    if (!model.image()) {
        return false;
    }

    m_cachedModel = model;
    m_hasData = true;

    m_router->unwire();

    } // 
#endif // USE_VTK

    m_render->applyPreset(QStringLiteral("SoftTissue"), m_assembly->axialViewer(), m_assembly->coronalViewer(), m_assembly->sagittalViewer(), m_assembly->volumeProperty());

    m_state->resetToCenter();
    m_assembly->refreshAll();

    m_router->wire();
    return true;
}

void OrthogonalMprService::resetCursorToCenter()
{
    if (!m_hasData) {
        return;
    }

    OrthogonalMprService::~OrthogonalMprService() = default;

    bool OrthogonalMprService::loadSeries(const QString& directory, QString* errorMessage)
    {
        if (!impl_) {
            impl_ = std::make_unique<Impl>();
        }

void OrthogonalMprService::setSliceIndex(int axial, int coronal, int sagittal)
{
    if (!m_hasData) {
        return;
    }

    m_state->setIndices(axial, coronal, sagittal);
    m_assembly->refreshAll();
}

void OrthogonalMprService::setWindowLevel(double window, double level, bool allViews)
{
    Q_UNUSED(allViews);
    if (!m_hasData) {
        return;
    }

    m_render->setWL(window, level);
    m_render->syncWLTo2D(m_assembly->axialViewer(), m_assembly->coronalViewer(), m_assembly->sagittalViewer());
}

void OrthogonalMprService::applyPreset(const QString& name)
{
    if (!m_hasData) {
        return;
    }

    m_render->applyPreset(name, m_assembly->axialViewer(), m_assembly->coronalViewer(), m_assembly->sagittalViewer(), m_assembly->volumeProperty());
    m_assembly->refreshAll();
}

bool OrthogonalMprService::loadSeries(const QString& directory, QString* errorMessage)
{
    core::io::VolumeIOServiceVtk io;
    auto result = io.loadDicomDir(directory);
    if (!result.ok()) {
        if (errorMessage) {
            *errorMessage = result.message;
        }
        m_hasData = false;
        return false;
    }

    m_cachedModel = result.value;
    m_hasData = true;
    if (errorMessage) {
        *errorMessage = result.message;
    }
    return setInput(m_cachedModel);
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
    if (!m_hasData) {
        return false;
    }

    m_router->unwire();

    m_assembly->attachRaw(axialWindow, axialInteractor, coronalWindow, coronalInteractor, sagittalWindow, sagittalInteractor, volumeWindow, volumeInteractor);
    m_assembly->setState(m_state.get());
    m_assembly->buildPipelines();
    m_render->syncWLTo2D(m_assembly->axialViewer(), m_assembly->coronalViewer(), m_assembly->sagittalViewer());
    m_state->resetToCenter();
    m_assembly->refreshAll();
    m_router->wire();
    return true;
}

bool OrthogonalMprService::hasData() const
{
    return m_hasData;
}

} // namespace core::services
