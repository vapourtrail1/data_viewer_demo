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
        { QStringLiteral("绘制分割应用"),  ":/select_icons/icons_other/select_icons/apply_draw_and_segmentation_model.png" },
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
    auto* ribbon03 = new QFrame(parent);//ribbon03是--功能区的容器
    ribbon03->setObjectName(QStringLiteral("selectRibbon"));
    ribbon03->setStyleSheet(QStringLiteral(
        "QFrame#selectRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout03 = new QHBoxLayout(ribbon03);
    layout03->setContentsMargins(4, 4, 4, 4);
    layout03->setSpacing(1);

    struct RibbonAction03 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction03> actions03 = {
        { QStringLiteral("导航光标"), 0 },
        { QStringLiteral("裁剪"), 1 },
        { QStringLiteral("选择对象(3D)"), 2 },
        // 下面这段要放成两行*四列（图标左、文字右）
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
        { QStringLiteral("绘制分割应用"), 0 },
        { QStringLiteral("深度分割"), 0 },

        //排列成两行3列
        { QStringLiteral("侵蚀/膨胀"), 0 },
        { QStringLiteral("平滑处理"), 0 },
        { QStringLiteral("裂纹分割"), 0 },
        { QStringLiteral("缩放"), 0 },
        { QStringLiteral("修正"), 0 },

        //这个也一列两个
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

    // 需要做成两行四列的那 8 个按钮
    const QStringList twoRowGroup = {
        QStringLiteral("矩形"),
        QStringLiteral("圆角矩形"),
        QStringLiteral("椭圆"),
        QStringLiteral("折线"),
        QStringLiteral("多边形套索"),
        QStringLiteral("折线3D"),
        QStringLiteral("矩形3D"),
        QStringLiteral("椭圆3D")
        // 如果你想把“椭圆3D”也放进来，把上面行数调成 2×5 或替换掉其中一个即可
    };


    const QStringList twoRowGroup02 = {
        QStringLiteral("添加ROI到ROI"),
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
        QStringLiteral("粘贴带选项的ROI"),
    };


    const QStringList twoRowGroup03 =
    {
         QStringLiteral("侵蚀/膨胀"),
         QStringLiteral("平滑处理"), 
         QStringLiteral("裂纹分割"), 
         QStringLiteral("缩放"), 
         QStringLiteral("修正"), 
    };

	QWidget* gridHolder = nullptr;//这个指针的意思是 用来承载那个 2×4 的小方阵
	QGridLayout* grid = nullptr;//这个指针是用来管理那个小方阵的布局
	int groupedCount = 0;//记录已经放进小方阵的按钮数量

    QWidget* gridHolder02 = nullptr;
    QGridLayout* grid02 = nullptr;
	int groupedCount02 = 0;

    QWidget* gridHolder03 = nullptr;
    QGridLayout* grid03 = nullptr;
    int groupedCount03 = 0;

    for (const auto& action : actions03) 
    {
		const bool inGroup = twoRowGroup.contains(action.text);//contains函数检查某个元素是否在列表中 返回true或false
		const bool inGroup02 = twoRowGroup02.contains(action.text);
        const bool inGroup03 = twoRowGroup03.contains(action.text);
    
        auto* button = new QToolButton(ribbon03);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));

        if (inGroup) 
        {
            if (!gridHolder) {//等价于gridholder == nullptr
                gridHolder = new QWidget(ribbon03);
                grid = new QGridLayout(gridHolder);
                grid->setContentsMargins(4, 2, 4, 2);
                grid->setHorizontalSpacing(8);//水平间距
                grid->setVerticalSpacing(4);//垂直间距
                layout03->addWidget(gridHolder); // 把小方阵插入到主 ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
            button->setMinimumSize(QSize(102, 32));       // 扁而宽
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);//
            button->setText(action.text);                 // 旁排文字，不再换行

            int row = groupedCount / 4;
            int col = groupedCount % 4;
            grid->addWidget(button, row, col);//三个参数的意思是：要添加的控件、行号、列号
            ++groupedCount;
            continue;
        }
        if (inGroup02) {
            if (!gridHolder02)
            {
                gridHolder02 = new QWidget(ribbon03);
				grid02 = new QGridLayout(gridHolder02);//QGridLayout的作用是把控件放在一个网格里
				grid02->setContentsMargins(4, 2, 4, 2);
                grid02->setHorizontalSpacing(8);
				grid02->setVerticalSpacing(4);
                layout03->addWidget(gridHolder02);
            }
			button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			button->setIconSize(QSize(20, 20));           // 小 icon
			button->setMinimumSize(QSize(129, 20));	  
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			button->setText(action.text);                 // 旁排文字，不再换行

			int row = groupedCount02 / 4;
			int col = groupedCount02 % 4;

            //布局管理  grid
            grid02->addWidget(button, row, col);
            ++groupedCount02;
            continue;
        }
        if (inGroup03) {
            if (!gridHolder03)
            {
                gridHolder03 = new QWidget(ribbon03);
                grid03 = new QGridLayout(gridHolder03);//QGridLayout的作用是把控件放在一个网格里
                grid03->setContentsMargins(4, 2, 4, 2);
                grid03->setHorizontalSpacing(8);
                grid03->setVerticalSpacing(4);
                layout03->addWidget(gridHolder03);
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // 小 icon
            button->setMinimumSize(QSize(95, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);                 // 旁排文字，不再换行

            int row = groupedCount03 / 3;
            int col = groupedCount03 % 3;

            //布局管理  grid
            grid03->addWidget(button, row, col);
            ++groupedCount03;
            continue;
        }
        else {
            // 非该分组：仍旧一行横排
            layout03->addWidget(button);
        }

        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_plane.png"), QStringLiteral("裁剪平面"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_frame.png"), QStringLiteral("裁剪框"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_polyline3D.png"), QStringLiteral("裁剪折线3D"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_sphere.png"), QStringLiteral("裁剪球体"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_cylinder.png"), QStringLiteral("裁剪圆柱"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/aligned_clip_frame.png"), QStringLiteral("对齐的裁剪框"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 2) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/rectangle2.png"), QStringLiteral("矩形"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/ellipse2.png"), QStringLiteral("椭圆"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/lasso2.png"), QStringLiteral("套索"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 3) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/region_grow.png"), QStringLiteral("区域生长"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/material_region_grow.png"), QStringLiteral("材料区域生长"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/grayscale_value_range.png"), QStringLiteral("灰度值范围"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_rectangle.png"), QStringLiteral("自适应矩形"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_polygon.png"), QStringLiteral("自适应多边形"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_line.png"), QStringLiteral("自适应线"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 4) 
{
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_volume_CAD_grid_ROI.png"), QStringLiteral("来自体积/CAD/网格的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_clip_obj_ROI.png"), QStringLiteral("来自裁剪对象的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_defect_mask_ROI.png"), QStringLiteral("来自缺陷掩模的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_designed_part_compare_mask_ROI.png"), QStringLiteral("来自设计件/实物比较掩模的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_wall_thick_mask_ROI.png"), QStringLiteral("来自壁厚掩模的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_CAD_choose_ROI.png"), QStringLiteral("来自CAD选择的ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_geometry_element_ROI.png"), QStringLiteral("来自几何元素的ROI"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 5)
        {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/input_ROI_template.png"), QStringLiteral("导入ROI模型"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/output_ROI_template.png"), QStringLiteral("导出ROI模型"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
    }
    layout03->addStretch();
    return ribbon03;
}
