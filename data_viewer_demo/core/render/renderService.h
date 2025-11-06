#pragma once
#include <QString>
#include "core/common/type.h"
#include "core/render/transferFunctionStroe.h"

class vtkResliceImageViewer;
class vtkVolumeProperty;

namespace core::render {

    /**
    RenderService 统一管理窗宽窗位与预设应用。
    */
    class RenderService
    {
    public:
        RenderService();

        void setWL(double window, double level);
        core::common::WL wl() const;

        void applyPreset(const QString& name, vtkResliceImageViewer* axial, vtkResliceImageViewer* coronal, vtkResliceImageViewer* sagittal, vtkVolumeProperty* volumeProp);
        void syncWLTo2D(vtkResliceImageViewer* axial, vtkResliceImageViewer* coronal, vtkResliceImageViewer* sagittal);

    private:
        TransferFunctionStore m_store;
        core::common::WL m_currentWL{ core::common::DefaultWL };
    };

} // namespace core::render