#include "c_ui/workbenches/VolumePage.h"
#include "c_ui/action/ActionIDs.h"
#include <QGridLayout>
#include <QWidget>

VolumePage::VolumePage(QWidget* parent) : PageBase(parent) {
    buildUi();
}

QList<MenuSpec> VolumePage::menus() const {
    return {
        { QStringLiteral("文件"),   { Act_Open, Act_Save, Act_SaveAs, Act_Export } },
        { QStringLiteral("开始"),   { Act_CTReconstruct, Act_Import, Act_Export } },
        { QStringLiteral("编辑"),   { Act_Undo, Act_Redo } },
        { QStringLiteral("体积"),   { Act_VolumeCrop } },
        { QStringLiteral("视图"),   { Act_ResetView, Act_FitAll } },
        { QStringLiteral("首选项"), { Act_Preferences } }
    };
}

void VolumePage::buildUi() {
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
        w->setStyleSheet("background:#111;border:1px solid #222;");
        return w;
        };
    viewAxial_ = makePlaceholder("viewAxial");
    viewSagittal_ = makePlaceholder("viewSagittal");
    viewCoronal_ = makePlaceholder("viewCoronal");
#endif
    auto placeholder = new QWidget(this);
    placeholder->setStyleSheet("background:#1a1a1a;border:1px dashed #333;");

    grid->addWidget(viewAxial_, 0, 0);
    grid->addWidget(viewSagittal_, 1, 0);
    grid->addWidget(viewCoronal_, 0, 1);
    grid->addWidget(placeholder, 1, 1);
}
