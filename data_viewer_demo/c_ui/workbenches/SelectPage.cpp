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
        { QStringLiteral("Ӧ�û�����ָ�ģ��"),  ":/select_icons/icons_other/select_icons/apply_draw_and_segmentation_model.png" },
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
        { QStringLiteral("Ӧ�û�����ָ�ģ��"), 0 },
        { QStringLiteral("��ȷָ�"), 0 },
        { QStringLiteral("��ʴ/����"), 0 },
        { QStringLiteral("ƽ������"), 0 },
        { QStringLiteral("���Ʒָ�"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("����"), 0 },

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

    const QStringList twoRowGroup = {
        QStringLiteral("����"),
        QStringLiteral("Բ�Ǿ���"),
        QStringLiteral("��Բ"),
        QStringLiteral("����"),
        QStringLiteral("���������"),
        QStringLiteral("����3D"),
        QStringLiteral("����3D"),
        QStringLiteral("��Բ3D"),

        /*QStringLiteral("���ROI��ROI"), 
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
        QStringLiteral("ճ����ѡ���ROI")*/
    };

    // ��������ڵ�һ������������ʱ���뵽�����򲼾֣������� Grid �� 2��4
    QWidget* gridHolder = nullptr;
    QGridLayout* grid = nullptr;
    int groupedCount = 0;

    auto makeButton = [&](const RibbonAction03& action) {
        auto* btn = new QToolButton(ribbon03);
        btn->setIcon(loadIconFor(action.text));

        // Ĭ�ϣ�ͼ�����ϡ��������£���������һ�£�
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setIconSize(QSize(40, 40));
        btn->setMinimumSize(QSize(70, 90));
        btn->setText(wrapByWidth(action.text, btn->font(), 51));

        // ����ǽ������飺ͼ�������������ң���������ʽ
        if (twoRowGroup.contains(action.text)) {
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            btn->setIconSize(QSize(20, 20));           // С icon
            btn->setMinimumSize(QSize(102, 32));       // �����
            btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            btn->setText(action.text);                // �Ա������֣�һ�㲻��Ҫ����
        }

        // �˵�
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
            if (!gridHolder) { // �״��������������������һ��С����
                gridHolder = new QWidget(ribbon03);
                grid = new QGridLayout(gridHolder);
                grid->setContentsMargins(4, 2, 4, 2);
                grid->setHorizontalSpacing(8);
                grid->setVerticalSpacing(4);
                layout03->addWidget(gridHolder);      // �ѡ����󡱲��뵽�� ribbon �ĵ�ǰλ��
            }
            auto* btn = makeButton(action);
            int row = groupedCount / 4;               // 2 �� �� 4 ��
            int col = groupedCount % 4;
            if (row < 2) {
                grid->addWidget(btn, row, col);
            }
            else {
                // ���� 8 �����Զ������� 3 �У���ֻҪ 8 ���Ļ������б���� 8 �����ɣ�
                grid->addWidget(btn, row, col);
            }
            ++groupedCount;
        }
        else {
            // ����������ģ�����ԭ����һ�к���
            layout03->addWidget(makeButton(action));
        }
    }

    layout03->addStretch();
    return ribbon03;
}

//QIcon EditPage::buildIcon() const
//{
//    // ����һ����ɫ�ķ���ռλͼ�꣬�����滻Ϊ��ʵ��Դ
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