#pragma once

#include <memory>
#include <QString>
#include <vtkImageData.h>
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

    /**
     * 封装三视图 + 3D MPR 的服务层，对外提供简单接口：
     *  - initializeViewers(...)：绑定 4 个 VTK 窗口 / 交互器
     *  - bindImage(...)：绑定一份 vtkImageData（目前关注 DICOM）
     *  - setSliceIndex / setWindowLevel / applyPreset 等操作
     */
    class OrthogonalMprService
    {
    public:
        OrthogonalMprService();
        ~OrthogonalMprService();

		//加上声明：从目录加载 DICOM 序列
        bool loadSeries(const QString& directory, QString* error);

        // 绑定 4 个 VTK 窗口 / 交互器  是从 UI 那边传进来
        bool initializeViewers(
            vtkRenderWindow* axialWindow, vtkRenderWindowInteractor* axialInteractor,
            vtkRenderWindow* sagittalWindow, vtkRenderWindowInteractor* sagittalInteractor,
            vtkRenderWindow* coronalWindow, vtkRenderWindowInteractor* coronalInteractor,
            vtkRenderWindow* volumeWindow, vtkRenderWindowInteractor* volumeInteractor);

        // 支持从 QVTKOpenGLNativeWidget 直接 attach  ，如果在 UI 那边走的是 Widget 模式
        void attachWidgets(QVTKOpenGLNativeWidget* axial,
            QVTKOpenGLNativeWidget* coronal,
            QVTKOpenGLNativeWidget* sagittal,
            QVTKOpenGLNativeWidget* volume3D);

        // 解除绑定
        void detach();

#if USE_VTK
        // 绑定一份体数据
        bool bindImage(vtkImageData* img);
#endif

        // 状态查询
        bool hasData() const;

        // 重置游标到体数据中心
        void resetCursorToCenter();

        // 设置三向切片索引（axial / coronal / sagittal）
        void setSliceIndex(int axial, int coronal, int sagittal);

        // 设置窗宽窗位
        void setWindowLevel(double window, double level, bool allViews = true);

        // 应用预设
        void applyPreset(const QString& name);

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace core::services
