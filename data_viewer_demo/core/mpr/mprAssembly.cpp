#include "core/mpr/mprAssembly.h"
#include "core/mpr/mprState.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkResliceCursor.h>
#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkImagePlaneWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkCamera.h>
#include <vtkProperty.h>

#include <qDebug>

namespace core::mpr {

    MprAssembly::MprAssembly() = default;

    MprAssembly::~MprAssembly()
    {
        detach();
    }

    //attach detach
    void MprAssembly::attach(QVTKOpenGLNativeWidget* axial,
        QVTKOpenGLNativeWidget* coronal,
        QVTKOpenGLNativeWidget* sagittal,
        QVTKOpenGLNativeWidget* volume3D)
    {
        m_axialWidget = axial;
        m_coronalWidget = coronal;
        m_sagittalWidget = sagittal;
        m_volumeWidget = volume3D;

        m_axialRawWindow = nullptr;
        m_axialRawInteractor = nullptr;
        m_coronalRawWindow = nullptr;
        m_coronalRawInteractor = nullptr;
        m_sagittalRawWindow = nullptr;
        m_sagittalRawInteractor = nullptr;
        m_volumeRawWindow = nullptr;
        m_volumeRawInteractor = nullptr;
    }

    void MprAssembly::attachRaw(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor)
    {
        m_axialRawWindow = axialWindow;
        m_axialRawInteractor = axialInteractor;
        m_coronalRawWindow = coronalWindow;
        m_coronalRawInteractor = coronalInteractor;
        m_sagittalRawWindow = sagittalWindow;
        m_sagittalRawInteractor = sagittalInteractor;
        m_volumeRawWindow = volumeWindow;
        m_volumeRawInteractor = volumeInteractor;

        m_axialWidget = nullptr;
        m_coronalWidget = nullptr;
        m_sagittalWidget = nullptr;
        m_volumeWidget = nullptr;
    }

    void MprAssembly::detach()
    {
        // 先删 2D / 3D 的 VTK 对象
        if (m_axialViewer) { m_axialViewer->Delete();   m_axialViewer = nullptr; }
        if (m_coronalViewer) { m_coronalViewer->Delete(); m_coronalViewer = nullptr; }
        if (m_sagittalViewer) { m_sagittalViewer->Delete(); m_sagittalViewer = nullptr; }

        if (m_volumeMapper) { m_volumeMapper->Delete();  m_volumeMapper = nullptr; }
        if (m_volume) { m_volume->Delete();        m_volume = nullptr; }
        if (m_volumeProperty) { m_volumeProperty->Delete(); m_volumeProperty = nullptr; }
        if (m_renderer3D) { m_renderer3D->Delete();    m_renderer3D = nullptr; }

        // 三个 plane widget 是 smart pointer，会自动释放

        // 把 Qt 画布上的 RenderWindow 解绑
        if (m_axialWidget) {
            m_axialWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }
        if (m_coronalWidget) {
            m_coronalWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }
        if (m_sagittalWidget) {
            m_sagittalWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }
        if (m_volumeWidget) {
            m_volumeWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }

        // 删除 Qt 模式下创建的 GenericOpenGLRenderWindow
        if (m_axialWindow) { m_axialWindow->Delete();   m_axialWindow = nullptr; }
        if (m_coronalWindow) { m_coronalWindow->Delete(); m_coronalWindow = nullptr; }
        if (m_sagittalWindow) { m_sagittalWindow->Delete(); m_sagittalWindow = nullptr; }
        if (m_volumeWindow) { m_volumeWindow->Delete();  m_volumeWindow = nullptr; }

        //坐标轴析构
		if (m_axesActor) { m_axesActor->Delete(); m_axesActor = nullptr; }
        if (m_axesWidget) { m_axesWidget->Delete(); m_axesWidget = nullptr; }

        m_axialWidget = m_coronalWidget = m_sagittalWidget = m_volumeWidget = nullptr;
        m_axialRawWindow = m_coronalRawWindow = m_sagittalRawWindow = m_volumeRawWindow = nullptr;
        m_axialRawInteractor = m_coronalRawInteractor = m_sagittalRawInteractor = m_volumeRawInteractor = nullptr;
    }

