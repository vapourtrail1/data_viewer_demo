#include "c_ui/workbenches/PerformancePage.h"
#include "core/services/OrthogonalMprService.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>

#if USE_VTK
#include <QVTKOpenGLNativeWidget.h>
#endif

PerformancePage::PerformancePage(QWidget* parent)
    : QWidget(parent)
{
    buildUi();
}

PerformancePage::~PerformancePage() = default;

void PerformancePage::buildUi()
{
    setObjectName(QStringLiteral("performancePage"));
    setStyleSheet(QStringLiteral(
        "QWidget#performancePage{background-color:#1c1c1c;}"
        "QLabel{color:#f0f0f0;}"));

    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(12, 12, 12, 12);
    rootLayout->setSpacing(8);

    // ---- 四视图区域：2x2 网格 ----
    auto* viewsFrame = new QFrame(this);
    viewsFrame->setObjectName(QStringLiteral("performanceViews"));
    viewsFrame->setStyleSheet(QStringLiteral(
        "QFrame#performanceViews{background-color:#202020; border-radius:8px;}"));

    auto* grid = new QGridLayout(viewsFrame);
    grid->setContentsMargins(10, 10, 10, 10);
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(8);

#if USE_VTK
    // 使用 QVTKOpenGLNativeWidget 作为真实的渲染容器。
    viewAxial_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewSagittal_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewCoronal_ = new QVTKOpenGLNativeWidget(viewsFrame);
    viewVolume_ = new QVTKOpenGLNativeWidget(viewsFrame);
#else
    // 若未启用 VTK，使用占位控件提醒用户。
    auto makePlaceholder = [viewsFrame](const QString& text) {
        auto* holder = new QWidget(viewsFrame);
        holder->setStyleSheet(QStringLiteral("background:#161616; border:1px dashed #444;"));
        auto* layout = new QVBoxLayout(holder);
        layout->setContentsMargins(4, 4, 4, 4);
        auto* label = new QLabel(text, holder);
        label->setAlignment(Qt::AlignCenter);
        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
        return holder;
        };
    viewAxial_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示轴状面"));
    viewSagittal_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示矢状面"));
    viewCoronal_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示冠状面"));
    viewVolume_ = makePlaceholder(QStringLiteral("需要启用 VTK 才能显示体渲染"));
#endif

    grid->addWidget(viewAxial_, 0, 0);
    grid->addWidget(viewSagittal_, 0, 1);
    grid->addWidget(viewCoronal_, 1, 0);
    grid->addWidget(viewVolume_, 1, 1);

    rootLayout->addWidget(viewsFrame, 1);
}

