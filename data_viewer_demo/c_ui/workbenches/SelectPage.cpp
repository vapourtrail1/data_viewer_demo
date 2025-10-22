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



//static���������޶�������ʾ�ú������ڵ�ǰ�ļ��ڿɼ�����ֹ������ͻ
//�����������ƻ���
static QString wrapByWidth(const QString& s, const QFont& font, int maxWidthPx) {//����������Ϊһ�������������ؿ��
    QFontMetrics fm(font); //�������������ÿ���ַ������ַ��������ؿ�ȡ�
    QString out;
    int lineWidth = 0;//��ǰ�е���ռ�õ����ؿ���ۼ�

    auto flushLineBreak = [&]() { out += QChar('\n');
    lineWidth = 0; };

    for (int i = 0; i < s.size(); ++i) {
        const QChar ch = s.at(i);//���ָ��λ�õ��ַ�
        int w = fm.horizontalAdvance(ch);//���ַ��ڵ�ǰ�����µ����ؿ��

        // ��������Ȼ�ϵ㴦����
        bool isBreakable = (ch.isSpace() || ch == '/' || ch == '��' || ch == '��');
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

// ��������  ���ݰ�ť�ı����ض�Ӧͼ��
static QIcon loadIconFor(const QString& text) {
    struct Map {
        QString key; //�������ת�� ֱ����QString
        const char* file;
    };
    static const Map map[] = {
        { QStringLiteral("�������"),  ":/select_icons/icons_other/select_icons/navigation_cursor.png" },
        { QStringLiteral("�ü�"),  ":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_plane.png" },
        { QStringLiteral("ѡ�����(3D)"), ":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/rectangle2.png" },
        { QStringLiteral("����"),  ":/select_icons/icons_other/select_icons/draw.png" },
        { QStringLiteral("����"),  ":/select_icons/icons_other/select_icons/rectangle.png" },
        { QStringLiteral("Բ�Ǿ���"),  ":/select_icons/icons_other/select_icons/round_rectangle.png" },
        { QStringLiteral("��Բ"),  ":/select_icons/icons_other/select_icons/ellipse.png" },
        { QStringLiteral("����"),  ":/select_icons/icons_other/select_icons/polyline.png" },
        { QStringLiteral("���������"),  ":/select_icons/icons_other/select_icons/lasso.png" },
        { QStringLiteral("����3D"),      ":/select_icons/icons_other/select_icons/polyline3D.png" },
        { QStringLiteral("����3D"),        ":/select_icons/icons_other/select_icons/rectangle3D.png" },
        { QStringLiteral("��Բ3D"),        ":/select_icons/icons_other/select_icons/ellipse3D.png" },
        { QStringLiteral("���ԻҶ�ֵ��ROI"),        ":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/region_grow.png" },
        { QStringLiteral("���Զ����ROI"),":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_obj_ROI.png" },
        { QStringLiteral("����&�ָ�"),":/select_icons/icons_other/select_icons/draw_segmentation.png" },
        { QStringLiteral("���Ʒָ�Ӧ��"),  ":/select_icons/icons_other/select_icons/apply_draw_and_segmentation_model.png" },
        { QStringLiteral("��ȷָ�"),  ":/select_icons/icons_other/select_icons/deep_segmentation.png" },
        { QStringLiteral("��ʴ/����"),  ":/select_icons/icons_other/select_icons/erosion_dilation.png" },
        { QStringLiteral("ƽ������"),  ":/select_icons/icons_other/select_icons/smooth_process.png" },
        { QStringLiteral("���Ʒָ�"),  ":/select_icons/icons_other/select_icons/crack_segmentation.png" },
        { QStringLiteral("����"),  ":/select_icons/icons_other/select_icons/scale.png" },
        { QStringLiteral("����"),  ":/select_icons/icons_other/select_icons/fix.png" },
        { QStringLiteral("���ROI��ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/add_ROI_to_ROI.png" },
        { QStringLiteral("��ROI��ȥROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/form_ROI_subtract_ROI.png" },
        { QStringLiteral("���ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/detach_ROI.png" },
        { QStringLiteral("����ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/clean_ROI.png" },
        { QStringLiteral("ROI��ROI�ཻ"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/ROI_and_ROI_intersection.png" },
        { QStringLiteral("�ϲ�ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/merge_ROI.png" },
        { QStringLiteral("��תROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/reverse_ROI.png" },
        { QStringLiteral("����ROI����"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/change_ROI_precision.png" },
        { QStringLiteral("��ȡROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/extract_ROI.png" },
        { QStringLiteral("���²���ROI"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/sample_ROI.png" },
        { QStringLiteral("ROI��Ⱦ"),  ":/select_icons_2/icons_other/select_icons/action_icon_group/ROI_render.png" },
        { QStringLiteral("ճ����ѡ���ROI"),  ":/select_icons/icons_other/select_icons/paste_with_option_ROI.png" },
        { QStringLiteral("ROIģ��"),  ":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/ROI_template.png" },
    };
    qDebug() << "[loadIconFor] text =" << text;

    for (const auto& m : map) {
        if (text == m.key) {
            const QString path = QString::fromUtf8(m.file);
            qDebug() << "use path =" << path << ", is exist? =" << QFile(path).exists();
            QIcon ico(path);//�ø�����·�� ����һ��Qicon����
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
    // ����ҳ�����
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout03 = new QVBoxLayout(this);
    layout03->setContentsMargins(0, 0, 0, 0);
    layout03->setSpacing(3);

    // ����������
    layout03->addWidget(buildRibbon03(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("ѡ��������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout03->addWidget(placeholder, 1);
}

QWidget* SelectPage::buildRibbon03(QWidget* parent)
{
    auto* ribbon03 = new QFrame(parent);//ribbon03��--������������
    ribbon03->setObjectName(QStringLiteral("editRibbon"));
    ribbon03->setStyleSheet(QStringLiteral(
        "QFrame#editRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout03 = new QHBoxLayout(ribbon03);
    layout03->setContentsMargins(4, 4, 4, 4);
    layout03->setSpacing(1);

    struct RibbonAction03 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction03> actions03 = {
        { QStringLiteral("�������"), 0 },
        { QStringLiteral("�ü�"), 1 },
        { QStringLiteral("ѡ�����(3D)"), 2 },

        // �������Ҫ�ų����С����У�ͼ���������ң�
        { QStringLiteral("����"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("Բ�Ǿ���"), 0 },
        { QStringLiteral("��Բ"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("���������"), 0 },
        { QStringLiteral("����3D"), 0 },
        { QStringLiteral("����3D"), 0 },
        { QStringLiteral("��Բ3D"), 0 },

        // �������һ�к���
        { QStringLiteral("���ԻҶ�ֵ��ROI"), 3 },
        { QStringLiteral("���Զ����ROI"), 4 },
        { QStringLiteral("����&�ָ�"), 0 },
        { QStringLiteral("���Ʒָ�Ӧ��"), 0 },
        { QStringLiteral("��ȷָ�"), 0 },

        //���г�����3��
        { QStringLiteral("��ʴ/����"), 0 },
        { QStringLiteral("ƽ������"), 0 },
        { QStringLiteral("���Ʒָ�"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("����"), 0 },

        //���Ҳһ������
        { QStringLiteral("���ROI��ROI"), 0 },
        { QStringLiteral("��ROI��ȥROI"), 0 },
        { QStringLiteral("���ROI"), 0 },
        { QStringLiteral("����ROI"), 0 },
        { QStringLiteral("ROI��ROI�ཻ"), 0 },
        { QStringLiteral("�ϲ�ROI"), 0 },
        { QStringLiteral("��תROI"), 0 },
        { QStringLiteral("����ROI����"), 0 },
        { QStringLiteral("��ȡROI"), 0 },
        { QStringLiteral("���²���ROI"), 0 },
        { QStringLiteral("ROI��Ⱦ"), 0 },
        { QStringLiteral("ճ����ѡ���ROI"), 0 },

        { QStringLiteral("ROIģ��"), 5 },
    };

    // ��Ҫ�����������е��� 8 ����ť
    const QStringList twoRowGroup = {
        QStringLiteral("����"),
        QStringLiteral("Բ�Ǿ���"),
        QStringLiteral("��Բ"),
        QStringLiteral("����"),
        QStringLiteral("���������"),
        QStringLiteral("����3D"),
        QStringLiteral("����3D"),
        QStringLiteral("��Բ3D")
        // �������ѡ���Բ3D��Ҳ�Ž������������������� 2��5 ���滻������һ������
    };


    const QStringList twoRowGroup02 = {
        QStringLiteral("���ROI��ROI"),
        QStringLiteral("��ROI��ȥROI"),
        QStringLiteral("���ROI"),
        QStringLiteral("����ROI"),
        QStringLiteral("ROI��ROI�ཻ"),
        QStringLiteral("�ϲ�ROI"),
        QStringLiteral("��תROI"),
        QStringLiteral("����ROI����"),
        QStringLiteral("��ȡROI"),
        QStringLiteral("���²���ROI"),
        QStringLiteral("ROI��Ⱦ"),
        QStringLiteral("ճ����ѡ���ROI"),
    };


    const QStringList twoRowGroup03 =
    {
         QStringLiteral("��ʴ/����"),
         QStringLiteral("ƽ������"), 
         QStringLiteral("���Ʒָ�"), 
         QStringLiteral("����"), 
         QStringLiteral("����"), 
    };

	QWidget* gridHolder = nullptr;//���ָ�����˼�� ���������Ǹ� 2��4 ��С����
	QGridLayout* grid = nullptr;//���ָ�������������Ǹ�С����Ĳ���
	int groupedCount = 0;//��¼�Ѿ��Ž�С����İ�ť����

    QWidget* gridHolder02 = nullptr;
    QGridLayout* grid02 = nullptr;
	int groupedCount02 = 0;

    QWidget* gridHolder03 = nullptr;
    QGridLayout* grid03 = nullptr;
    int groupedCount03 = 0;

    for (const auto& action : actions03) 
    {
		const bool inGroup = twoRowGroup.contains(action.text);//contains�������ĳ��Ԫ���Ƿ����б��� ����true��false
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
            if (!gridHolder) {//�ȼ���gridholder == nullptr
                gridHolder = new QWidget(ribbon03);
                grid = new QGridLayout(gridHolder);
                grid->setContentsMargins(4, 2, 4, 2);
                grid->setHorizontalSpacing(8);//ˮƽ���
                grid->setVerticalSpacing(4);//��ֱ���
                layout03->addWidget(gridHolder); // ��С������뵽�� ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // С icon
            button->setMinimumSize(QSize(102, 32));       // �����
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);//
            button->setText(action.text);                 // �������֣����ٻ���

            int row = groupedCount / 4;
            int col = groupedCount % 4;
            grid->addWidget(button, row, col);//������������˼�ǣ�Ҫ��ӵĿؼ����кš��к�
            ++groupedCount;
            continue;
        }
        if (inGroup02) {
            if (!gridHolder02)
            {
                gridHolder02 = new QWidget(ribbon03);
				grid02 = new QGridLayout(gridHolder02);//QGridLayout�������ǰѿؼ�����һ��������
				grid02->setContentsMargins(4, 2, 4, 2);
                grid02->setHorizontalSpacing(8);
				grid02->setVerticalSpacing(4);
                layout03->addWidget(gridHolder02);
            }
			button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
			button->setIconSize(QSize(20, 20));           // С icon
			button->setMinimumSize(QSize(129, 20));	  
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
			button->setText(action.text);                 // �������֣����ٻ���

			int row = groupedCount02 / 4;
			int col = groupedCount02 % 4;

            //���ֹ���  grid
            grid02->addWidget(button, row, col);
            ++groupedCount02;
            continue;
        }
        if (inGroup03) {
            if (!gridHolder03)
            {
                gridHolder03 = new QWidget(ribbon03);
                grid03 = new QGridLayout(gridHolder03);//QGridLayout�������ǰѿؼ�����һ��������
                grid03->setContentsMargins(4, 2, 4, 2);
                grid03->setHorizontalSpacing(8);
                grid03->setVerticalSpacing(4);
                layout03->addWidget(gridHolder03);
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // С icon
            button->setMinimumSize(QSize(95, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);                 // �������֣����ٻ���

            int row = groupedCount03 / 3;
            int col = groupedCount03 % 3;

            //���ֹ���  grid
            grid03->addWidget(button, row, col);
            ++groupedCount03;
            continue;
        }
        else {
            // �Ǹ÷��飺�Ծ�һ�к���
            layout03->addWidget(button);
        }

        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_plane.png"), QStringLiteral("�ü�ƽ��"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_frame.png"), QStringLiteral("�ü���"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_polyline3D.png"), QStringLiteral("�ü�����3D"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_sphere.png"), QStringLiteral("�ü�����"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/clip_cylinder.png"), QStringLiteral("�ü�Բ��"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/clip_pull_down_menu/aligned_clip_frame.png"), QStringLiteral("����Ĳü���"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 2) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/rectangle2.png"), QStringLiteral("����"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/ellipse2.png"), QStringLiteral("��Բ"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/choose_obj_3D_pull_down_menu/lasso2.png"), QStringLiteral("����"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 3) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/region_grow.png"), QStringLiteral("��������"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/material_region_grow.png"), QStringLiteral("������������"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/grayscale_value_range.png"), QStringLiteral("�Ҷ�ֵ��Χ"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_rectangle.png"), QStringLiteral("����Ӧ����"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_polygon.png"), QStringLiteral("����Ӧ�����"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_grayscale_value_ROI_pull_down_menu/adaptive_line.png"), QStringLiteral("����Ӧ��"));
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
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_volume_CAD_grid_ROI.png"), QStringLiteral("�������/CAD/�����ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_clip_obj_ROI.png"), QStringLiteral("���Բü������ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_defect_mask_ROI.png"), QStringLiteral("����ȱ����ģ��ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_designed_part_compare_mask_ROI.png"), QStringLiteral("������Ƽ�/ʵ��Ƚ���ģ��ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_wall_thick_mask_ROI.png"), QStringLiteral("���Աں���ģ��ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_CAD_choose_ROI.png"), QStringLiteral("����CADѡ���ROI"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/from_obj_ROI_pull_down_menu/from_geometry_element_ROI.png"), QStringLiteral("���Լ���Ԫ�ص�ROI"));
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
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/input_ROI_template.png"), QStringLiteral("����ROIģ��"));
            menu->addAction(QIcon(":/select_icons_2/icons_other/select_icons/ROI_template_pull_down_menu/output_ROI_template.png"), QStringLiteral("����ROIģ��"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
    }
    layout03->addStretch();
    return ribbon03;
}
