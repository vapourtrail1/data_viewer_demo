#pragma once
#include "c_ui/PageBase.h"
#include <QPointer>

class QWidget;
#if USE_VTK
class QVTKOpenGLNativeWidget;
#endif

class VolumePage : public PageBase {
    Q_OBJECT
public:
    explicit VolumePage(QWidget* parent = nullptr);
    QList<MenuSpec> menus() const override;

private:
    void buildUi();
#if USE_VTK
    QPointer<QVTKOpenGLNativeWidget> viewAxial_;
    QPointer<QVTKOpenGLNativeWidget> viewSagittal_;
    QPointer<QVTKOpenGLNativeWidget> viewCoronal_;
#else
    QPointer<QWidget> viewAxial_;
    QPointer<QWidget> viewSagittal_;
    QPointer<QWidget> viewCoronal_;
#endif
};
