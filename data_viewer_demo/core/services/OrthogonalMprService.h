#pragma once
#include <memory>
#include <QString>
#include "core/common/VtkMacros.h"

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
    /*
     * 负责加载 DICOM 系列和连接 VTK 查看器的逻辑的服务接口。
     * 该实现有意隐藏头文件中的所有 VTK 头文件；当未启用 VTK 时可降级。
     */
    class OrthogonalMprService
    {
    public:
        OrthogonalMprService();
        ~OrthogonalMprService();

        // 数据加载
        bool loadSeries(const QString& directory, QString* errorMessage = nullptr);

        // 绑定现有的 4 个窗口与交互器（Raw 模式）
        bool initializeViewers(vtkRenderWindow* axialWindow,
            vtkRenderWindowInteractor* axialInteractor,   // 横断面
            vtkRenderWindow* sagittalWindow,
            vtkRenderWindowInteractor* sagittalInteractor,
            vtkRenderWindow* coronalWindow,
            vtkRenderWindowInteractor* coronalInteractor,
            vtkRenderWindow* volumeWindow,
            vtkRenderWindowInteractor* volumeInteractor);

        // 资源解绑
        void detach();                                  

        // 状态查询
        bool hasData() const;

        // 光标/切片与窗宽窗位控制
        void resetCursorToCenter();                       
        void setSliceIndex(int axial, int coronal, int sagittal);       
        void setWindowLevel(double window, double level, bool allViews = true); 
        void applyPreset(const QString& name);            

    private:
        struct Impl;
		std::unique_ptr<Impl> impl_;// 
    };
} // namespace core::services
