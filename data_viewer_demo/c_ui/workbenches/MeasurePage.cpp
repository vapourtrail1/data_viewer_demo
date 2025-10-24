#include "MeasurePage.h"
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
        { QStringLiteral("�ߴ��ע"),  ":/new/prefix1/icons_other/measure_icons/size_label.PNG" },
        { QStringLiteral("���Գߴ�"),  ":/new/prefix1/icons_other/measure_icons/linear_dimension.PNG" },
        { QStringLiteral("ֱ�߶�"), ":/measure_icons/icons_other/measure_icons/straightness.PNG" },
        { QStringLiteral("ƽ���"),  ":/measure_icons/icons_other/measure_icons/flatness.PNG" },
        { QStringLiteral("Բ��"),  ":/measure_icons/icons_other/measure_icons/roundness.PNG" },
        { QStringLiteral("Բ����"),  ":/measure_icons/icons_other/measure_icons/Cylindricity.PNG" },
        { QStringLiteral("Բ׶��"),  ":/measure_icons/icons_other/measure_icons/cone_degree.PNG" },
        { QStringLiteral("���"),  ":/measure_icons/icons_other/measure_icons/sqhere_degree.PNG" },
        { QStringLiteral("��ֱ��"),  ":/measure_icons/icons_other/measure_icons/perpendicularity.PNG" },
        { QStringLiteral("ƽ�ж�"),      ":/measure_icons/icons_other/measure_icons/Parallelism.PNG" },
        { QStringLiteral("��б��"),        ":/measure_icons/icons_other/measure_icons/angularity.PNG" },
        { QStringLiteral("λ�ö�"),        ":/measure_icons/icons_other/measure_icons/location_degree.PNG" },
        { QStringLiteral("ͬ���"),        ":/measure_icons/icons_other/measure_icons/Coaxiality.PNG" },
        { QStringLiteral("�Գƶ�"),":/measure_icons/icons_other/measure_icons/symmetry_degree.PNG" },
        { QStringLiteral("��������"),":/measure_icons/icons_other/measure_icons/line_profile.PNG" },
        { QStringLiteral("��������"),  ":/measure_icons/icons_other/measure_icons/surface_profile.PNG" },
        { QStringLiteral("Բ����"),  ":/measure_icons/icons_other/measure_icons/circle_runout.PNG" },
        { QStringLiteral("ȫ����"),  ":/measure_icons/icons_other/measure_icons/total_runout.PNG" },
        { QStringLiteral("����"),  ":/new/prefix1/icons_other/measure_icons/option_pull_down_menu/setting.PNG" },
        { QStringLiteral("ָʾ��"),  ":/new/prefix1/icons_other/measure_icons/indicator.PNG" },
        { QStringLiteral("����"),  ":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/measure_tool.PNG" },
        { QStringLiteral("��׽ģʽ"),  ":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/capture_pattern.PNG" },
        { QStringLiteral("���²�׽���߿ص�"),  ":/new/prefix1/icons_other/measure_icons/re_capture_measure_tool_control_point.PNG" },
        { QStringLiteral("����ģ��"),  ":/new/prefix1/icons_other/measure_icons/measure_template_pull_down_menu/measure_template.PNG" },
        { QStringLiteral("����ģ��"),  ":/new/prefix1/icons_other/measure_icons/measure_tool_template_pull_down_menu/measure_tool_template.PNG" },
        { QStringLiteral("ָʾ��ģ��"),  ":/new/prefix1/icons_other/measure_icons/indicator_template_pull_down_menu/indicator_template.PNG" },
        { QStringLiteral("����ע��"),  ":/new/prefix1/icons_other/measure_icons/tolerance_annotation.PNG" },
        { QStringLiteral("CM���"),  ":/new/prefix1/icons_other/measure_icons/CM_result.PNG" },
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


