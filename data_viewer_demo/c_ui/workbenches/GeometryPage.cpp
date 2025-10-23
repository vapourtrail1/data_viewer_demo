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
        { QStringLiteral("�Զ�"),  ":/geometry_icons/icons_other/geometry_icons/auto.PNG" },
        { QStringLiteral("��"),  ":/geometry_icons/icons_other/geometry_icons/point.PNG" },
        { QStringLiteral("��"), ":/geometry_icons/icons_other/geometry_icons/line.PNG" },
        { QStringLiteral("Բ"),  ":/geometry_icons/icons_other/geometry_icons/circle.PNG" },
        { QStringLiteral("��Բ"),  ":/geometry_icons/icons_other/geometry_icons/corner_circle.PNG" },
        { QStringLiteral("ƽ��"),  ":/geometry_icons/icons_other/geometry_icons/plane.PNG" },
        { QStringLiteral("����"),  ":/geometry_icons/icons_other/geometry_icons/sqhere.PNG" },
        { QStringLiteral("Բ��"),  ":/geometry_icons/icons_other/geometry_icons/cylinder.PNG" },
        { QStringLiteral("Բ׶"),  ":/geometry_icons/icons_other/geometry_icons/cone.PNG" },
        { QStringLiteral("����"),      ":/geometry_icons/icons_other/geometry_icons/torus.PNG" },
        { QStringLiteral("����������"),        ":/geometry_icons/icons_other/geometry_icons/freedom_modeling_line.PNG" },
        { QStringLiteral("�������ͱ���"),        ":/geometry_icons/icons_other/geometry_icons/freedom_modeling_surface.PNG" },
        { QStringLiteral("���弸��Ԫ��"),        ":/geometry_icons/icons_other/geometry_icons/define_geometry_element.png" },
        { QStringLiteral("��ʾ��ϵ�"),":/geometry_icons/icons_other/geometry_icons/display_fit_point.PNG" },
        { QStringLiteral("������ϵ�"),":/geometry_icons/icons_other/geometry_icons/hide_fit_point.PNG" },
        { QStringLiteral("�л���ϵ�ɼ���"),  ":/geometry_icons/icons_other/geometry_icons/switch_fit_point_visibility.PNG" },
        { QStringLiteral("����CM����"),  ":/geometry_icons02/icons_other/geometry_icons/update_CM_obj.PNG" },
        { QStringLiteral("ɾ��δʹ�õ�CM����"),  ":/geometry_icons/icons_other/geometry_icons/delete_unused_CM_obj.PNG" },
        { QStringLiteral("�滻����Ԫ��"),  ":/geometry_icons/icons_other/geometry_icons/replace_geometry_element.PNG" },
        { QStringLiteral("�ཻ"),  ":/geometry_icons/icons_other/geometry_icons/intersect.PNG" },
        { QStringLiteral("�Գ�Ԫ��"),  ":/geometry_icons/icons_other/geometry_icons/symmetry_element.PNG" },
        { QStringLiteral("�޸ĳߴ�"),  ":/geometry_icons/icons_other/geometry_icons/modify_size.PNG" },
        { QStringLiteral("���"),  ":/geometry_icons/icons_other/geometry_icons/combine.PNG" },
        { QStringLiteral("ת��"),  ":/geometry_icons/icons_other/geometry_icons/transformation.PNG" },
        { QStringLiteral("����Ԫ��"),  ":/geometry_icons/icons_other/geometry_icons/regularize_element.PNG" },
        { QStringLiteral("ͶӰ"),  ":/geometry_icons/icons_other/geometry_icons/projection.PNG" },
        { QStringLiteral("��ȡ��С/����"),  ":/geometry_icons/icons_other/geometry_icons/extract_max_min_value.PNG" },
        { QStringLiteral("��ȡ������"),  ":/geometry_icons/icons_other/geometry_icons/extract_mid_axis.PNG" },
        { QStringLiteral("ת��������"),  ":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/save_fit_point_info.PNG" },
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


