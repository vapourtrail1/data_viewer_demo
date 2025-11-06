#include "core/render/renderService.h"

#include <vtkResliceImageViewer.h>
#include <vtkVolumeProperty.h>

namespace core::render {

    RenderService::RenderService() = default;

    void RenderService::setWL(double window, double level)
    {
        m_currentWL = { window, level };
    }

    core::common::WL RenderService::wl() const
    {
        return m_currentWL;
    }

    void RenderService::applyPreset(const QString& name, vtkResliceImageViewer* axial, vtkResliceImageViewer* coronal, vtkResliceImageViewer* sagittal, vtkVolumeProperty* volumeProp)
    {
        // 首先使用预设更新当前窗宽/窗位。
        const core::common::WL preset = m_store.presetWL(name);
        setWL(preset.first, preset.second);
        syncWLTo2D(axial, coronal, sagittal);

        // 同时配置 3D 体渲染属性。
        m_store.fillVolumePreset(name, volumeProp);
    }

    void RenderService::syncWLTo2D(vtkResliceImageViewer* axial, vtkResliceImageViewer* coronal, vtkResliceImageViewer* sagittal)
    {
        const auto apply = [this](vtkResliceImageViewer* viewer) {
            if (!viewer) {
                return;
            }
            viewer->SetColorWindow(m_currentWL.first);
            viewer->SetColorLevel(m_currentWL.second);
            viewer->Render();
            };

        apply(axial);
        apply(coronal);
        apply(sagittal);
    }

} // namespace core::render