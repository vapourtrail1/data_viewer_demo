#include "GeometryPage.h"
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
        { QStringLiteral("自动"),  ":/geometry_icons/icons_other/geometry_icons/auto.PNG" },
        { QStringLiteral("点"),  ":/geometry_icons/icons_other/geometry_icons/point.PNG" },
        { QStringLiteral("线"), ":/geometry_icons/icons_other/geometry_icons/line.PNG" },
        { QStringLiteral("圆"),  ":/geometry_icons/icons_other/geometry_icons/circle.PNG" },
        { QStringLiteral("角圆"),  ":/geometry_icons/icons_other/geometry_icons/corner_circle.PNG" },
        { QStringLiteral("平面"),  ":/geometry_icons/icons_other/geometry_icons/plane.PNG" },
        { QStringLiteral("球体"),  ":/geometry_icons/icons_other/geometry_icons/sqhere.PNG" },
        { QStringLiteral("圆柱"),  ":/geometry_icons/icons_other/geometry_icons/cylinder.PNG" },
        { QStringLiteral("圆锥"),  ":/geometry_icons/icons_other/geometry_icons/cone.PNG" },
        { QStringLiteral("环面"),      ":/geometry_icons/icons_other/geometry_icons/torus.PNG" },
        { QStringLiteral("自由造型线"),        ":/geometry_icons/icons_other/geometry_icons/freedom_modeling_line.PNG" },
        { QStringLiteral("自由造型表面"),        ":/geometry_icons/icons_other/geometry_icons/freedom_modeling_surface.PNG" },
        { QStringLiteral("定义几何元素"),        ":/geometry_icons/icons_other/geometry_icons/define_geometry_element.png" },
        { QStringLiteral("显示拟合点"),":/geometry_icons/icons_other/geometry_icons/display_fit_point.PNG" },
        { QStringLiteral("隐藏拟合点"),":/geometry_icons/icons_other/geometry_icons/hide_fit_point.PNG" },
        { QStringLiteral("切换拟合点可见性"),  ":/geometry_icons/icons_other/geometry_icons/switch_fit_point_visibility.PNG" },
        { QStringLiteral("更新CM对象"),  ":/geometry_icons02/icons_other/geometry_icons/update_CM_obj.PNG" },
        { QStringLiteral("删除未使用的CM对象"),  ":/geometry_icons/icons_other/geometry_icons/delete_unused_CM_obj.PNG" },
        { QStringLiteral("替换几何元素"),  ":/geometry_icons/icons_other/geometry_icons/replace_geometry_element.PNG" },
        { QStringLiteral("相交"),  ":/geometry_icons/icons_other/geometry_icons/intersect.PNG" },
        { QStringLiteral("对称元素"),  ":/geometry_icons/icons_other/geometry_icons/symmetry_element.PNG" },
        { QStringLiteral("修改尺寸"),  ":/geometry_icons/icons_other/geometry_icons/modify_size.PNG" },
        { QStringLiteral("组合"),  ":/geometry_icons/icons_other/geometry_icons/combine.PNG" },
        { QStringLiteral("转换"),  ":/geometry_icons/icons_other/geometry_icons/transformation.PNG" },
        { QStringLiteral("规则元素"),  ":/geometry_icons/icons_other/geometry_icons/regularize_element.PNG" },
        { QStringLiteral("投影"),  ":/geometry_icons/icons_other/geometry_icons/projection.PNG" },
        { QStringLiteral("提取最小/最大点"),  ":/geometry_icons/icons_other/geometry_icons/extract_max_min_value.PNG" },
        { QStringLiteral("提取中轴线"),  ":/geometry_icons/icons_other/geometry_icons/extract_mid_axis.PNG" },
        { QStringLiteral("转化并导出"),  ":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/save_fit_point_info.PNG" },
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


