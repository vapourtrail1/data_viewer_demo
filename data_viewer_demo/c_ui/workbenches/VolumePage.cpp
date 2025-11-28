#include "VolumePage.h"
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
        if (lineWidth + w  > maxWidthPx) {
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
        { QStringLiteral("拆分体积"),  ":/volume_icons/icons_other/volume_icons/split_volume.png" },
        { QStringLiteral("表面测定"),  ":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/surface_measure_and_based_on_iosvalue.png" },
        { QStringLiteral("删除表面测定"), ":/volume_icons/icons_other/volume_icons/delete_surface_measure.png" },
        { QStringLiteral("体积数据"),  ":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/volume_data.png" },
        { QStringLiteral("基于特征的缩放"),  ":/volume_icons/icons_other/volume_icons/feature_scale.png" },
        { QStringLiteral("手动缩放"),  ":/volume_icons/icons_other/volume_icons/manual_scale.png" },
        { QStringLiteral("绘制数据"),  ":/icons/icons/delete.png" },
        { QStringLiteral("选择颜色"),  ":/volume_icons/icons_other/volume_icons/choose_color.png" },
        { QStringLiteral("填充"),  ":/volume_icons/icons_other/volume_icons/fill.png" },
        { QStringLiteral("自适应高斯"),      ":/volume_icons/icons_other/volume_icons/adaptive_gaussian.png" },
        { QStringLiteral("非局部均值"),        ":/volume_icons/icons_other/volume_icons/non_local_mean.png" },
        { QStringLiteral("卷积"),        ":/volume_icons/icons_other/volume_icons/convolution.png" },
        { QStringLiteral("高斯"),        ":/volume_icons/icons_other/volume_icons/gaussian.png" },
        { QStringLiteral("框"),":/volume_icons/icons_other/volume_icons/frame.png" },
        { QStringLiteral("偏差"),":/volume_icons/icons_other/volume_icons/deviation.png" },
        { QStringLiteral("中值"),  ":/volume_icons/icons_other/volume_icons/mid_value.png" },
        { QStringLiteral("侵蚀"),  ":/volume_icons/icons_other/volume_icons/erosion.png" },
        { QStringLiteral("膨胀"),  ":/volume_icons/icons_other/volume_icons/dilation.png" },
        { QStringLiteral("应用不透明映射"),  ":/volume_icons/icons_other/volume_icons/apply_opacity_mapping.png" },
        { QStringLiteral("FIB-SEM 修正"),  ":/volume_icons/icons_other/volume_icons/FIB-SEM_fix.png" },
        { QStringLiteral("合并和重新采样"),  ":/volume_icons/icons_other/volume_icons/merge_and_resample.png" },
        { QStringLiteral("体积投影器"),  ":/volume_icons/icons_other/volume_icons/volume_projector.png" },
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


VolumePage::VolumePage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout02 = new QVBoxLayout(this);
    layout02->setContentsMargins(0, 0, 0, 0);
    layout02->setSpacing(3);

    // 功能区调用
    layout02->addWidget(buildRibbon02(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("体积功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout02->addWidget(placeholder, 1);
}

QWidget* VolumePage::buildRibbon02(QWidget* parent)
{
    // 创建功能区容器
    auto* ribbon02 = new QFrame(parent);
    ribbon02->setObjectName(QStringLiteral("volumeRibbon"));
    ribbon02->setStyleSheet(QStringLiteral(
        "QFrame#volumeRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout02 = new QHBoxLayout(ribbon02);
    layout02->setContentsMargins(4, 4, 4, 4);
    layout02->setSpacing(1);

    struct RibbonAction02
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction02> actions02 = {
        { QStringLiteral("拆分体积"), 0 },
        { QStringLiteral("表面测定"), 1 },
        { QStringLiteral("删除表面测定"), 0 },
        { QStringLiteral("体积数据"), 2 },
        { QStringLiteral("基于特征的缩放"), 0 },
        { QStringLiteral("手动缩放"), 0 },
        { QStringLiteral("绘制数据"), 0 },
        { QStringLiteral("选择颜色"), 0 },
        { QStringLiteral("填充"), 0 },
        { QStringLiteral("自适应高斯"), 0 },
        { QStringLiteral("非局部均值"), 0 },
        { QStringLiteral("卷积"), 0 },
        { QStringLiteral("高斯"), 0 },
        { QStringLiteral("框"), 0 },
        { QStringLiteral("偏差"), 0 },
        { QStringLiteral("中值"), 0 },
        { QStringLiteral("侵蚀"), 0 },
        { QStringLiteral("膨胀"), 0 },
        { QStringLiteral("应用不透明映射"), 0 },
        { QStringLiteral("FIB-SEM 修正"), 0 },
        { QStringLiteral("合并和重新采样"), 0 },
        { QStringLiteral("体积投影器"), 0 }
    };


    for (const auto& action : actions02) {
        // 每个功能都使用图标,文字的形式展示
        auto* button = new QToolButton(ribbon02);
        QString wrappedText = wrapByWidth(action.text, button->font(), 51);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));


        if (action.hasMenu == 1) {
            // 转换为功能 需要后期拓展
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/surface_measure_and_based_on_iosvalue.png"), QStringLiteral("基于等值"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/advanced_classic.png"), QStringLiteral("高级(经典)"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/advanced_multi_material.png"), QStringLiteral("高级(多材料)"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/fixed_contour.png"), QStringLiteral("固定轮廓"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/edit_surface_measure.png"), QStringLiteral("编辑表面测定"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        if (action.hasMenu == 2) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/volume_data.png"), QStringLiteral("体积数据"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/create_synthetic_volume_data.png"), QStringLiteral("创建合成体积数据"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/delete_volume_data.png"), QStringLiteral("删除体积数据"));
            menu02->addAction(QIcon(":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/uninstall_volume_data.png"), QStringLiteral("卸载体积数据"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/reload_volume_data.png"), QStringLiteral("重新加载体积数据"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        layout02->addWidget(button);
    }
    layout02->addStretch();
    return ribbon02;
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