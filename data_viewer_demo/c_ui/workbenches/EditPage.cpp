#include "EditPage.h"
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


//git 
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
        if (lineWidth + (w*1.2) > maxWidthPx) {
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
        { QStringLiteral("撤销"),  ":/icons/icons/undo.png" },
        { QStringLiteral("重做"),  ":/icons/icons/redo.png" },
        { QStringLiteral("释放内存/清除撤销队列"), ":/icons/icons/free_memory.png" },
        { QStringLiteral("剪切"),  ":/icons/icons/cut.png" },
        { QStringLiteral("复制"),  ":/icons/icons/copy.png" },
        { QStringLiteral("粘贴"),  ":/icons/icons/paste.png" },
        { QStringLiteral("删除"),  ":/icons/icons/delete.png" },
        { QStringLiteral("创建对象组"),  ":/icons/icons/create_obj_group.png" },
        { QStringLiteral("取消对象组"),  ":/icons/icons/cancel_obj_group.png" },
        { QStringLiteral("转换为"),      ":/icons/icons/trans_pull_down_menu/trans.png" },
        { QStringLiteral("属性"),        ":/icons/icons/property.png" },
        { QStringLiteral("旋转"),        ":/icons/icons/spin.png" },
        { QStringLiteral("移动"),        ":/icons/icons/move.png" },
        { QStringLiteral("复制可视状态"),":/icons/icons/copy_visible_status.png" },
        { QStringLiteral("粘贴可视状态"),":/icons/icons/paste_visible_status.png" },
        { QStringLiteral("复制元信息"),  ":/icons/icons/copy_meta.png" },
        { QStringLiteral("粘贴元信息"),  ":/icons/icons/paste_meta.png" },
        { QStringLiteral("动态重命名"),  ":/icons/icons/dynamic_rename.png" },
    };

    for (const auto& m : map) {
        if (text == m.key) {
            const QString path = QString::fromUtf8(m.file);
            qDebug() << "use path =" << path << ", is exist? =" << QFile(path).exists();
            QIcon ico(path);//用给定的路径 创建一个Qicon对象
			if (!ico.isNull()){
				return ico;//
            }
        }
    }
    return QIcon(":/icons/icons/move.png");
}


EditPage::EditPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("pageEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    // 功能区调用
    layout->addWidget(buildRibbon(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("编辑功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout->addWidget(placeholder, 1);

    
}

QWidget* EditPage::buildRibbon(QWidget* parent)
{
    // 创建功能区容器
    auto* ribbon = new QFrame(parent);
    ribbon->setObjectName(QStringLiteral("editRibbon"));
    ribbon->setStyleSheet(QStringLiteral(
        "QFrame#editRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout = new QHBoxLayout(ribbon);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(1);

    /*const QIcon placeholderIcon = buildIcon(); */// 预生成占位图标，供所有按钮复用

    
    struct RibbonAction
    {
        QString text;
        bool hasMenu;
    };

    const QList<RibbonAction> actions = {
        { QStringLiteral("撤销"), false },
        { QStringLiteral("重做"), false },
        { QStringLiteral("释放内存/清除撤销队列"), false },
        { QStringLiteral("剪切"), false },
        { QStringLiteral("复制"), false },
        { QStringLiteral("粘贴"), false },
        { QStringLiteral("删除"), false },
        { QStringLiteral("创建对象组"), false },
        { QStringLiteral("取消对象组"), false },
        { QStringLiteral("转换为"), true },
        { QStringLiteral("属性"), false },
        { QStringLiteral("旋转"), false },
        { QStringLiteral("移动"), false },
        { QStringLiteral("复制可视状态"), false },
        { QStringLiteral("粘贴可视状态"), false },
        { QStringLiteral("复制元信息"), false },
        { QStringLiteral("粘贴元信息"), false },
        { QStringLiteral("动态重命名"), false }
    };


	for (const auto& action : actions) {
        // 每个功能都使用图标,文字的形式展示
        auto* button = new QToolButton(ribbon); 
        QString wrappedText = wrapByWidth(action.text, button->font(), 70);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));

       
        if (action.hasMenu) {
            // 转换为功能 需要后期拓展
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
				"QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/volume.png"), QStringLiteral("体积"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/volume_grid.png"), QStringLiteral("四面体体积网格"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/surface_grid.png"), QStringLiteral("表面网格"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/CAD.png"), QStringLiteral("CAD"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/golden_surface.png"), QStringLiteral("黄金表面"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/analysis_surface.png"), QStringLiteral("分析结果中的有色表面网格"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/integration_grid.png"), QStringLiteral("来自四面体体积网格的集成网格"));
            button->setMenu(menu);
			button->setPopupMode(QToolButton::InstantPopup);//点击按钮时直接弹出菜单
        }
        layout->addWidget(button);
    }

    layout->addStretch();
    return ribbon;
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