    //  state & pipeline 

    void MprAssembly::setState(MprState* state)
    {
        m_state = state;
    }

    void MprAssembly::buildPipelines()
    {
        if (!m_state || !m_state->cursor() || !m_state->image()) {
            return;
        }

        // 重新创建前先释放旧的 VTK 对象（保留窗口指针）
        if (m_axialViewer) { m_axialViewer->Delete();   m_axialViewer = nullptr; }
        if (m_coronalViewer) { m_coronalViewer->Delete(); m_coronalViewer = nullptr; }
        if (m_sagittalViewer) { m_sagittalViewer->Delete(); m_sagittalViewer = nullptr; }
        if (m_volumeMapper) { m_volumeMapper->Delete();  m_volumeMapper = nullptr; }
        if (m_volume) { m_volume->Delete();        m_volume = nullptr; }
        if (m_volumeProperty) { m_volumeProperty->Delete(); m_volumeProperty = nullptr; }
        if (m_renderer3D) { m_renderer3D->Delete();    m_renderer3D = nullptr; }
		if (m_axesWidget) { m_axesWidget->Delete(); m_axesWidget = nullptr; }//坐标轴
        if (m_axesActor) { m_axesActor->Delete();  m_axesActor = nullptr; }

        m_axialViewer = vtkResliceImageViewer::New();
        m_coronalViewer = vtkResliceImageViewer::New();
        m_sagittalViewer = vtkResliceImageViewer::New();

        // Qt Widget 模式
        if (m_axialWidget || m_coronalWidget || m_sagittalWidget || m_volumeWidget) {
            setup2DViewer(m_axialViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_XY,
                m_axialWidget,
                &m_axialWindow);
            setup2DViewer(m_coronalViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_XZ,
                m_coronalWidget,
                &m_coronalWindow);
            setup2DViewer(m_sagittalViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_YZ,
                m_sagittalWidget,
                &m_sagittalWindow);
            setup3DScene(m_volumeWidget);
        }
        // VTK 模式
        else if (m_axialRawWindow && m_coronalRawWindow &&
            m_sagittalRawWindow && m_volumeRawWindow) {

            setup2DViewerRaw(m_axialViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_XY,
                m_axialRawWindow,
                m_axialRawInteractor);
            setup2DViewerRaw(m_coronalViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_XZ,
                m_coronalRawWindow,
                m_coronalRawInteractor);
            setup2DViewerRaw(m_sagittalViewer,
                vtkResliceImageViewer::SLICE_ORIENTATION_YZ,
                m_sagittalRawWindow,
                m_sagittalRawInteractor);
            setup3DSceneRaw(m_volumeRawWindow, m_volumeRawInteractor);
        }
        SetAxesPosition(0, 0);  
        SetAxesSize(0.27, 0.27);
    }

    void MprAssembly::SetAxesPosition(double x, double y) {
        m_axesX = x;
        m_axesY = y;
        UpdateAxesViewPort();
    }

    void MprAssembly::SetAxesSize(double w, double h) {
        m_axesW = w;
        m_axesH = h;
        UpdateAxesViewPort();
    }

    void MprAssembly::UpdateAxesViewPort() {
        if (!m_axesWidget) {
            return;
        }

        double x_min = m_axesX;
        double y_min = m_axesY;
        double x_max = m_axesX + m_axesW;
        double y_max = m_axesY + m_axesH;

        m_axesWidget->SetViewport(x_min, y_min, x_max, y_max);

        if (m_volumeWindow) {
            m_volumeWindow->Render();
        }

    }

