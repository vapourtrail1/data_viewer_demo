#include "CADAndThen.h"
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
        { QStringLiteral("简化表面网格"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/simplify_surface_mesh.PNG" },
        { QStringLiteral("删除孤立的分量"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/delete_lonely_component.PNG" },
        { QStringLiteral("翻转表面方向"), ":/cad_icons/icons_other/CAD_surfacemesh_icons/reverse_surface_direction.PNG" },
        { QStringLiteral("重新计算CAD网格"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/re-cal_CAD_mesh.PNG" },
        { QStringLiteral("合并表面网格对象"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/merge_surface_mesh_obj.PNG" },
        { QStringLiteral("变形网格"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/deformation_mesh.PNG" },
        { QStringLiteral("模具修正"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/mold_modify.PNG" },
        { QStringLiteral("补偿网格"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/compensation_mesh.PNG" },
        { QStringLiteral("迭代补偿网格"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/lteration_compensation_mesh.PNG" },
        { QStringLiteral("变形场"),      ":/cad_icons/icons_other/CAD_surfacemesh_icons/deformation_field.PNG" },
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


CADAndThen::CADAndThen(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("CADEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#CADEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout07 = new QVBoxLayout(this);
    layout07->setContentsMargins(0, 0, 0, 0);
    layout07->setSpacing(3);

    // 功能区调用
    layout07->addWidget(buildRibbon07(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("CAD/表面网格功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout07->addWidget(placeholder, 1);
}

QWidget* CADAndThen::buildRibbon07(QWidget* parent)
{
    // 创建功能区容器
    auto* ribbon07 = new QFrame(parent);
    ribbon07->setObjectName(QStringLiteral("CADRibbon"));
    ribbon07->setStyleSheet(QStringLiteral(
        "QFrame#CADRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout07 = new QHBoxLayout(ribbon07);
    layout07->setContentsMargins(4, 4, 4, 4);
    layout07->setSpacing(1);

    /*const QIcon placeholderIcon = buildIcon(); */// 预生成占位图标，供所有按钮复用


    struct RibbonAction07
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction07> actions07 = {
        { QStringLiteral("简化表面网格"), 0 },
        { QStringLiteral("删除孤立的分量"), 0 },
        { QStringLiteral("翻转表面方向"), 0 },
        { QStringLiteral("重新计算CAD网格"), 0 },
        { QStringLiteral("合并表面网格对象"), 0 },
        { QStringLiteral("变形网格"), 0 },
        { QStringLiteral("模具修正"), 0 },
        { QStringLiteral("补偿网格"), 0 },
        { QStringLiteral("迭代补偿网格"), 0 },
        { QStringLiteral("变形场"), 0 }
    };


    for (const auto& action : actions07) {
        // 每个功能都使用图标,文字的形式展示
        auto* button = new QToolButton(ribbon07);
        QString wrappedText = wrapByWidth(action.text, button->font(), 51);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));
        layout07->addWidget(button);
    }
    layout07->addStretch();
    return ribbon07;
}

