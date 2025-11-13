#include "core/mpr/mprAssembly.h"
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkResliceCursor.h>
#include <vtkResliceCursorActor.h>
#include <vtkResliceImageViewer.h>
#include <vtkSmartPointer.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkResliceCursorPolyDataAlgorithm.h>
#include <vtkCamera.h>
#include "core/mpr/mprState.h"


//assembly: 装配,组合

namespace core::mpr {

    MprAssembly::MprAssembly() = default;

    MprAssembly::~MprAssembly()
    {
        detach();
    }

    void MprAssembly::attach(QVTKOpenGLNativeWidget* axial, QVTKOpenGLNativeWidget* coronal, QVTKOpenGLNativeWidget* sagittal, QVTKOpenGLNativeWidget* volume3D)
    {
        // 记录四个画布的指针，后续 buildPipelines() 会真正创建管线。
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
        // 兼容旧接口，直接保存 renderWindow 与 interactor。
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
        // 删除所有 VTK 对象并解除与 Qt 画布的关联，避免内存泄漏。
        if (m_axialViewer) {
            m_axialViewer->Delete();
            m_axialViewer = nullptr;
        }
        if (m_coronalViewer) {
            m_coronalViewer->Delete();
            m_coronalViewer = nullptr;
        }
        if (m_sagittalViewer) {
            m_sagittalViewer->Delete();
            m_sagittalViewer = nullptr;
        }

        if (m_cursorActor) {
            m_cursorActor->Delete();
            m_cursorActor = nullptr;
        }
        if (m_volumeMapper) {
            m_volumeMapper->Delete();
            m_volumeMapper = nullptr;
        }
        if (m_volume) {
            m_volume->Delete();
            m_volume = nullptr;
        }
        if (m_volumeProperty) {
            m_volumeProperty->Delete();
            m_volumeProperty = nullptr;
        }
        if (m_renderer3D) {
            m_renderer3D->Delete();
            m_renderer3D = nullptr;
        }

        if (m_axialWidget) {
            m_axialWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }
        if (m_coronalWidget) {
            m_coronalWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }
        if (m_sagittalWidget) {
            m_sagittalWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));//显式类型转换static_cast 
        }
        if (m_volumeWidget) {
            m_volumeWidget->setRenderWindow(static_cast<vtkRenderWindow*>(nullptr));
        }

        if (m_axialWindow) {
            m_axialWindow->Delete();
            m_axialWindow = nullptr;
        }
        if (m_coronalWindow) {
            m_coronalWindow->Delete();
            m_coronalWindow = nullptr;
        }
        if (m_sagittalWindow) {
            m_sagittalWindow->Delete();
            m_sagittalWindow = nullptr;
        }
        if (m_volumeWindow) {
            m_volumeWindow->Delete();
            m_volumeWindow = nullptr;
        }
    }

    void MprAssembly::setState(MprState* state)
    {
        m_state = state;
    }

    void MprAssembly::buildPipelines()
    {
        if (!m_state || !m_state->cursor()) {
            return;
        }

        // 重新创建视图前先释放旧的 VTK 对象。
        if (m_axialViewer || m_coronalViewer || m_sagittalViewer || m_cursorActor) {
            // 仅释放 VTK 对象，保留窗口指针。
            if (m_axialViewer) {
                m_axialViewer->Delete();
                m_axialViewer = nullptr;
            }
            if (m_coronalViewer) {
                m_coronalViewer->Delete();
                m_coronalViewer = nullptr;
            }
            if (m_sagittalViewer) {
                m_sagittalViewer->Delete();
                m_sagittalViewer = nullptr;
            }
            if (m_cursorActor) {
                m_cursorActor->Delete();
                m_cursorActor = nullptr;
            }
            if (m_volumeMapper) {
                m_volumeMapper->Delete();
                m_volumeMapper = nullptr;
            }
            if (m_volume) {
                m_volume->Delete();
                m_volume = nullptr;
            }
            if (m_volumeProperty) {
                m_volumeProperty->Delete();
                m_volumeProperty = nullptr;
            }
            if (m_renderer3D) {
                m_renderer3D->Delete();
                m_renderer3D = nullptr;
            }
        }

        m_axialViewer = vtkResliceImageViewer::New();
        m_coronalViewer = vtkResliceImageViewer::New();
        m_sagittalViewer = vtkResliceImageViewer::New();

        if (m_axialWidget || m_coronalWidget || m_sagittalWidget || m_volumeWidget) {
            setup2DViewer(m_axialViewer, vtkResliceImageViewer::SLICE_ORIENTATION_XY, m_axialWidget, &m_axialWindow);
            setup2DViewer(m_coronalViewer, vtkResliceImageViewer::SLICE_ORIENTATION_XZ, m_coronalWidget, &m_coronalWindow);
            setup2DViewer(m_sagittalViewer, vtkResliceImageViewer::SLICE_ORIENTATION_YZ, m_sagittalWidget, &m_sagittalWindow);
            setup3DScene(m_volumeWidget);
        }
        else {
            setup2DViewerRaw(m_axialViewer, vtkResliceImageViewer::SLICE_ORIENTATION_XY, m_axialRawWindow, m_axialRawInteractor);
            setup2DViewerRaw(m_coronalViewer, vtkResliceImageViewer::SLICE_ORIENTATION_XZ, m_coronalRawWindow, m_coronalRawInteractor);
            setup2DViewerRaw(m_sagittalViewer, vtkResliceImageViewer::SLICE_ORIENTATION_YZ, m_sagittalRawWindow, m_sagittalRawInteractor);
            setup3DSceneRaw(m_volumeRawWindow, m_volumeRawInteractor);
        }
    }

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

    void MprAssembly::setup2DViewer(vtkResliceImageViewer* viewer, int orientation, QVTKOpenGLNativeWidget* widget, vtkGenericOpenGLRenderWindow** windowStorage)
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

    void MprAssembly::setup2DViewerRaw(vtkResliceImageViewer* viewer, int orientation, vtkRenderWindow* window, vtkRenderWindowInteractor* interactor)
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

        auto cam = m_renderer3D->GetActiveCamera();
        if (cam) {
            cam->Zoom(30);  
            m_renderer3D->ResetCameraClippingRange();
        }
    }

    void MprAssembly::setup3DSceneRaw(vtkRenderWindow* window, vtkRenderWindowInteractor* interactor)
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

        auto cam = m_renderer3D->GetActiveCamera();
        if (cam) {
            cam->Zoom(30);  
            m_renderer3D->ResetCameraClippingRange();
           /* auto GetCenter(double[3]);
            if (GetCenter) 
            {
                cam->Zoom(35);
            }*/

        }
    }

} // namespace core::mpr