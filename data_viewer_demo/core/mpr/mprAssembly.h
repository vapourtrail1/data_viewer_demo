#pragma once

#include <array>
#include <vtkSmartPointer.h>

class QVTKOpenGLNativeWidget;
class vtkGenericOpenGLRenderWindow;
class vtkRenderWindow;
class vtkRenderWindowInteractor;
class vtkRenderer;
class vtkResliceCursorActor;
class vtkResliceImageViewer;
class vtkSmartVolumeMapper;
class vtkVolume;
class vtkVolumeProperty;
class vtkImageData;
class vtkLookupTable;
class vtkCallbackCommand;
class vtkImagePlaneWidget;
class vtkAxesActor;
class vtkOrientationMarkerWidget;

namespace core::mpr {

    class MprState;

    /**
     * 负责粘合三视图（axial / coronal / sagittal）与 3D 视窗的 VTK 管线。
     * 本版本额外在 3D 视窗加入三张 vtkImagePlaneWidget，可拖动，
     * 并与三视图做双向联动。
     */
    class MprAssembly
    {
    public:
        MprAssembly();
        ~MprAssembly();

        //  UI 模式：传入 4 个 QVTKOpenGLNativeWidget 
        void attach(QVTKOpenGLNativeWidget* axial,
            QVTKOpenGLNativeWidget* coronal,
            QVTKOpenGLNativeWidget* sagittal,
            QVTKOpenGLNativeWidget* volume3D);

        //  裸 VTK 模式：直接传 4 套 window + interactor 
        void attachRaw(vtkRenderWindow* axialWindow,
            vtkRenderWindowInteractor* axialInteractor,
            vtkRenderWindow* coronalWindow,
            vtkRenderWindowInteractor* coronalInteractor,
            vtkRenderWindow* sagittalWindow,
            vtkRenderWindowInteractor* sagittalInteractor,
            vtkRenderWindow* volumeWindow,
            vtkRenderWindowInteractor* volumeInteractor);

        // 释放内部持有的 VTK 对象 不删除 Qt Widget 和 外部 window
        void detach();

        // 绑定状态 里面有 image  reslice cursor
        void setState(MprState* state);

        // 建立三视图 + 3D 的基础管线（不含 3D 三平面）
        void buildPipelines();

        // 3D 三平面相关 
        // 在 buildPipelines() 完成后调用，在 3D 视窗里创建三张可拖动切面
        void build3DPlanes();

        // 3D -> 2D：任意平面交互后，同步三视图的切片索引
        void syncFrom3DWidgets();

        // 2D -> 3D：三视图切片变化后回写 3D 平面
        void on2DSliceChanged(int axialZ, int coronalY, int sagittalX);

        // 刷新所有视图
        void refreshAll();

        // 坐标轴控制接口
        void SetAxesPosition(double x, double y);
        void SetAxesSize(double w, double h);
        void UpdateAxesViewPort();

        // 对外 getter，给 OrthogonalMprService / RenderService / Router 用 
        vtkResliceImageViewer* axialViewer() const;
        vtkResliceImageViewer* coronalViewer() const;
        vtkResliceImageViewer* sagittalViewer() const;
        vtkVolumeProperty* volumeProperty() const;

    private:
        // 内部工具函数 
        void setup2DViewer(vtkResliceImageViewer* viewer,
            int orientation,
            QVTKOpenGLNativeWidget* widget,
            vtkGenericOpenGLRenderWindow** windowStorage);

        void setup2DViewerRaw(vtkResliceImageViewer* viewer,
            int orientation,
            vtkRenderWindow* window,
            vtkRenderWindowInteractor* interactor);

        void setup3DScene(QVTKOpenGLNativeWidget* widget);
        void setup3DSceneRaw(vtkRenderWindow* window,
            vtkRenderWindowInteractor* interactor);

        // 三平面用的 LUT / 工具
        void ensureLutFromImage(vtkImageData* img);
        static int centerIndex(int lo, int hi) { return (lo + hi) / 2; }

        // 状态（图像 + Reslice Cursor） 
        MprState* m_state = nullptr;

        // 成员是Qt的组件
        QVTKOpenGLNativeWidget* m_axialWidget = nullptr;
        QVTKOpenGLNativeWidget* m_coronalWidget = nullptr;
        QVTKOpenGLNativeWidget* m_sagittalWidget = nullptr;
        QVTKOpenGLNativeWidget* m_volumeWidget = nullptr;

        vtkGenericOpenGLRenderWindow* m_axialWindow = nullptr;
        vtkGenericOpenGLRenderWindow* m_coronalWindow = nullptr;
        vtkGenericOpenGLRenderWindow* m_sagittalWindow = nullptr;
        vtkGenericOpenGLRenderWindow* m_volumeWindow = nullptr;

        //坐标轴
		vtkAxesActor* m_axesActor = nullptr;
        vtkOrientationMarkerWidget* m_axesWidget = nullptr;
        //坐标轴成员变量
        double m_axesX = 0.0;
        double m_axesY = 0.0;
        double m_axesW = 0.4;
        double m_axesH = 0.4;

        // 成员：VTK 模式 
        vtkRenderWindow* m_axialRawWindow = nullptr;
        vtkRenderWindowInteractor* m_axialRawInteractor = nullptr;
        vtkRenderWindow* m_coronalRawWindow = nullptr;
        vtkRenderWindowInteractor* m_coronalRawInteractor = nullptr;
        vtkRenderWindow* m_sagittalRawWindow = nullptr;
        vtkRenderWindowInteractor* m_sagittalRawInteractor = nullptr;
        vtkRenderWindow* m_volumeRawWindow = nullptr;
        vtkRenderWindowInteractor* m_volumeRawInteractor = nullptr;

        // 三视图 / 3D 渲染对象 
        vtkResliceImageViewer* m_axialViewer = nullptr;
        vtkResliceImageViewer* m_coronalViewer = nullptr;
        vtkResliceImageViewer* m_sagittalViewer = nullptr;

        vtkSmartVolumeMapper* m_volumeMapper = nullptr;
        vtkVolume* m_volume = nullptr;
        vtkVolumeProperty* m_volumeProperty = nullptr;
        vtkRenderer* m_renderer3D = nullptr;

        // 3D 三张平面 widget 与 LUT / 回调 
        vtkSmartPointer<vtkImagePlaneWidget> m_planeX; // 垂直 X
        vtkSmartPointer<vtkImagePlaneWidget> m_planeY; // 垂直 Y
        vtkSmartPointer<vtkImagePlaneWidget> m_planeZ; // 垂直 Z
        vtkSmartPointer<vtkLookupTable>      m_lut;
        vtkSmartPointer<vtkCallbackCommand>  m_cb3D;
    };

} // namespace core::mpr