    //2D/3D 管线 
    void MprAssembly::setup2DViewer(vtkResliceImageViewer* viewer,
        int orientation,
        QVTKOpenGLNativeWidget* widget,
        vtkGenericOpenGLRenderWindow** windowStorage)
    {
        if (!viewer || !widget || !m_state) {
            return;
        }

        auto* window = vtkGenericOpenGLRenderWindow::New();
        widget->setRenderWindow(window);
        viewer->SetRenderWindow(window);
        viewer->SetupInteractor(widget->interactor());
        viewer->SetResliceCursor(m_state->cursor());
        viewer->SetInputData(m_state->image());
        viewer->SetSliceOrientation(orientation);
        viewer->SetResliceModeToAxisAligned();
        viewer->SetColorWindow(400.0);
        viewer->SetColorLevel(40.0);
        viewer->Render();

        if (windowStorage) {
            *windowStorage = window;
        }
    }

    void MprAssembly::setup2DViewerRaw(vtkResliceImageViewer* viewer,
        int orientation,
        vtkRenderWindow* window,
        vtkRenderWindowInteractor* interactor)
    {
        if (!viewer || !window || !interactor || !m_state) {
            return;
        }

        viewer->SetRenderWindow(window);
        viewer->SetupInteractor(interactor);
        viewer->SetResliceCursor(m_state->cursor());
        viewer->SetInputData(m_state->image());
        viewer->SetSliceOrientation(orientation);
        viewer->SetResliceModeToAxisAligned();
        viewer->SetColorWindow(400.0);
        viewer->SetColorLevel(40.0);
        viewer->Render();
    }

    void MprAssembly::setup3DScene(QVTKOpenGLNativeWidget* widget)
    {
        if (!widget || !m_state) {
            return;
        }

        m_volumeWindow = vtkGenericOpenGLRenderWindow::New();
        widget->setRenderWindow(m_volumeWindow);

        m_renderer3D = vtkRenderer::New();
        m_volumeWindow->AddRenderer(m_renderer3D);

        m_volumeMapper = vtkSmartVolumeMapper::New();
        m_volumeMapper->SetInputData(m_state->image());

        m_volumeProperty = vtkVolumeProperty::New();
        m_volumeProperty->ShadeOn();
        m_volumeProperty->SetInterpolationTypeToLinear();

        // 3D 坐标轴  可修改
        auto* interactor = widget->interactor();
        if (interactor) {
            m_axesActor = vtkAxesActor::New();
            // 轴长度
            m_axesActor->SetTotalLength(80.0, 80.0, 80.0);
            m_axesWidget = vtkOrientationMarkerWidget::New();
            m_axesWidget->SetOrientationMarker(m_axesActor);
            // 坐标轴显示在左下角小窗口 
            m_axesWidget->SetInteractor(interactor);
            m_axesWidget->SetEnabled(1);
            m_axesWidget->InteractiveOff();
            UpdateAxesViewPort();
        }

        m_volume = vtkVolume::New();
        m_volume->SetMapper(m_volumeMapper);
        m_volume->SetProperty(m_volumeProperty);

        m_renderer3D->AddVolume(m_volume);
        m_renderer3D->ResetCamera();

        
		//auto* camera = m_renderer3D->GetActiveCamera();
  //      if (camera) {
  //          camera->Zoom(30);
		//	m_renderer3D->ResetCameraClippingRange();//放大厚重新设置裁剪范围
  //      }
    }