MeasurePage::MeasurePage(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("measureEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageMeasure{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout06 = new QVBoxLayout(this);
    layout06->setContentsMargins(0, 0, 0, 0);
    layout06->setSpacing(3);

    // ����������
    layout06->addWidget(buildRibbon06(this));

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
    layout06->addWidget(placeholder, 1);
}

QWidget* MeasurePage::buildRibbon06(QWidget* parent)
{
    auto* ribbon06 = new QFrame(parent);//ribbon03��--������������
    ribbon06->setObjectName(QStringLiteral("measureRibbon"));
    ribbon06->setStyleSheet(QStringLiteral(
        "QFrame#measureRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout06 = new QHBoxLayout(ribbon06);
    layout06->setContentsMargins(4, 4, 4, 4);
    layout06->setSpacing(1);

    struct RibbonAction06 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction06> actions06 = {
        { QStringLiteral("�ߴ��ע"), 0 },
        { QStringLiteral("���Գߴ�"), 0 },
        //���Ҳһ��n��
        { QStringLiteral("ֱ�߶�"), 0 },
        { QStringLiteral("ƽ���"), 0 },
        { QStringLiteral("Բ��"), 0 },
        { QStringLiteral("Բ����"), 0 },
        { QStringLiteral("Բ׶��"), 0 },
        { QStringLiteral("���"), 0 },
        { QStringLiteral("��ֱ��"), 0 },
        { QStringLiteral("ƽ�ж�"), 0 },
        { QStringLiteral("��б��"), 0 },
        { QStringLiteral("λ�ö�"), 0 },
        { QStringLiteral("ͬ���"), 0 },
        { QStringLiteral("�Գƶ�"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("Բ����"), 0 },
        { QStringLiteral("ȫ����"), 0 },

        { QStringLiteral("����"), 1 },
        { QStringLiteral("ָʾ��"), 0 },
        { QStringLiteral("����"), 2 },
        { QStringLiteral("��׽ģʽ"), 3 },
        { QStringLiteral("���²�׽���߿ص�"), 0 },
        { QStringLiteral("����ģ��"), 4 },
        { QStringLiteral("����ģ��"), 5 },
        { QStringLiteral("ָʾ��ģ��"), 6 },
        { QStringLiteral("����ע��"), 0 },
        { QStringLiteral("CM���"), 0 },
    };

    // ��Ҫ�����������е��� 8 ����ť
    const QStringList twoRowGroup = {
        QStringLiteral("ֱ�߶�"), 
        QStringLiteral("ƽ���"), 
        QStringLiteral("Բ��"), 
        QStringLiteral("Բ����"), 
        QStringLiteral("Բ׶��"), 
        QStringLiteral("���"), 
        QStringLiteral("��ֱ��"), 
        QStringLiteral("ƽ�ж�"), 
        QStringLiteral("��б��"), 
        QStringLiteral("λ�ö�"), 
        QStringLiteral("ͬ���"), 
        QStringLiteral("�Գƶ�"), 
        QStringLiteral("��������"), 
        QStringLiteral("��������"), 
        QStringLiteral("Բ����"), 
        QStringLiteral("ȫ����"),
        // �������ѡ���Բ3D��Ҳ�Ž������������������� 2��5 ���滻������һ������
    };

    

    QWidget* gridHolder_06 = nullptr;//���ָ�����˼�� ���������Ǹ� 2��4 ��С����
    QGridLayout* grid_06 = nullptr;//���ָ�������������Ǹ�С����Ĳ���
    int groupedCount_06 = 0;//��¼�Ѿ��Ž�С����İ�ť����

    
    for (const auto& action : actions06)
    {
        const bool inGroup_06 = twoRowGroup.contains(action.text);//contains�������ĳ��Ԫ���Ƿ����б��� ����true��false
        auto* button = new QToolButton(ribbon06);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(32, 32));
        button->setMinimumSize(QSize(59, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 43));
        if (inGroup_06)
        {
            if (!gridHolder_06) {//�ȼ���gridholder == nullptr
                gridHolder_06 = new QWidget(ribbon06);
                grid_06 = new QGridLayout(gridHolder_06);
                grid_06->setContentsMargins(4, 2, 4, 2);
                grid_06->setHorizontalSpacing(8);//ˮƽ���
                grid_06->setVerticalSpacing(4);//��ֱ���
                layout06->addWidget(gridHolder_06); // ��С������뵽�� ribbon
            }
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            button->setIconSize(QSize(20, 20));           // С icon
            button->setMinimumSize(QSize(90, 20));       
            button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            button->setText(action.text);                 

            int row = groupedCount_06 / 6;
            int col = groupedCount_06 % 6;
            grid_06->addWidget(button, row, col);//������������˼�ǣ�Ҫ��ӵĿؼ����кš��к�
            ++groupedCount_06;
            continue;
        }
        
        else {
            // �Ǹ÷��飺�Ծ�һ�к���
            layout06->addWidget(button);
        }

        if (action.hasMenu == 1) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/option_pull_down_menu/regular_tolerance_preset.png"), QStringLiteral("���湫��Ԥ��"));
            menu->addAction(QIcon(":/measure_icons03/icons_other/measure_icons/option_pull_down_menu/tolerance_visible_setting.png"), QStringLiteral("������ӻ�����"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 2) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/distance.PNG"), QStringLiteral("����"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/angle_4points.PNG"), QStringLiteral("�Ƕ�(4����)"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/angle_3points.PNG"), QStringLiteral("�Ƕ�(3����)"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/fold_line_length.PNG"), QStringLiteral("���߳���"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/max_min_distance.PNG"), QStringLiteral("���/��С����"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_pull_down_menu/caliper.PNG"), QStringLiteral("����"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        if (action.hasMenu == 3) {
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/min.PNG"), QStringLiteral("��С"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/max.PNG"), QStringLiteral("���"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/gradient.PNG"), QStringLiteral("�ݶ�"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/surface.PNG"), QStringLiteral("����"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/local.PNG"), QStringLiteral("�ֲ�"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/capture_pattern_pull_down_menu/off.PNG"), QStringLiteral("��"));
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
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_template_pull_down_menu/input_measure_template.PNG"), QStringLiteral("�������ģ��"));
            menu->addAction(QIcon(":/measure_icons03/icons_other/measure_icons/measure_template_pull_down_menu/output_measure_template.png"), QStringLiteral("��������ģ��"));
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
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_template_pull_down_menu/intput_measure_tool_template.PNG"), QStringLiteral("��������ģ��"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/measure_tool_template_pull_down_menu/output_measure_tool_template.PNG"), QStringLiteral("��������ģ��"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        else if (action.hasMenu == 6) {
            auto menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/indicator_template_pull_down_menu/input_indicator_template.PNG"), QStringLiteral("����ָʾ��ģ��"));
            menu->addAction(QIcon(":/new/prefix1/icons_other/measure_icons/indicator_template_pull_down_menu/output_indicator_template.PNG"), QStringLiteral("����ָʾ��ģ��"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }
        
    }
    layout06->addStretch();
    return ribbon06;
}
