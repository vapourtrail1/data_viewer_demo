#pragma once

#include <array>

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

namespace core::mpr {

class MprState;

/**
 * @brief MprAssembly 将同一个 ResliceCursor 装配到多个视图管线。
 */
class MprAssembly
{
public:
    MprAssembly();
    ~MprAssembly();

    void attach(QVTKOpenGLNativeWidget* axial, QVTKOpenGLNativeWidget* coronal, QVTKOpenGLNativeWidget* sagittal, QVTKOpenGLNativeWidget* volume3D);
    void attachRaw(vtkRenderWindow* axialWindow,
        vtkRenderWindowInteractor* axialInteractor,
        vtkRenderWindow* coronalWindow,
        vtkRenderWindowInteractor* coronalInteractor,
        vtkRenderWindow* sagittalWindow,
        vtkRenderWindowInteractor* sagittalInteractor,
        vtkRenderWindow* volumeWindow,
        vtkRenderWindowInteractor* volumeInteractor);
    void detach();

    void setState(MprState* state);
    void buildPipelines();
    void refreshAll();

    vtkResliceImageViewer* axialViewer() const;
    vtkResliceImageViewer* coronalViewer() const;
    vtkResliceImageViewer* sagittalViewer() const;
    vtkResliceCursorActor* cursorActor() const;
    vtkVolumeProperty* volumeProperty() const;

private:
    void setup2DViewer(vtkResliceImageViewer* viewer, int orientation, QVTKOpenGLNativeWidget* widget, vtkGenericOpenGLRenderWindow** windowStorage);
    void setup2DViewerRaw(vtkResliceImageViewer* viewer, int orientation, vtkRenderWindow* window, vtkRenderWindowInteractor* interactor);
    void setup3DScene(QVTKOpenGLNativeWidget* widget);
    void setup3DSceneRaw(vtkRenderWindow* window, vtkRenderWindowInteractor* interactor);

    MprState* m_state = nullptr;

    QVTKOpenGLNativeWidget* m_axialWidget = nullptr;
    QVTKOpenGLNativeWidget* m_coronalWidget = nullptr;
    QVTKOpenGLNativeWidget* m_sagittalWidget = nullptr;
    QVTKOpenGLNativeWidget* m_volumeWidget = nullptr;

    vtkRenderWindow* m_axialRawWindow = nullptr;
    vtkRenderWindowInteractor* m_axialRawInteractor = nullptr;
    vtkRenderWindow* m_coronalRawWindow = nullptr;
    vtkRenderWindowInteractor* m_coronalRawInteractor = nullptr;
    vtkRenderWindow* m_sagittalRawWindow = nullptr;
    vtkRenderWindowInteractor* m_sagittalRawInteractor = nullptr;
    vtkRenderWindow* m_volumeRawWindow = nullptr;
    vtkRenderWindowInteractor* m_volumeRawInteractor = nullptr;

    vtkGenericOpenGLRenderWindow* m_axialWindow = nullptr;
    vtkGenericOpenGLRenderWindow* m_coronalWindow = nullptr;
    vtkGenericOpenGLRenderWindow* m_sagittalWindow = nullptr;
    vtkGenericOpenGLRenderWindow* m_volumeWindow = nullptr;

    vtkResliceImageViewer* m_axialViewer = nullptr;
    vtkResliceImageViewer* m_coronalViewer = nullptr;
    vtkResliceImageViewer* m_sagittalViewer = nullptr;

    vtkResliceCursorActor* m_cursorActor = nullptr;
    vtkSmartVolumeMapper* m_volumeMapper = nullptr;
    vtkVolume* m_volume = nullptr;
    vtkVolumeProperty* m_volumeProperty = nullptr;
    vtkRenderer* m_renderer3D = nullptr;
};

} // namespace core::mpr