    void MprAssembly::setup3DSceneRaw(vtkRenderWindow* window,
        vtkRenderWindowInteractor* interactor)
    {
        if (!window || !interactor || !m_state) {
            return;
        }

        window->SetInteractor(interactor);

        m_renderer3D = vtkRenderer::New();
        window->AddRenderer(m_renderer3D);

        m_volumeMapper = vtkSmartVolumeMapper::New();
        m_volumeMapper->SetInputData(m_state->image());

        m_volumeProperty = vtkVolumeProperty::New();
        m_volumeProperty->ShadeOn();
        m_volumeProperty->SetInterpolationTypeToLinear();

        if (interactor) {
            m_axesActor = vtkAxesActor::New();
            m_axesActor->SetTotalLength(99, 99, 99);
            m_axesWidget = vtkOrientationMarkerWidget::New();
            m_axesWidget->SetOrientationMarker(m_axesActor);
            m_axesWidget->SetInteractor(interactor);
            m_axesWidget->SetEnabled(1);
            m_axesWidget->InteractiveOff();
            UpdateAxesViewPort();
        }

        m_volume = vtkVolume::New();
        m_volume->SetMapper(m_volumeMapper);
        m_volume->SetProperty(m_volumeProperty);

        m_renderer3D->AddVolume(m_volume);
        m_renderer3D->ResetCamera();

		/*auto carema = m_renderer3D->GetActiveCamera();
        if (carema) {
			carema->Zoom(30);
            m_renderer3D->ResetCameraClippingRange();
        }*/
    }

    //  刷新 & getter 

    void MprAssembly::refreshAll()
    {
        if (m_axialWindow) {
            m_axialWindow->Render();
        }
        else if (m_axialRawWindow) {
            m_axialRawWindow->Render();
        }

        if (m_coronalWindow) {
            m_coronalWindow->Render();
        }
        else if (m_coronalRawWindow) {
            m_coronalRawWindow->Render();
        }

        if (m_sagittalWindow) {
            m_sagittalWindow->Render();
        }
        else if (m_sagittalRawWindow) {
            m_sagittalRawWindow->Render();
        }

        if (m_volumeWindow) {
            m_volumeWindow->Render();
        }
        else if (m_volumeRawWindow) {
            m_volumeRawWindow->Render();
        }
    }

    vtkResliceImageViewer* MprAssembly::axialViewer() const
    {
        return m_axialViewer;
    }

    vtkResliceImageViewer* MprAssembly::coronalViewer() const
    {
        return m_coronalViewer;
    }

    vtkResliceImageViewer* MprAssembly::sagittalViewer() const
    {
        return m_sagittalViewer;
    }

    vtkVolumeProperty* MprAssembly::volumeProperty() const
    {
        return m_volumeProperty;
    }

    //  3D 三平面相关 

    void MprAssembly::ensureLutFromImage(vtkImageData* img)
    {
        if (!m_lut) {
            m_lut = vtkSmartPointer<vtkLookupTable>::New();
        }
        double range[2] = { 0.0, 1.0 };
        if (img) {
            img->GetScalarRange(range);
        }
        m_lut->SetRange(range);
        m_lut->SetHueRange(0.0, 0.0);   // 单色
        m_lut->Build();
    }

