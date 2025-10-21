#include "SelectPage.h"
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
        { QStringLiteral("导航光标"),  ":/select_icons/icons_other/select_icons/navigation_cursor.png" },
        { QStringLiteral("裁剪"),  ":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_plane.png" },
        { QStringLiteral("选择对象(3D)"), ":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/rectangle2.png" },
        { QStringLiteral("绘制"),  ":/select_icons/icons_other/select_icons/draw.png" },
        { QStringLiteral("矩形"),  ":/select_icons/icons_other/select_icons/rectangle.png" },
        { QStringLiteral("圆角矩形"),  ":/select_icons/icons_other/select_icons/round_rectangle.png" },
        { QStringLiteral("椭圆"),  ":/select_icons/icons_other/select_icons/ellipse.png" },
        { QStringLiteral("折线"),  ":/select_icons/icons_other/select_icons/polyline.png" },
        { QStringLiteral("多边形套索"),  ":/select_icons/icons_other/select_icons/lasso.png" },
        { QStringLiteral("折线3D"),      ":/select_icons/icons_other/select_icons/polyline3D.png" },
        { QStringLiteral("矩形3D"),        ":/select_icons/icons_other/select_icons/rectangle3D.png" },
        { QStringLiteral("椭圆3D"),        ":/select_icons/icons_other/select_icons/ellipse3D.png" },
        { QStringLiteral("来自灰度值的ROI"),        ":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/region_grow.png" },
        { QStringLiteral("来自对象的ROI"),":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_obj_ROI.png" },
        { QStringLiteral("绘制&分割"),":/select_icons/icons_other/select_icons/draw_segmentation.png" },
        { QStringLiteral("应用绘制与分割模型"),  ":/select_icons/icons_other/select_icons/apply_draw_and_segmentation_model.png" },
        { QStringLiteral("深度分割"),  ":/select_icons/icons_other/select_icons/deep_segmentation.png" },
        { QStringLiteral("侵蚀/膨胀"),  ":/select_icons/icons_other/select_icons/erosion_dilation.png" },
        { QStringLiteral("平滑处理"),  ":/select_icons/icons_other/select_icons/smooth_process.png" },
        { QStringLiteral("裂纹分割"),  ":/select_icons/icons_other/select_icons/crack_segmentation.png" },
        { QStringLiteral("缩放"),  ":/select_icons/icons_other/select_icons/scale.png" },
        { QStringLiteral("修正"),  ":/select_icons/icons_other/select_icons/fix.png" },
        { QStringLiteral("添加ROI到ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/add_ROI_to_ROI.png" },
        { QStringLiteral("从ROI减去ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/form_ROI_subtract_ROI.png" },
        { QStringLiteral("拆分ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/detach_ROI.png" },
        { QStringLiteral("清理ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/clean_ROI.png" },
        { QStringLiteral("ROI与ROI相交"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/ROI_and_ROI_intersection.png" },
        { QStringLiteral("合并ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/merge_ROI.png" },
        { QStringLiteral("反转ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/reverse_ROI.png" },
        { QStringLiteral("更改ROI精度"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/change_ROI_precision.png" },
        { QStringLiteral("提取ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/extract_ROI.png" },
        { QStringLiteral("重新采样ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/sample_ROI.png" },
        { QStringLiteral("ROI渲染"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/ROI_render.png" },
        { QStringLiteral("粘贴带选项的ROI"),  ":/select_icons/icons_other/select_icons/paste_with_option_ROI.png" },
        { QStringLiteral("ROI模板"),  ":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/ROI_template.png" },
    };
    qDebug() << "[loadIconFor] text =" << text;

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
    qDebug() << "no path, use default path";
    return QIcon(":/icons/icons/move.png");
}


SelectPage::SelectPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout03 = new QVBoxLayout(this);
    layout03->setContentsMargins(0, 0, 0, 0);
    layout03->setSpacing(3);

    // 功能区调用
    layout03->addWidget(buildRibbon03(this));

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
    layout03->addWidget(placeholder, 1);
}

QWidget* SelectPage::buildRibbon03(QWidget* parent)
{
    auto* ribbon03 = new QFrame(parent);
    ribbon03->setObjectName(QStringLiteral("editRibbon"));
    ribbon03->setStyleSheet(QStringLiteral(
        "QFrame#editRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout03 = new QHBoxLayout(ribbon03);
    layout03->setContentsMargins(4, 4, 4, 4);
    layout03->setSpacing(1);

    struct RibbonAction03 { QString text; int hasMenu; };

    const QList<RibbonAction03> actions03 = {
        { QStringLiteral("导航光标"), 0 },
        { QStringLiteral("裁剪"), 1 },
        { QStringLiteral("选择对象(3D)"), 2 },

        // 下面这段要放成两行×四列（图标左、文字右）
        { QStringLiteral("绘制"), 0 },
        { QStringLiteral("矩形"), 0 },
        { QStringLiteral("圆角矩形"), 0 },
        { QStringLiteral("椭圆"), 0 },
        { QStringLiteral("折线"), 0 },
        { QStringLiteral("多边形套索"), 0 },
        { QStringLiteral("折线3D"), 0 },
        { QStringLiteral("矩形3D"), 0 },
        { QStringLiteral("椭圆3D"), 0 },

        // 其余继续一行横排
        { QStringLiteral("来自灰度值的ROI"), 3 },
        { QStringLiteral("来自对象的ROI"), 4 },
        { QStringLiteral("绘制&分割"), 0 },
        { QStringLiteral("应用绘制与分割模型"), 0 },
        { QStringLiteral("深度分割"), 0 },
        { QStringLiteral("侵蚀/膨胀"), 0 },
        { QStringLiteral("平滑处理"), 0 },
        { QStringLiteral("裂纹分割"), 0 },
        { QStringLiteral("缩放"), 0 },
        { QStringLiteral("修正"), 0 },

        { QStringLiteral("添加ROI到ROI"), 0 },
        { QStringLiteral("从ROI减去ROI"), 0 },
        { QStringLiteral("拆分ROI"), 0 },
        { QStringLiteral("清理ROI"), 0 },
        { QStringLiteral("ROI与ROI相交"), 0 },
        { QStringLiteral("合并ROI"), 0 },
        { QStringLiteral("反转ROI"), 0 },
        { QStringLiteral("更改ROI精度"), 0 },
        { QStringLiteral("提取ROI"), 0 },
        { QStringLiteral("重新采样ROI"), 0 },
        { QStringLiteral("ROI渲染"), 0 },
        { QStringLiteral("粘贴带选项的ROI"), 0 },

        { QStringLiteral("ROI模板"), 5 },
    };

    const QStringList twoRowGroup = {
        QStringLiteral("矩形"),
        QStringLiteral("圆角矩形"),
        QStringLiteral("椭圆"),
        QStringLiteral("折线"),
        QStringLiteral("多边形套索"),
        QStringLiteral("折线3D"),
        QStringLiteral("矩形3D"),
        QStringLiteral("椭圆3D"),

        /*QStringLiteral("添加ROI到ROI"), 
        QStringLiteral("从ROI减去ROI"),
        QStringLiteral("拆分ROI"), 
        QStringLiteral("清理ROI"), 
        QStringLiteral("ROI与ROI相交"),
        QStringLiteral("合并ROI"), 
        QStringLiteral("反转ROI"),
        QStringLiteral("更改ROI精度"),
        QStringLiteral("提取ROI"),
        QStringLiteral("重新采样ROI"),
        QStringLiteral("ROI渲染"), 
        QStringLiteral("粘贴带选项的ROI")*/
    };

    // 这个容器在第一次遇到分组项时插入到主横向布局，里面用 Grid 摆 2×4
    QWidget* gridHolder = nullptr;
    QGridLayout* grid = nullptr;
    int groupedCount = 0;

    auto makeButton = [&](const RibbonAction03& action) {
        auto* btn = new QToolButton(ribbon03);
        btn->setIcon(loadIconFor(action.text));

        // 默认：图标在上、文字在下（和你现在一致）
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setIconSize(QSize(40, 40));
        btn->setMinimumSize(QSize(70, 90));
        btn->setText(wrapByWidth(action.text, btn->font(), 51));

        // 如果是进两行组：图标在左、文字在右，做紧凑样式
        if (twoRowGroup.contains(action.text)) {
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setIconSize(QSize(20, 20));           // 小 icon
            btn->setMinimumSize(QSize(102, 32));       // 扁而宽
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            btn->setText(action.text);                // 旁边排文字，一般不需要换行
        }

        // 菜单
        if (action.hasMenu == 1 || action.hasMenu == 2 || action.hasMenu == 3
            || action.hasMenu == 4 || action.hasMenu == 5) {
            auto* menu = new QMenu(btn);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            btn->setMenu(menu);
            btn->setPopupMode(QToolButton::InstantPopup);
        }
        return btn;
        };

    for (const auto& action : actions03) {
        const bool inGroup = twoRowGroup.contains(action.text);

        if (inGroup) {
            if (!gridHolder) { // 首次遇到分组项：创建并插入一个小容器
                gridHolder = new QWidget(ribbon03);
                grid = new QGridLayout(gridHolder);
                grid->setContentsMargins(4, 2, 4, 2);
                grid->setHorizontalSpacing(8);
                grid->setVerticalSpacing(4);
                layout03->addWidget(gridHolder);      // 把“方阵”插入到主 ribbon 的当前位置
            }
            auto* btn = makeButton(action);
            int row = groupedCount / 4;               // 2 行 × 4 列
            int col = groupedCount % 4;
            if (row < 2) {
                grid->addWidget(btn, row, col);
            }
            else {
                // 超过 8 个就自动换到第 3 行（你只要 8 个的话，把列表减到 8 个即可）
                grid->addWidget(btn, row, col);
            }
            ++groupedCount;
        }
        else {
            // 不是两行组的，沿用原来的一行横排
            layout03->addWidget(makeButton(action));
        }
    }

    layout03->addStretch();
    return ribbon03;
}

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