#include "app/App.h"

#include <QApplication>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QLoggingCategory>
#include <QPalette>
#include <QStyleFactory>

#include "app/Settings.h"
#include "c_ui/MainWindow.h"
#include "core/common/Logging.h"

namespace app {

App::App() = default;

int App::run(int argc, char** argv)
{
    // 创建并持有 QApplication 对象，确保 Qt 事件循环存在。
    QApplication qtApp(argc, argv);

    // 启动前设置日志过滤和 UI 样式，使得调试输出及界面外观一致。
    initializeLogging();
    applyGlobalStyle();

    // 读取应用级设置（最近文件、默认窗宽窗位等）。
    Settings settings;
    settings.load();

    // 前端主窗口由前端代码提供，这里只负责展示。
    CTViewer mainWindow;
    mainWindow.show();

    const int exitCode = qtApp.exec();

    // 关闭时将最新设置写回磁盘。
    settings.save();

    return exitCode;
}

void App::initializeLogging() const
{
    // 统一开启所有日志级别，方便调试阶段快速定位问题。
    QLoggingCategory::setFilterRules(QStringLiteral("*.debug=true\n*.info=true\n*.warning=true\n*.critical=true"));
}

void App::applyGlobalStyle() const
{
    // 使用 Fusion 样式保证跨平台外观一致。
    QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));

    // 构建深色调配色方案，减轻夜间使用疲劳。
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(30, 30, 30));
    palette.setColor(QPalette::WindowText, QColor(220, 220, 220));
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(30, 30, 30));
    palette.setColor(QPalette::Text, QColor(220, 220, 220));
    palette.setColor(QPalette::Button, QColor(45, 45, 45));
    palette.setColor(QPalette::ButtonText, QColor(220, 220, 220));
    palette.setColor(QPalette::BrightText, QColor(255, 0, 0));
    palette.setColor(QPalette::Highlight, QColor(66, 133, 244));
    palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    QApplication::setPalette(palette);

    // 预加载常用字体，保证跨平台字体一致性。
    QFontDatabase::addApplicationFont(QStringLiteral(":/fonts/SourceHanSansCN-Regular.otf"));
}

} // namespace app
