#include "ReportPage.h"
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

// 辅助函数  根据按钮文本加载对应图标 git
static QIcon loadIconFor(const QString& text) {
    struct Map {
        QString key; //避免编码转换 直接用QString
        const char* file;
    };
    static const Map map[] = {
        { QStringLiteral("创建报告"),  ":/report_icons/icons_other/report_icons/create_report.PNG" },
        { QStringLiteral("打开报告"),  ":/report_icons/icons_other/report_icons/open_report.PNG" },
        { QStringLiteral("更新报告"), ":/report_icons/icons_other/report_icons/updata_report.PNG" },
        { QStringLiteral("删除报告"),  ":/report_icons/icons_other/report_icons/delelte_report.PNG" },
        { QStringLiteral("导出PDF"),  ":/report_icons/icons_other/report_icons/output_PDF.PNG" },
        { QStringLiteral("导出HTML"),  ":/report_icons/icons_other/report_icons/output_html.png" },
        { QStringLiteral("导出默认布局"),  ":/report_icons/icons_other/report_icons/output_default_layout.png" },
        { QStringLiteral("导出特定布局"),  ":/report_icons/icons_other/report_icons/output_special_layout.png" },
        { QStringLiteral("创建书签"),  ":/report_icons/icons_other/report_icons/create_bookmark.PNG" },
        { QStringLiteral("书签编辑器"),      ":/report_icons/icons_other/report_icons/bookmark_editor.PNG" },
        { QStringLiteral("保存图像"),        ":/report_icons/icons_other/report_icons/save_image.png" },
        { QStringLiteral("保存影片/图像堆栈"),        ":/report_icons/icons_other/report_icons/save_movie_image_stack.png" },
        { QStringLiteral("导出为Q-DAS文件"),        ":/report_icons/icons_other/report_icons/output_Q_DAS_document.png" },
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

ReportPage::ReportPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("reportEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageReport{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout10 = new QVBoxLayout(this);
    layout10->setContentsMargins(0, 0, 0, 0);
    layout10->setSpacing(3);

    // 功能区调用
    layout10->addWidget(buildRibbon10(this));

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
    layout10->addWidget(placeholder, 1);
}

QWidget* ReportPage::buildRibbon10(QWidget* parent)
{
    auto* ribbon10 = new QFrame(parent);//ribbon03是--功能区的容器
    ribbon10->setObjectName(QStringLiteral("reportRibbon"));
    ribbon10->setStyleSheet(QStringLiteral(
        "QFrame#reportRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout10 = new QHBoxLayout(ribbon10);
    layout10->setContentsMargins(4, 4, 4, 4);
    layout10->setSpacing(1);

    struct RibbonAction10 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction10> actions10 = {
        { QStringLiteral("创建报告"), 0 },
        { QStringLiteral("打开报告"), 0 },
        { QStringLiteral("更新报告"), 0 },
        { QStringLiteral("删除报告"), 0 },
        { QStringLiteral("导出PDF"), 0 },
        { QStringLiteral("导出HTML"), 0 },
        { QStringLiteral("导出默认布局"), 0 },
        { QStringLiteral("导出特定布局"), 0 },
        { QStringLiteral("创建书签"), 0 },
        { QStringLiteral("书签编辑器"), 0 },
        { QStringLiteral("保存图像"), 0 },
        { QStringLiteral("保存影片/图像堆栈"), 0 },
        { QStringLiteral("导出为Q-DAS文件"), 0 },
    };

    // 需要做成两行四列的那 8 个按钮
    const QStringList twoRowGroup = {
        QStringLiteral("导出PDF"),
        QStringLiteral("导出HTML"),
        QStringLiteral("导出默认布局"),
        QStringLiteral("导出特定布局"),
        // 如果你想把“椭圆3D”也放进来，把上面行数调成 2×5 或替换掉其中一个即可
    };



    QWidget* gridHolder_10 = nullptr;//这个指针的意思是 用来承载那个 2×4 的小方阵
    QGridLayout* grid_10 = nullptr;//这个指针是用来管理那个小方阵的布局
    int groupedCount_10 = 0;//记录已经放进小方阵的按钮数量


    for (const auto& action : actions10)
    {
        const bool inGroup_10 = twoRowGroup.contains(action.text);//contains函数检查某个元素是否在列表中 返回true或false
        auto* button = new QToolButton(ribbon10);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));
        if (inGroup_10)
        {
            if (!gridHolder_10) {//等价于gridholder == nullptr
                gridHolder_10 = new QWidget(ribbon10);
                grid_10 = new QGridLayout(gridHolder_10);
                grid_10->setContentsMargins(4, 2, 4, 2);
                grid_10->setHorizontalSpacing(8);//水平间距
                grid_10->setVerticalSpacing(4);//垂直间距
                layout10->addWidget(gridHolder_10); // 把小方阵插入到主 ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
            button->setMinimumSize(QSize(90, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);

            int row = groupedCount_10 / 2;
            int col = groupedCount_10 % 2;
            grid_10->addWidget(button, row, col);//三个参数的意思是：要添加的控件、行号、列号
            ++groupedCount_10;
            continue;
        }

        else {
            // 非该分组：仍旧一行横排
            layout10->addWidget(button);
        }
    }
    layout10->addStretch();
    return ribbon10;
}
