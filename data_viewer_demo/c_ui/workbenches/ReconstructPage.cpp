#include "ReconstructPage.h"
#include <QGridLayout>
#include <QWidget>

#if USE_VTK
#include <QVTKOpenGLNativeWidget.h>
#endif

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
#endif

    viewReserved_ = new QWidget(this);
    viewReserved_->setStyleSheet("background:#1a1a1a; border:1px dashed #333;");

    grid->addWidget(viewAxial_, 0, 0);
    grid->addWidget(viewSagittal_, 1, 0);
    grid->addWidget(viewCoronal_, 0, 1);
    grid->addWidget(viewReserved_, 1, 1);
}
