#include "AlignmentPage.h"
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


//gitt
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
        { QStringLiteral("最佳拟合对齐"),  ":/alignment_icons/icons_other/align_icons/best_fit_align.png" },
        { QStringLiteral("3-2-1对齐"),  ":/alignment_icons/icons_other/align_icons/3-2-1_align.png" },
        { QStringLiteral("基于特征的对齐"), ":/alignment_icons/icons_other/align_icons/based_on_feature_align.png" },
        { QStringLiteral("按次序对齐"),  ":/alignment_icons/icons_other/align_icons/in_order_align.png" },
        { QStringLiteral("RPS对齐"),  ":/alignment_icons/icons_other/align_icons/RPS_align.png" },
        { QStringLiteral("基于几何元素的拟合"),  ":/alignment_icons/icons_other/align_icons/based_on_geometry_element_best_fit.png" },
        { QStringLiteral("编辑当前对齐"),  ":/alignment_icons/icons_other/align_icons/edit_current_align.png" },
        { QStringLiteral("简单3-2-1对齐"),  ":/alignment_icons/icons_other/align_icons/simple_3-2-1_align.png" },
        { QStringLiteral("简单对齐"),  ":/alignment_icons/icons_other/align_icons/simple_align.png" },
        { QStringLiteral("将切片图对齐到对象"),      ":/alignment_icons/icons_other/align_icons/slice_images_align_to_obj.png" },
        { QStringLiteral("坐标系原点"),        ":/alignment_icons/icons_other/align_icons/coordinate_origin.png" },
        { QStringLiteral("坐标系编辑器"),        ":/alignment_icons/icons_other/align_icons/coordinate_system_editor.png" },
        { QStringLiteral("存储对齐"),        ":/alignment_icons/icons_other/align_icons/store_align.png" },
        { QStringLiteral("应用对齐"),":/alignment_icons/icons_other/align_icons/apply_align.png" },
        { QStringLiteral("复制转换"),":/alignment_icons/icons_other/align_icons/copy_transformation.png" },
        { QStringLiteral("粘贴转换"),  ":/alignment_icons/icons_other/align_icons/paste_transformation.png" },
        { QStringLiteral("重置转换"),  ":/alignment_icons/icons_other/align_icons/reset_transformation.png" },
        { QStringLiteral("锁定"),  ":/alignment_icons/icons_other/align_icons/lock.png" },
        { QStringLiteral("解锁"),  ":/alignment_icons/icons_other/align_icons/unlock.png" },
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


AlignmentPage::AlignmentPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("alignmentEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#alignmentEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout04 = new QVBoxLayout(this);
    layout04->setContentsMargins(0, 0, 0, 0);
    layout04->setSpacing(3);

    // 功能区调用
    layout04->addWidget(buildRibbon04(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("对齐功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout04->addWidget(placeholder, 1);
}

QWidget* AlignmentPage::buildRibbon04(QWidget* parent)
{
    auto* ribbon04 = new QFrame(parent);//ribbon04是"  "功能区的容器
    ribbon04->setObjectName(QStringLiteral("alignmentRibbon"));
    ribbon04->setStyleSheet(QStringLiteral(
        "QFrame#alignmentRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout04 = new QHBoxLayout(ribbon04);
    layout04->setContentsMargins(4, 4, 4, 4);
    layout04->setSpacing(1);

    struct RibbonAction04 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction04> actions04 = {
        { QStringLiteral("最佳拟合对齐"), 0 },
        { QStringLiteral("3-2-1对齐"), 0 },
        { QStringLiteral("基于特征的对齐"),0 },
        { QStringLiteral("按次序对齐"), 0 },
        { QStringLiteral("RPS对齐"), 0 },
        { QStringLiteral("基于几何元素的拟合"), 0 },
        { QStringLiteral("编辑当前对齐"), 0 },
        { QStringLiteral("简单3-2-1对齐"), 0 },
        { QStringLiteral("简单对齐"), 0 },
        { QStringLiteral("将切片图对齐到对象"), 0 },
        { QStringLiteral("坐标系原点"), 0 },
        { QStringLiteral("坐标系编辑器"), 0 },
        { QStringLiteral("存储对齐"), 0 },
        { QStringLiteral("应用对齐"), 0 },
        { QStringLiteral("复制转换"), 0 },
        { QStringLiteral("粘贴转换"), 0 },
        { QStringLiteral("重置转换"), 0 },
        { QStringLiteral("锁定"), 0 },
        { QStringLiteral("解锁"), 0 },
    };
    for (const auto& action : actions04)
    {
        auto* button = new QToolButton(ribbon04);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 60));
        layout04->addWidget(button);
    }
    layout04->addStretch();
    return ribbon04;
}
