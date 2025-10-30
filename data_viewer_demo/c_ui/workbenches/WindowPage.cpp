#include "WindowPage.h"
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
#include <QCheckBox>
#include <QWidgetAction>



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
        { QStringLiteral("四分"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/four.PNG" },
        { QStringLiteral("切片图靠左"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/slice_left.PNG" },
        { QStringLiteral("切片图靠右"), ":/window_icons02/icons_other/window_icons/layout_section_icon_group/slice_right.PNG" },
        { QStringLiteral("切片图靠下"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/slice_down.PNG" },
        { QStringLiteral("旋转"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/spin.PNG" },
        { QStringLiteral("四分，分布图"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/four__.PNG" },
        { QStringLiteral("非平面"),  ":/window_icons02/icons_other/window_icons/layout_section_icon_group/non_plane.PNG" },
        { QStringLiteral("布局编辑器"),  ":/window_icons/icons_other/window_icons/layout_editor.PNG" },
        { QStringLiteral("2D背景"),  ":/window_icons/icons_other/window_icons/2D_background.png" },
        { QStringLiteral("3D背景"),      ":/window_icons/icons_other/window_icons/3D_background.png" },
        { QStringLiteral("放大"),        ":/window_icons/icons_other/window_icons/zoom_in.PNG" },
        { QStringLiteral("缩小"),        ":/window_icons/icons_other/window_icons/zoom_out.PNG" },
        { QStringLiteral("体素分辨率"),        ":/window_icons/icons_other/window_icons/voxel_resolution.PNG" },
        { QStringLiteral("设置缩放比例"),":/window_icons/icons_other/window_icons/set_zoom_ratio.PNG" },
        { QStringLiteral("调整对象以适应窗口"),":/window_icons/icons_other/window_icons/fit_obj_to_window.PNG" },
        { QStringLiteral("调整场景以适应窗口"),  ":/window_icons/icons_other/window_icons/fit_scene_to_window.PNG" },
        { QStringLiteral("重置"),  ":/window_icons/icons_other/window_icons/reset_zoom.PNG" },
        { QStringLiteral("显示模式"),  ":/window_icons02/icons_other/window_icons/display_pattern_pull_down_menu/display_pattern.png" },
        { QStringLiteral("厚板"),  ":/window_icons/icons_other/window_icons/thick_board.PNG" },
        { QStringLiteral("切片图步宽"),  ":/window_icons/icons_other/window_icons/slice_step_size.PNG" },
        { QStringLiteral("水平/窗口模式"),  ":/window_icons/icons_other/window_icons/horizontal_window_pattern.png" },
        { QStringLiteral("标尺"),  ":/window_icons/icons_other/window_icons/ruler.PNG" },
        { QStringLiteral("注解"),  ":/window_icons02/icons_other/window_icons/annotation_pull_down_menu/annotation.png" },
        { QStringLiteral("文本叠加层编辑器"),  ":/window_icons/icons_other/window_icons/text_overlay_layer_editor.png" },
        { QStringLiteral("工具"),  ":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG" },
        { QStringLiteral("工具停靠栏"),  ":/window_icons/icons_other/window_icons/toolbar_docking.PNG" },
        { QStringLiteral("重置工具"),  ":/window_icons/icons_other/window_icons/reset_tool.PNG" },
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


WindowPage::WindowPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("windowEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageWindow{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout12 = new QVBoxLayout(this);
    layout12->setContentsMargins(0, 0, 0, 0);
    layout12->setSpacing(3);

    // 功能区调用
    layout12->addWidget(buildRibbon12(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("选择功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout12->addWidget(placeholder, 1);
}

QWidget* WindowPage::buildRibbon12(QWidget* parent)
{
    auto* ribbon12 = new QFrame(parent);//ribbon12是--功能区的容器
    ribbon12->setObjectName(QStringLiteral("measureRibbon"));
    ribbon12->setStyleSheet(QStringLiteral(
        "QFrame#measureRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout12 = new QHBoxLayout(ribbon12);
    layout12->setContentsMargins(4, 4, 4, 4);
    layout12->setSpacing(1);

    struct RibbonAction12 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction12> actions12 = {
        { QStringLiteral("四分"), 0 },
        { QStringLiteral("切片图靠左"), 0 },
        { QStringLiteral("切片图靠右"), 0 },
        { QStringLiteral("切片图靠下"), 0 },
        { QStringLiteral("旋转"), 0 },
        { QStringLiteral("四分，分布图"), 0 },
        { QStringLiteral("非平面"), 0 },
        { QStringLiteral("布局编辑器"), 0 },
        { QStringLiteral("2D背景"), 0 },
        { QStringLiteral("3D背景"), 0 },
        { QStringLiteral("放大"), 0 },
        { QStringLiteral("缩小"), 0 },
        { QStringLiteral("体素分辨率"), 0 },
        { QStringLiteral("设置缩放比例"), 0 },
        { QStringLiteral("调整对象以适应窗口"), 0 },
        { QStringLiteral("调整场景以适应窗口"), 0 },
        { QStringLiteral("重置"), 0 },
        { QStringLiteral("显示模式"), 1 },
        { QStringLiteral("厚板"), 0 },
        { QStringLiteral("切片图步宽"), 0 },
        { QStringLiteral("水平/窗口模式"), 0 },
        { QStringLiteral("标尺"), 0 },
        { QStringLiteral("注解"), 2 },
        { QStringLiteral("文本叠加层编辑器"), 0 },
        { QStringLiteral("工具"), 3 },
        { QStringLiteral("工具停靠栏"), 4 },
        { QStringLiteral("重置工具"), 0 },
    };

    // 需要做成两行四列的那 8 个按钮
    const QStringList twoRowGroup = {
         QStringLiteral("四分"), 
         QStringLiteral("切片图靠左"), 
         QStringLiteral("切片图靠右"),
         QStringLiteral("切片图靠下"),
         QStringLiteral("旋转"), 
         QStringLiteral("四分，分布图"),
         QStringLiteral("非平面"), 
        // 如果你想把“椭圆3D”也放进来，把上面行数调成 2×5 或替换掉其中一个即可
    };



    QWidget* gridHolder_12 = nullptr;//这个指针的意思是 用来承载那个 2×4 的小方阵
    QGridLayout* grid_12 = nullptr;//这个指针是用来管理那个小方阵的布局
    int groupedCount_12 = 0;//记录已经放进小方阵的按钮数量


    for (const auto& action : actions12)
    {
        const bool inGroup_12 = twoRowGroup.contains(action.text);//contains函数检查某个元素是否在列表中 返回true或false
        auto* button = new QToolButton(ribbon12);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));
        if (inGroup_12)
        {
            if (!gridHolder_12) {//等价于gridholder == nullptr
                gridHolder_12 = new QWidget(ribbon12);
                grid_12 = new QGridLayout(gridHolder_12);
                grid_12->setContentsMargins(4, 2, 4, 2);
                grid_12->setHorizontalSpacing(8);//水平间距
                grid_12->setVerticalSpacing(4);//垂直间距
                layout12->addWidget(gridHolder_12); // 把小方阵插入到主 ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
            button->setMinimumSize(QSize(90, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);

            int row = groupedCount_12 / 4;
            int col = groupedCount_12 % 4;
            grid_12->addWidget(button, row, col);//三个参数的意思是：要添加的控件、行号、列号
            ++groupedCount_12;
            continue;
        
        }

        else {
            // 非该分组：仍旧一行横排
            layout12->addWidget(button);
        }

        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/display_pattern_pull_down_menu/origin.png"), QStringLiteral("原始"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/display_pattern_pull_down_menu/color.png"), QStringLiteral("颜色"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/display_pattern_pull_down_menu/color_and_transparency.png"), QStringLiteral("颜色和不透明度"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 2) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/annotation_pull_down_menu/sort.png"), QStringLiteral("排列"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/annotation_pull_down_menu/min.png"), QStringLiteral("最小化"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/annotation_pull_down_menu/max.png"), QStringLiteral("最大化"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/annotation_pull_down_menu/set.png"), QStringLiteral("配置"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 3) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("渲染"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("光源"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("摄像机"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("裁剪"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("自动化"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("场景树"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("动画"));
            menu->addAction(QIcon(":/window_icons02/icons_other/window_icons/tool_pull_down_menu/same.PNG"), QStringLiteral("书签"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 4) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QWidget{color:#e0e0e0;}"
                "QCheckBox{padding:6px 24px;}"));

            auto addCheck = [&](const QString& text, bool checked,std::function<void(bool)> onToggled)
            {//onToggled：用户勾/取消时要做的事（回调）
                    auto* cb = new QCheckBox(text, menu);//checkbox控件
                    cb->setChecked(checked);
                    cb->setAttribute(Qt::WA_Hover, true);// 开启悬停事件
					auto* wa = new QWidgetAction(menu);//菜单默认不能放控件  菜单能放QAction  所以用QWidgetAction包装一下
                    //能把QCheckbox显示到菜单栏里
                    wa->setDefaultWidget(cb);
                    menu->addAction(wa);
                    QObject::connect(cb, &QCheckBox::toggled, menu, [=](bool on) {
                        if (onToggled)
                        {
                            onToggled(on);
                        }
                    });
            };

           
            addCheck(QStringLiteral("显示/隐藏右侧工具停靠栏"), true, [=](bool on) {
                // rightDock->setVisible(on);
                // emit requestToggleDock(Qt::RightDockWidgetArea, on); // 如果你走信号
                });
            addCheck(QStringLiteral("显示/隐藏左侧工具停靠栏"), true, [=](bool on) {
                // leftDock->setVisible(on);
                });
            addCheck(QStringLiteral("显示/隐藏下方工具停靠栏"), false, [=](bool on) {
                // bottomDock->setVisible(on);
                });
            addCheck(QStringLiteral("显示/隐藏上方工具停靠栏"), false, [=](bool on) {
                // topDock->setVisible(on);
                });

            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }

    }
    layout12->addStretch();
    return ribbon12;
}
