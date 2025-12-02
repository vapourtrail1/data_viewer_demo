#include "StartPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QPixmap>
#include <QList>
#include <QSize>
#include <QDebug>
#include <QFile>

//static是作用域限定符，表示该函数仅在当前文件内可见，防止命名冲突
//辅助函数控制换行
static QString wrapByWidth(const QString& s, const QFont& font, int maxWidthPx) {//第三个参数为一行允许的最大像素宽度
    QFontMetrics fm(font); //给出这个字体下每个字符或者字符串的像素宽度。
    QString out;
    int lineWidth = 0;//当前行的已占用的像素宽度累计

    auto flushLineBreak = [&]() { out += QChar('\n');
    lineWidth = 0; };

    for (int i = 0; i < s.size(); ++i) {
        const QChar ch = s.at(i);//获得指定位置的字符
        int w = fm.horizontalAdvance(ch);//该字符在当前字体下的像素宽度

        // 优先在自然断点处换行
        bool isBreakable = (ch.isSpace() || ch == '/' || ch == '·' || ch == '、');
        if (lineWidth + w > maxWidthPx) {
            if (!out.isEmpty())
            {
                flushLineBreak();
            }
        }
        out += ch;
        lineWidth += w;
        if (isBreakable) {
            if (lineWidth > maxWidthPx * 0.85)
            {
                flushLineBreak();
            }
        }
    }
    return out;
}

// 辅助函数  根据按钮文本加载对应图标
static QIcon loadIconFor(const QString& text) {
    struct Map {
        QString key; //避免编码转换 直接用QString
        const char* file;
    };
    static const Map map[] = {
        { QStringLiteral("快速导入"),  ":/start_icons01/icons_other/start_icons/quick_input.png" },
        { QStringLiteral("体积导入"),  ":/start_icons01/icons_other/start_icons/reload_volume_data.png" },
        { QStringLiteral("显示模式"), ":/start_icons01/icons_other/start_icons/display_pattern.png" },
        { QStringLiteral("水平/窗口模式"),  ":/start_icons01/icons_other/start_icons/shuipingchuangkou.png" },
        { QStringLiteral("厚板"),  ":/start_icons01/icons_other/start_icons/thick_board.PNG" },
        { QStringLiteral("裁剪当前切片图"),  ":/start_icons01/icons_other/start_icons/clip_plane.png" },
        { QStringLiteral("对齐"),  ":/start_icons01/icons_other/start_icons/simple_align.png" },
        { QStringLiteral("指示器"),  ":/start_icons01/icons_other/start_icons/indicator.PNG" },
        { QStringLiteral("距离"),  ":/start_icons01/icons_other/start_icons/distance.PNG" },
        { QStringLiteral("角度(4个点)"),      ":/start_icons01/icons_other/start_icons/angle_4points.PNG" },
        { QStringLiteral("角度(3个点)"),        ":/start_icons01/icons_other/start_icons/angle_3points.PNG" },
        { QStringLiteral("折线长度"),        ":/start_icons01/icons_other/start_icons/fold_line_length.PNG" },
        { QStringLiteral("最小/最大距离"),        ":/start_icons01/icons_other/start_icons/max_min_distance.PNG" },
        { QStringLiteral("卡尺"),":/start_icons01/icons_other/start_icons/caliper.PNG" },
        { QStringLiteral("捕捉模式"),":/start_icons01/icons_other/start_icons/capture_pattern.PNG" },
        { QStringLiteral("重新捕捉量具控点"),  ":/start_icons01/icons_other/start_icons/re_capture_measure_tool_control_point.PNG" },
        { QStringLiteral("创建报告"),  ":/start_icons01/icons_other/start_icons/create_report.PNG" },
        { QStringLiteral("创建书签"),  ":/start_icons01/icons_other/start_icons/create_bookmark.PNG" },
        { QStringLiteral("书签编辑器"),  ":/start_icons01/icons_other/start_icons/bookmark_editor.PNG" },
        { QStringLiteral("保存图像/影片"),  ":/start_icons01/icons_other/start_icons/save_image.png" },
    };

    for (const auto& m : map) {
        if (text == m.key) {
            const QString path = QString::fromUtf8(m.file);
            qDebug() << "use path =" << path << ", is exist? =" << QFile(path).exists();
            QIcon ico(path);//用给定的路径 创建一个Qicon对象
            if (!ico.isNull()) {
                return ico;//
            }
        }
    }
    return QIcon(":/icons/icons/move.png");
}


