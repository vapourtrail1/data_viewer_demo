#pragma once

#include <memory>
#include <QString>

#include "core/data/VolumeModel.h"

class QVTKOpenGLNativeWidget;
class vtkRenderWindow;
class vtkRenderWindowInteractor;

namespace core::mpr {
class MprAssembly;
class MprState;
class MprInteractionRouter;
}

namespace core::render {
class RenderService;
}

namespace core::services {

/**
 * @brief OrthogonalMprService 提供三正交面+3D 渲染的统一入口。
 */
class OrthogonalMprService
{
public:
    OrthogonalMprService();
    ~OrthogonalMprService();

    void attachTo(QVTKOpenGLNativeWidget* axial, QVTKOpenGLNativeWidget* coronal, QVTKOpenGLNativeWidget* sagittal, QVTKOpenGLNativeWidget* volume3D);
    void detach();

    bool setInput(const core::data::VolumeModel& model);

    void resetCursorToCenter();
    void setSliceIndex(int axial, int coronal, int sagittal);

    void setWindowLevel(double window, double level, bool allViews = true);
    void applyPreset(const QString& name);

    // 兼容旧前端接口
    bool loadSeries(const QString& directory, QString* errorMessage = nullptr);
    bool initializeViewers(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor);
    bool hasData() const;

private:
    std::unique_ptr<core::mpr::MprState> m_state;
    std::unique_ptr<core::mpr::MprAssembly> m_assembly;
    std::unique_ptr<core::mpr::MprInteractionRouter> m_router;
    std::unique_ptr<core::render::RenderService> m_render;

    core::data::VolumeModel m_cachedModel;
    bool m_hasData = false;
};

} // namespace core::services
