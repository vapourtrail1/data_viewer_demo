#include "ReconstructPage.h"
#include <QGridLayout>
#include <QWidget>
#include "core/common/VtkMacros.h" 

#if USE_VTK
#include <QVTKOpenGLNativeWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#endif

#include "core/services/OrthogonalMprService.h"

ReconstructPage::ReconstructPage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
}

void ReconstructPage::buildUi()
{
    auto grid = new QGridLayout(this);
    grid->setContentsMargins(6, 6, 6, 6);
    grid->setHorizontalSpacing(6);
    grid->setVerticalSpacing(6);

#if USE_VTK
    viewAxial_ = new QVTKOpenGLNativeWidget(this);
    viewSagittal_ = new QVTKOpenGLNativeWidget(this);
    viewCoronal_ = new QVTKOpenGLNativeWidget(this);
    viewReserved_ = new QVTKOpenGLNativeWidget(this);
#else
    auto makePlaceholder = [this](const QString& name) {
        auto w = new QWidget(this);
        w->setObjectName(name);
        w->setStyleSheet("background:#111; border:1px solid #222;");
        return w;
        };
    viewAxial_ = makePlaceholder(QStringLiteral("viewAxial"));
    viewSagittal_ = makePlaceholder(QStringLiteral("viewSagittal"));
    viewCoronal_ = makePlaceholder(QStringLiteral("viewCoronal"));
    viewReserved_ = new QWidget(this);
    viewReserved_->setStyleSheet("background:#1a1a1a; border:1px dashed #333;");
#endif
    grid->addWidget(viewAxial_, 0, 0);
    grid->addWidget(viewSagittal_, 1, 0);
    grid->addWidget(viewCoronal_, 0, 1);
    grid->addWidget(viewReserved_, 1, 1);
}

//提供视图窗口
bool ReconstructPage::initializeWithService(core::services::OrthogonalMprService* service)
{
#if !USE_VTK
    Q_UNUSED(service);
    return false;
#else
    // 判空
    if (!service || !viewAxial_ || !viewCoronal_ || !viewSagittal_ || !viewReserved_) {
        return false;
    }

    //把四个 QVTKOpenGLNativeWidget 交给 service
    service->attachWidgets(
        qobject_cast<QVTKOpenGLNativeWidget*>(viewAxial_),
        qobject_cast<QVTKOpenGLNativeWidget*>(viewCoronal_),
        qobject_cast<QVTKOpenGLNativeWidget*>(viewSagittal_),
        qobject_cast<QVTKOpenGLNativeWidget*>(viewReserved_)
    );

	
    auto* axialWindow = qobject_cast<QVTKOpenGLNativeWidget*>(viewAxial_)->renderWindow();
    auto* coronalWindow = qobject_cast<QVTKOpenGLNativeWidget*>(viewCoronal_)->renderWindow();
    auto* sagittalWindow = qobject_cast<QVTKOpenGLNativeWidget*>(viewSagittal_)->renderWindow();
    auto* volumeWindow = qobject_cast<QVTKOpenGLNativeWidget*>(viewReserved_)->renderWindow();

    //将渲染窗口和交互器给service->initializeViewers
    const bool initialized = service->initializeViewers(
        axialWindow, axialWindow ? axialWindow->GetInteractor() : nullptr,
        sagittalWindow, sagittalWindow ? sagittalWindow->GetInteractor() : nullptr,
        coronalWindow, coronalWindow ? coronalWindow->GetInteractor() : nullptr,
        volumeWindow, volumeWindow ? volumeWindow->GetInteractor() : nullptr
        );

    // 初始化成功，触发一次渲染刷新
    if (initialized && volumeWindow) {
        volumeWindow->Render();
    }

    return initialized;
#endif
}
