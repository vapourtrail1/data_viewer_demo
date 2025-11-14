#include "core/mpr/mprAssembly.h"
#include "core/mpr/mprState.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
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

        // 清空 Qt Widget 模式
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

        if (m_cursorActor) { m_cursorActor->Delete();   m_cursorActor = nullptr; }
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
        if (m_cursorActor) { m_cursorActor->Delete();   m_cursorActor = nullptr; }
        if (m_volumeMapper) { m_volumeMapper->Delete();  m_volumeMapper = nullptr; }
        if (m_volume) { m_volume->Delete();        m_volume = nullptr; }
        if (m_volumeProperty) { m_volumeProperty->Delete(); m_volumeProperty = nullptr; }
        if (m_renderer3D) { m_renderer3D->Delete();    m_renderer3D = nullptr; }

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
        // 裸 VTK 模式
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
    }

    // 工具：2D / 3D 管线 

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

        m_cursorActor = vtkResliceCursorActor::New();
        m_cursorActor->GetCursorAlgorithm()->SetResliceCursor(m_state->cursor());

        m_volumeMapper = vtkSmartVolumeMapper::New();
        m_volumeMapper->SetInputData(m_state->image());

        m_volumeProperty = vtkVolumeProperty::New();
        m_volumeProperty->ShadeOn();
        m_volumeProperty->SetInterpolationTypeToLinear();

        m_volume = vtkVolume::New();
        m_volume->SetMapper(m_volumeMapper);
        m_volume->SetProperty(m_volumeProperty);

        m_renderer3D->AddActor(m_cursorActor);
        m_renderer3D->AddVolume(m_volume);
        m_renderer3D->ResetCamera();

		auto* camera = m_renderer3D->GetActiveCamera();
        if (camera) {
            camera->Zoom(30);
			m_renderer3D->ResetCameraClippingRange();//放大厚重新设置裁剪范围
        }
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

        m_cursorActor = vtkResliceCursorActor::New();
        m_cursorActor->GetCursorAlgorithm()->SetResliceCursor(m_state->cursor());

        m_volumeMapper = vtkSmartVolumeMapper::New();
        m_volumeMapper->SetInputData(m_state->image());

        m_volumeProperty = vtkVolumeProperty::New();
        m_volumeProperty->ShadeOn();
        m_volumeProperty->SetInterpolationTypeToLinear();

        m_volume = vtkVolume::New();
        m_volume->SetMapper(m_volumeMapper);
        m_volume->SetProperty(m_volumeProperty);

        m_renderer3D->AddActor(m_cursorActor);
        m_renderer3D->AddVolume(m_volume);
        m_renderer3D->ResetCamera();

		auto carema = m_renderer3D->GetActiveCamera();
        if (carema) {
			carema->Zoom(30);
            m_renderer3D->ResetCameraClippingRange();
        }
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

    vtkResliceCursorActor* MprAssembly::cursorActor() const
    {
        return m_cursorActor;
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

        //  X（Sagittal）
        m_planeX = vtkSmartPointer<vtkImagePlaneWidget>::New();
        m_planeX->SetInteractor(interactor);
        m_planeX->SetInputData(img);
        m_planeX->SetLookupTable(m_lut);
        m_planeX->TextureInterpolateOn();
        m_planeX->SetResliceInterpolateToLinear();
        m_planeX->DisplayTextOff();
        m_planeX->SetPlaneOrientationToXAxes();
        m_planeX->SetSliceIndex(cx);
        m_planeX->On();

        //  Y（Coronal）
        m_planeY = vtkSmartPointer<vtkImagePlaneWidget>::New();
        m_planeY->SetInteractor(interactor);
        m_planeY->SetInputData(img);
        m_planeY->SetLookupTable(m_lut);
        m_planeY->TextureInterpolateOn();
        m_planeY->SetResliceInterpolateToLinear();
        m_planeY->DisplayTextOff();
        m_planeY->SetPlaneOrientationToYAxes();
        m_planeY->SetSliceIndex(cy);
        m_planeY->On();

        //  Z（Axial）
        m_planeZ = vtkSmartPointer<vtkImagePlaneWidget>::New();
        m_planeZ->SetInteractor(interactor);
        m_planeZ->SetInputData(img);
        m_planeZ->SetLookupTable(m_lut);
        m_planeZ->TextureInterpolateOn();
        m_planeZ->SetResliceInterpolateToLinear();
        m_planeZ->DisplayTextOff();
        m_planeZ->SetPlaneOrientationToZAxes();
        m_planeZ->SetSliceIndex(cz);
        m_planeZ->On();

        // —— 3D -> 2D：任意平面交互都触发同步 —— 
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

        // 若你的三视图朝向定义不同，请在此做映射调整
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

	void MprAssembly::on2DSliceChanged(int axialZ, int coronalY, int sagittalX)//这个函数会被2D视图切片变化调用
    {
        if (m_planeX) {
            m_planeX->SetSliceIndex(sagittalX);
            m_planeX->UpdatePlacement();
        }
        if (m_planeY) {
            m_planeY->SetSliceIndex(coronalY);
            m_planeY->UpdatePlacement();
        }
        if (m_planeZ) {
            m_planeZ->SetSliceIndex(axialZ);
            m_planeZ->UpdatePlacement();
        }

        if (m_volumeWindow) {
            m_volumeWindow->Render();
        }
        else if (m_volumeRawWindow) {
            m_volumeRawWindow->Render();
        }
    }

} // namespace core::mpr