GeometryPage::GeometryPage(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout05 = new QVBoxLayout(this);
    layout05->setContentsMargins(0, 0, 0, 0);
    layout05->setSpacing(3);

    // ����������
    layout05->addWidget(buildRibbon05(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("���ι�������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout05->addWidget(placeholder, 1);
}

QWidget* GeometryPage::buildRibbon05(QWidget* parent)
{
    auto* ribbon05 = new QFrame(parent);//ribbon0-��--������������
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
        { QStringLiteral("�Զ�"), 0 },

        // �������Ҫ�ų����С����У�ͼ���������ң�
        { QStringLiteral("��"), 0 },
        { QStringLiteral("��"), 0 },
        { QStringLiteral("Բ"), 0 },
        { QStringLiteral("��Բ"), 0 },
        { QStringLiteral("ƽ��"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("Բ��"), 0 },
        { QStringLiteral("Բ׶"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("����������"), 0 },
        { QStringLiteral("�������ͱ���"), 0 },

        // �������һ�к���
        { QStringLiteral("���弸��Ԫ��"), 0 },
        { QStringLiteral("��ʾ��ϵ�"), 0 },
        { QStringLiteral("������ϵ�"), 0 },
        { QStringLiteral("�л���ϵ�ɼ���"), 0 },
        { QStringLiteral("����CM����"), 0 },
        { QStringLiteral("ɾ��δʹ�õ�CM����"), 0 },
        { QStringLiteral("�滻����Ԫ��"), 0 },

        // �������Ҫ�ų����С����У�ͼ���������ң�
        { QStringLiteral("�ཻ"), 0 },
        { QStringLiteral("�Գ�Ԫ��"), 0 },
        { QStringLiteral("�޸ĳߴ�"), 0 },
        { QStringLiteral("���"), 0 },
        { QStringLiteral("ת��"), 0 },
        { QStringLiteral("����Ԫ��"), 0 },
        { QStringLiteral("ͶӰ"), 0 },
        { QStringLiteral("��ȡ��С/����"), 0 },
        { QStringLiteral("��ȡ������"), 0 },

        // �������һ�к���
        { QStringLiteral("ת��������"), 1 },
    };

    // ��Ҫ�����������е��� 8 ����ť
    const QStringList twoRowGroup = {
         QStringLiteral("��"),
         QStringLiteral("��"), 
         QStringLiteral("Բ"),
         QStringLiteral("��Բ"),
         QStringLiteral("ƽ��"), 
         QStringLiteral("����"),
         QStringLiteral("Բ��"), 
         QStringLiteral("Բ׶"), 
         QStringLiteral("����"), 
         QStringLiteral("����������"), 
         QStringLiteral("�������ͱ���"),
        // �������ѡ���Բ3D��Ҳ�Ž������������������� 2��5 ���滻������һ������
    };


    const QStringList twoRowGroup02 = {
        QStringLiteral("�ཻ"),
        QStringLiteral("�Գ�Ԫ��"),
        QStringLiteral("�޸ĳߴ�"),
        QStringLiteral("���"),
        QStringLiteral("ת��"),
        QStringLiteral("����Ԫ��"),
        QStringLiteral("ͶӰ"),
        QStringLiteral("��ȡ��С/����"),
        QStringLiteral("��ȡ������"),
    };


    QWidget* gridHolder_05 = nullptr;//���ָ�����˼�� ���������Ǹ� 2��4 ��С����
    QGridLayout* grid_05 = nullptr;//���ָ�������������Ǹ�С����Ĳ���
    int groupedCount_05 = 0;//��¼�Ѿ��Ž�С����İ�ť����

    QWidget* gridHolder02_05 = nullptr;
    QGridLayout* grid02_05 = nullptr;
    int groupedCount02_05 = 0;

    QWidget* gridHolder03_05 = nullptr;
    QGridLayout* grid03_05 = nullptr;
    int groupedCount03_05 = 0;

    for (const auto& action : actions05)
    {
        const bool inGroup_05 = twoRowGroup.contains(action.text);//contains�������ĳ��Ԫ���Ƿ����б��� ����true��false
        const bool inGroup02_05 = twoRowGroup02.contains(action.text);
     

        auto* button = new QToolButton(ribbon05);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));

        if (inGroup_05)
        {
            if (!gridHolder_05) {//�ȼ���gridholder == nullptr
                gridHolder_05 = new QWidget(ribbon05);
                grid_05 = new QGridLayout(gridHolder_05);
                grid_05->setContentsMargins(4, 2, 4, 2);
                grid_05->setHorizontalSpacing(8);//ˮƽ���
                grid_05->setVerticalSpacing(4);//��ֱ���
                layout05->addWidget(gridHolder_05); // ��С������뵽�� ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // С icon
			button->setMinimumSize(QSize(90, 20));       
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);//
            button->setText(action.text);                 // �������֣����ٻ���

            int row = groupedCount_05 / 4;
            int col = groupedCount_05 % 4;
            grid_05->addWidget(button, row, col);//������������˼�ǣ�Ҫ��ӵĿؼ����кš��к�
            ++groupedCount_05;
            continue;
        }
        if (inGroup02_05) {
            if (!gridHolder02_05)
            {
                gridHolder02_05 = new QWidget(ribbon05);
                grid02_05 = new QGridLayout(gridHolder02_05);//QGridLayout�������ǰѿؼ�����һ��������
				grid02_05->setContentsMargins(4, 2, 4, 2);
                grid02_05->setHorizontalSpacing(8);
                grid02_05->setVerticalSpacing(4);
                layout05->addWidget(gridHolder02_05);
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // С icon
            button->setMinimumSize(QSize(90, 20));
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);                 // �������֣����ٻ���

            int row = groupedCount02_05 / 4;
            int col = groupedCount02_05 % 4;

            //���ֹ���  grid
            grid02_05->addWidget(button, row, col);
            ++groupedCount02_05;
            continue;
        }
        else {
            // �Ǹ÷��飺�Ծ�һ�к���
            layout05->addWidget(button);
        }
        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/ROI_from_geomety_element.png"), QStringLiteral("���Լ���Ԫ�ص�ROI"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/surface_mash_from_geomety_element.PNG"), QStringLiteral("���Լ���Ԫ�صı�������"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/CAD_from_geomety_element.PNG"), QStringLiteral("���Լ���Ԫ�ص� CAD"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/geometry_element_export_to_surface_mash.PNG"), QStringLiteral("������Ԫ�ص���Ϊ��������"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/geometry_element_export_to_CAD.PNG"), QStringLiteral("������Ԫ�ص���Ϊ CAD"));
            menu->addAction(QIcon(":/geometry_icons02/icons_other/geometry_icons/transformation_and_export_pull_down_menu/untitle.PNG"), QStringLiteral("������ϵ���Ϣ"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
    }
    layout05->addStretch();
    return ribbon05;
}