StartPagePage::StartPagePage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("pageStart"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageStart{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout01 = new QVBoxLayout(this);
    layout01->setContentsMargins(0, 0, 0, 0);
    layout01->setSpacing(3);

    // 功能区调用
    layout01->addWidget(buildRibbon01(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    /*auto* title = new QLabel(QStringLiteral("开始功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);*/

    // 用于挂载 2x2 视口的容器
    auto* viewerHost = new QFrame(placeholder);
    viewerHost->setObjectName(QStringLiteral("viewerHost"));
    viewerHost->setStyleSheet(QStringLiteral(
        "QFrame#viewerHost{background:#111; border:1px solid #222; border-radius:6px;}"
    ));

    auto* hostLayout = new QVBoxLayout(viewerHost);
    hostLayout->setContentsMargins(0, 0, 0, 0);
    hostLayout->setSpacing(0);

    placeholderLayout->addWidget(viewerHost, 1);
    
	// 占位区填充到主布局中
    layout01->addWidget(placeholder, 1);
}

QWidget* StartPagePage::buildRibbon01(QWidget* parent)
{
    // 创建功能区容器
    auto* ribbon01_ = new QFrame(parent);
    ribbon01_->setObjectName(QStringLiteral("startRibbon"));
    ribbon01_->setStyleSheet(QStringLiteral(
        "QFrame#startRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout01 = new QHBoxLayout(ribbon01_);
    layout01->setContentsMargins(4, 4, 4, 4);
    layout01->setSpacing(1);

    struct RibbonAction01
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction01> actions01 = {
        { QStringLiteral("快速导入"), 0 },
        { QStringLiteral("体积导入"), 1 },
        { QStringLiteral("显示模式"), 2 },
        { QStringLiteral("水平/窗口模式"), 0 },
        { QStringLiteral("厚板"), 0 },
        { QStringLiteral("裁剪当前切片图"), 0 },
        { QStringLiteral("对齐"), 3 },
        { QStringLiteral("指示器"), 0 },
        { QStringLiteral("距离"), 0 },
        { QStringLiteral("角度(4个点)"), 0 },
        { QStringLiteral("角度(3个点)"), 0 },
        { QStringLiteral("折线长度"), 0 },
        { QStringLiteral("最小/最大距离"), 0 },
        { QStringLiteral("卡尺"), 0 },
        { QStringLiteral("捕捉模式"), 4 },
        { QStringLiteral("重新捕捉量具控点"), 0 },
        { QStringLiteral("创建报告"), 0 },
        { QStringLiteral("创建书签"), 0 },
        { QStringLiteral("书签编辑器"), 0 },
        { QStringLiteral("保存图像/影片"), 5 },
    };

    for (const auto& action : actions01) {
        // 每个功能都使用图标,文字的形式展示
        auto* button = new QToolButton(ribbon01_);
        QString wrappedText = wrapByWidth(action.text, button->font(), 51);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));

        if (action.text == QStringLiteral("距离")) {
            // “距离”按钮点击时发出信号，交由主窗口调用后端测距逻辑
            connect(button, &QToolButton::clicked, this, [this]() {
                emit distanceRequested();
                });
        }

        if (action.hasMenu == 1) {  
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/start_icons02/icons_other/start_icons/volume_input_pull_down_menu/origin_volume.png"), QStringLiteral("原始体积"));
            menu->addAction(QIcon(":/start_icons02/icons_other/start_icons/volume_input_pull_down_menu/images_stack.png"), QStringLiteral("图像堆栈"));
            menu->addAction(QIcon(":/start_icons02/icons_other/start_icons/volume_input_pull_down_menu/merge_obj.png"), QStringLiteral("合并对象"));
            menu->addAction(QIcon(":/start_icons02/icons_other/start_icons/volume_input_pull_down_menu/CT_rebuild.png"), QStringLiteral("CT重建"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        if (action.hasMenu == 2) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/display_pattern_pull_down_menu/display_pattern.png"), QStringLiteral("原始"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/display_pattern_pull_down_menu/color.png"), QStringLiteral("颜色"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/delete_volume_data.png"), QStringLiteral("颜色和不透明度"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        if (action.hasMenu == 3) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/best_fit_align.png"), QStringLiteral("最佳拟合对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/3-2-1_align.png"), QStringLiteral("3-2-1对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/based_on_feature_align.png"), QStringLiteral("基于特征的对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/in_order_align.png"), QStringLiteral("按次序对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/RPS_align.png"), QStringLiteral("RPS对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/based_on_geometry_element_best_fit.png"), QStringLiteral("基于几何元素的最佳拟合"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/simple_3-2-1_align.png"), QStringLiteral("简单3-2-1对齐"));
            menu02->addAction(QIcon(":/start_icons02/icons_other/start_icons/align_pull_down_menu/simple_align.png"), QStringLiteral("简单对齐"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        if (action.hasMenu == 4) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/min.PNG"), QStringLiteral("最小"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/max.PNG"), QStringLiteral("最大"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/gradient.PNG"), QStringLiteral("梯度"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/surface.PNG"), QStringLiteral("表面"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/local.PNG"), QStringLiteral("局部"));
            menu02->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/off.PNG"), QStringLiteral("关"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        if (action.hasMenu == 5) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/start_icons01/icons_other/start_icons/save_image.png"), QStringLiteral("保存图像"));
            menu02->addAction(QIcon(":/start_icons01/icons_other/start_icons/save_image.png"), QStringLiteral("保存影片/图像堆栈"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        layout01->addWidget(button);
    }
    layout01->addStretch();
    return ribbon01_;
}

//void StartPagePage::openDicomDistance() {
//    if (!ribbon01_) {
//        return;
//    }
//
//    connect(ribbon01_, &QPushButton::clicked, this, [this](QFrame* frame) {
//		const QString action = frame->property("action").toString();   
//        if (action == "距离")
//        {
//			emit dicomDistance(action);
//        }
//        });
//}

//QIcon EditPage::buildIcon() const
//{
//    // 创建一个灰色的方形占位图标，后续替换为真实资源
//    QPixmap pixmap(48, 48);
//    pixmap.fill(Qt::transparent);
//
//    QPainter painter(&pixmap);
//    painter.setRenderHint(QPainter::Antialiasing);
//    painter.setBrush(QColor(QStringLiteral("#4a4a4a")));
//    painter.setPen(QPen(QColor(QStringLiteral("#6c6c6c")), 2));
//    painter.drawRoundedRect(pixmap.rect().adjusted(3, 3, -3, -3), 8, 8);
//
//    painter.setPen(QPen(QColor(QStringLiteral("#bdbdbd"))));
//    painter.setFont(QFont(QStringLiteral("Microsoft YaHei"), 8, QFont::Bold));
//    painter.drawText(pixmap.rect(), Qt::AlignCenter, QStringLiteral("ICON"));
//
//    painter.end();
//    return QIcon(pixmap);
//}