GeometryPage::GeometryPage(QWidget* parent)
    : QWidget(parent)
{
    // 设置页面外观
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout05 = new QVBoxLayout(this);
    layout05->setContentsMargins(0, 0, 0, 0);
    layout05->setSpacing(3);

    // 功能区调用
    layout05->addWidget(buildRibbon05(this));

    // 预留的内容区占位，用于后续填充具体的编辑工具界面
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("几何功能区内容区域"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("这里可以继续扩展体积编辑、几何调整等操作界面。"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout05->addWidget(placeholder, 1);
}

QWidget* GeometryPage::buildRibbon05(QWidget* parent)
{
    auto* ribbon05 = new QFrame(parent);//ribbon0-是--功能区的容器
    ribbon05->setObjectName(QStringLiteral("geometryRibbon"));
    ribbon05->setStyleSheet(QStringLiteral(
        "QFrame#geometryRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout05 = new QHBoxLayout(ribbon05);
    layout05->setContentsMargins(4, 4, 4, 4);
    layout05->setSpacing(1);

    struct RibbonAction05 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction05> actions05 = {
        { QStringLiteral("自动"), 0 },

        // 下面这段要放成两行×四列（图标左、文字右）
        { QStringLiteral("点"), 0 },
        { QStringLiteral("线"), 0 },
        { QStringLiteral("圆"), 0 },
        { QStringLiteral("角圆"), 0 },
        { QStringLiteral("平面"), 0 },
        { QStringLiteral("球体"), 0 },
        { QStringLiteral("圆柱"), 0 },
        { QStringLiteral("圆锥"), 0 },
        { QStringLiteral("环面"), 0 },
        { QStringLiteral("自由造型线"), 0 },
        { QStringLiteral("自由造型表面"), 0 },

        // 其余继续一行横排
        { QStringLiteral("定义几何元素"), 0 },
        { QStringLiteral("显示拟合点"), 0 },
        { QStringLiteral("隐藏拟合点"), 0 },
        { QStringLiteral("切换拟合点可见性"), 0 },
        { QStringLiteral("更新CM对象"), 0 },
        { QStringLiteral("删除未使用的CM对象"), 0 },
        { QStringLiteral("替换几何元素"), 0 },

        // 下面这段要放成两行×四列（图标左、文字右）
        { QStringLiteral("相交"), 0 },
        { QStringLiteral("对称元素"), 0 },
        { QStringLiteral("修改尺寸"), 0 },
        { QStringLiteral("组合"), 0 },
        { QStringLiteral("转换"), 0 },
        { QStringLiteral("规则元素"), 0 },
        { QStringLiteral("投影"), 0 },
        { QStringLiteral("提取最小/最大点"), 0 },
        { QStringLiteral("提取中轴线"), 0 },

        // 其余继续一行横排
        { QStringLiteral("转化并导出"), 1 },
    };

    // 需要做成两行四列的那 8 个按钮
    const QStringList twoRowGroup = {
         QStringLiteral("点"),
         QStringLiteral("线"), 
         QStringLiteral("圆"),
         QStringLiteral("角圆"),
         QStringLiteral("平面"), 
         QStringLiteral("球体"),
         QStringLiteral("圆柱"), 
         QStringLiteral("圆锥"), 
         QStringLiteral("环面"), 
         QStringLiteral("自由造型线"), 
         QStringLiteral("自由造型表面"),
        // 如果你想把“椭圆3D”也放进来，把上面行数调成 2×5 或替换掉其中一个即可
    };


    const QStringList twoRowGroup02 = {
        QStringLiteral("相交"),
        QStringLiteral("对称元素"),
        QStringLiteral("修改尺寸"),
        QStringLiteral("组合"),
        QStringLiteral("转换"),
        QStringLiteral("规则元素"),
        QStringLiteral("投影"),
        QStringLiteral("提取最小/最大点"),
        QStringLiteral("提取中轴线"),
    };


    QWidget* gridHolder_05 = nullptr;//这个指针的意思是 用来承载那个 2×4 的小方阵
    QGridLayout* grid_05 = nullptr;//这个指针是用来管理那个小方阵的布局
    int groupedCount_05 = 0;//记录已经放进小方阵的按钮数量

    QWidget* gridHolder02_05 = nullptr;
    QGridLayout* grid02_05 = nullptr;
    int groupedCount02_05 = 0;

    QWidget* gridHolder03_05 = nullptr;
    QGridLayout* grid03_05 = nullptr;
    int groupedCount03_05 = 0;

    for (const auto& action : actions05)
    {
        const bool inGroup_05 = twoRowGroup.contains(action.text);//contains函数检查某个元素是否在列表中 返回true或false
        const bool inGroup02_05 = twoRowGroup02.contains(action.text);
     

        auto* button = new QToolButton(ribbon05);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));

        if (inGroup_05)
        {
            if (!gridHolder_05) {//等价于gridholder == nullptr
                gridHolder_05 = new QWidget(ribbon05);
                grid_05 = new QGridLayout(gridHolder_05);
                grid_05->setContentsMargins(4, 2, 4, 2);
                grid_05->setHorizontalSpacing(8);//水平间距
                grid_05->setVerticalSpacing(4);//垂直间距
                layout05->addWidget(gridHolder_05); // 把小方阵插入到主 ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
			button->setMinimumSize(QSize(90, 20));       
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);//
            button->setText(action.text);                 // 旁排文字，不再换行

            int row = groupedCount_05 / 4;
            int col = groupedCount_05 % 4;
            grid_05->addWidget(button, row, col);//三个参数的意思是：要添加的控件、行号、列号
            ++groupedCount_05;
            continue;
        }
        if (inGroup02_05) {
            if (!gridHolder02_05)
            {
                gridHolder02_05 = new QWidget(ribbon05);
                grid02_05 = new QGridLayout(gridHolder02_05);//QGridLayout的作用是把控件放在一个网格里
				grid02_05->setContentsMargins(4, 2, 4, 2);
                grid02_05->setHorizontalSpacing(8);
                grid02_05->setVerticalSpacing(4);
                layout05->addWidget(gridHolder02_05);
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
            button->setMinimumSize(QSize(90, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);                 // 旁排文字，不再换行

            int row = groupedCount02_05 / 4;
            int col = groupedCount02_05 % 4;

            //布局管理  grid
            grid02_05->addWidget(button, row, col);
            ++groupedCount02_05;
            continue;
        }
        else {
            // 非该分组：仍旧一行横排
            layout05->addWidget(button);
        }
        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/ROI_from_geomety_element.png"), QStringLiteral("来自几何元素的ROI"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/surface_mash_from_geomety_element.PNG"), QStringLiteral("来自几何元素的表面网格"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/CAD_from_geomety_element.PNG"), QStringLiteral("来自几何元素的 CAD"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/geometry_element_export_to_surface_mash.PNG"), QStringLiteral("将几何元素导出为表面网格"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/geometry_element_export_to_CAD.PNG"), QStringLiteral("将几何元素导出为 CAD"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/untitle.PNG"), QStringLiteral("保存拟合点信息"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
    }
    layout05->addStretch();
    return ribbon05;
}