    void MprAssembly::build3DPlanes()
    {
        if (!m_state || !m_state->image()) {
            return;
        }

        // 找到 3D 视窗对应的 interactor
        vtkRenderWindowInteractor* interactor = nullptr;
        if (m_volumeRawInteractor) {
            interactor = m_volumeRawInteractor;
        }
        else if (m_volumeWindow) {
            interactor = m_volumeWindow->GetInteractor();
        }
        if (!interactor) {
            return;
        }

        auto* img = m_state->image();
        ensureLutFromImage(img);

        const int* ext = img->GetExtent(); // [x0,x1, y0,y1, z0,z1]
        const int cx = centerIndex(ext[0], ext[1]);
        const int cy = centerIndex(ext[2], ext[3]);
        const int cz = centerIndex(ext[4], ext[5]);

        // 统一封装lambda：创建+配置 plane 
        auto setupPlane = [this, interactor, img](vtkSmartPointer<vtkImagePlaneWidget>& plane,
            int orientation,
            int sliceIndex)
            {
                plane = vtkSmartPointer<vtkImagePlaneWidget>::New();
                plane->SetInteractor(interactor);
                plane->SetInputData(img);
                plane->SetLookupTable(m_lut);
                plane->TextureInterpolateOn();
                plane->SetResliceInterpolateToLinear();
                plane->DisplayTextOff();

                // 设定朝向
                if (orientation == 0) {
                    plane->SetPlaneOrientationToXAxes();
                }
                else if (orientation == 1) {
                    plane->SetPlaneOrientationToYAxes();
                }
                else if (orientation == 2) {
                    plane->SetPlaneOrientationToZAxes();
                }

                plane->SetSliceIndex(sliceIndex);

                // 1) 只保留三种动作：光标、slice 平移
               
                // 左键：沿法向移动 slice
                plane->SetLeftButtonAction(vtkImagePlaneWidget::VTK_SLICE_MOTION_ACTION);

                // 右键只是选中
                plane->SetRightButtonAction(vtkImagePlaneWidget::VTK_CURSOR_ACTION);

                // 2) 把 margin 区域关掉，避免点到边上进入旋转 / 缩放模式
                plane->SetMarginSizeX(0.0);
                plane->SetMarginSizeY(0.0);
                plane->GetMarginProperty()->SetOpacity(0.0);   // 看不见 margin

                // 3) 限制平面在体数据范围内，避免拖出 volume
                plane->RestrictPlaneToVolumeOn();

                plane->On();
            };

        //  X Sagittal
        setupPlane(m_planeX, 0, cx);

        //  Y Coronal
        setupPlane(m_planeY, 1, cy);

        //  Z Axial
        setupPlane(m_planeZ, 2, cz);

        //  3D -> 2D：任意平面交互都触发同步 
        m_cb3D = vtkSmartPointer<vtkCallbackCommand>::New();
        m_cb3D->SetClientData(this);
        m_cb3D->SetCallback([](vtkObject*, unsigned long, void* clientData, void*) {
            static_cast<MprAssembly*>(clientData)->syncFrom3DWidgets();
            });

        m_planeX->AddObserver(vtkCommand::InteractionEvent, m_cb3D);
        m_planeY->AddObserver(vtkCommand::InteractionEvent, m_cb3D);
        m_planeZ->AddObserver(vtkCommand::InteractionEvent, m_cb3D);
    }


	void MprAssembly::syncFrom3DWidgets()//这个函数会被3D平面的交互事件调用
    {
        if (!m_planeX || !m_planeY || !m_planeZ) return;
        if (!m_axialViewer || !m_coronalViewer || !m_sagittalViewer) return;

        const int i = m_planeX->GetSliceIndex(); // sagittal
        const int j = m_planeY->GetSliceIndex(); // coronal
        const int k = m_planeZ->GetSliceIndex(); // axial

        m_sagittalViewer->SetSlice(i);
        m_coronalViewer->SetSlice(j);
        m_axialViewer->SetSlice(k);

        m_sagittalViewer->Render();
        m_coronalViewer->Render();
        m_axialViewer->Render();

        if (m_volumeWindow) {
            m_volumeWindow->Render();
        }
        else if (m_volumeRawWindow) {
            m_volumeRawWindow->Render();
        }
    }

    //
	void MprAssembly::on2DSliceChanged(int axialZ, int coronalY, int sagittalX)//这个函数会被2D视图切片变化调用
    {
        if (m_planeX) {
            m_planeX->SetSliceIndex(sagittalX);
            m_planeX->UpdatePlacement();
            qDebug() << "aaa";
        }
        if (m_planeY) {
            m_planeY->SetSliceIndex(coronalY);
            m_planeY->UpdatePlacement();
            qDebug() << "bbb";
        }
        if (m_planeZ) {
            m_planeZ->SetSliceIndex(axialZ);
            m_planeZ->UpdatePlacement();
            qDebug() << "ccc";
        }

        if (m_volumeWindow) {
            m_volumeWindow->Render();
        }
        else if (m_volumeRawWindow) {
            m_volumeRawWindow->Render();
        }
    }

} // namespace core::mpr
