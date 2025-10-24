#include "AnalysisPage.h"
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
        { QStringLiteral("ע��"),  ":/analyisis_icons/icons_other/analysis_icons/annotation.PNG" },
        { QStringLiteral("ʵʱֵ"),  ":/analyisis_icons/icons_other/analysis_icons/realtime_value_on.PNG" },
        { QStringLiteral("��϶/������"), ":/analysis_icons02/icons_other/analysis_icons/Pores_and_inclusions_pull_down_menu/a.PNG" },
        { QStringLiteral("P203"),  ":/analyisis_icons/icons_other/analysis_icons/P203.PNG" },
        { QStringLiteral("P202/P201"),  ":/analyisis_icons/icons_other/analysis_icons/P202_P201.PNG" },
        { QStringLiteral("��Ƽ�/ʵ��Ա�"),  ":/analyisis_icons/icons_other/analysis_icons/design_to_actual_compare.PNG" },
        { QStringLiteral("�ں�"),  ":/analyisis_icons/icons_other/analysis_icons/wall_thickness.PNG" },
        { QStringLiteral("λ��"),  ":/analyisis_icons/icons_other/analysis_icons/displacement.PNG" },
        { QStringLiteral("��ά���ϲ���"),  ":/analyisis_icons/icons_other/analysis_icons/fiber_recombination_material.PNG" },
        { QStringLiteral("��״/��ĩ�ṹ"),      ":/analyisis_icons/icons_other/analysis_icons/paozhuang_fenmu_struct.PNG" },
        { QStringLiteral("���������ؼ���"),        ":/analyisis_icons/icons_other/analysis_icons/data_volume_cal.PNG" },
        { QStringLiteral("�Ҷ�ֵ"),        ":/analyisis_icons/icons_other/analysis_icons/grey_value.PNG" },
        { QStringLiteral("��������"),        ":/analyisis_icons/icons_other/analysis_icons/data_quality.PNG" },
        { QStringLiteral("��Ƭͼ���"),":/analyisis_icons/icons_other/analysis_icons/slice_area.PNG" },
        { QStringLiteral("OCR"),":/analyisis_icons/icons_other/analysis_icons/OCR.PNG" },
        { QStringLiteral("�н�ģ��"),  ":/analyisis_icons/icons_other/analysis_icons/clip_simulation.PNG" },
        { QStringLiteral("�ṹ��ѧģ��"),  ":/analyisis_icons/icons_other/analysis_icons/struct_simulation.PNG" },
        { QStringLiteral("��������"),  ":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/transfer_phenomenon.PNG" },
        { QStringLiteral("��ؼ�Ƭ�������"),  ":/analyisis_icons/icons_other/analysis_icons/battery_analysis.PNG" },
        { QStringLiteral("���뼯������"),  ":/analyisis_icons/icons_other/analysis_icons/input_integration_mesh.PNG" },
        { QStringLiteral("������������"),  ":/analysis_icons02/icons_other/analysis_icons/create_integration_mesh_pull_down_menu/create_integration_mesh.PNG" },
        { QStringLiteral("����"),  ":/analysis_icons02/icons_other/analysis_icons/evaluate_pull_down_menu/evaluate.PNG" },
        { QStringLiteral("�������з���"),  ":/analyisis_icons/icons_other/analysis_icons/update_analysis.PNG" },
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


AnalysisPage::AnalysisPage (QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("analysisEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#analysisEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout08 = new QVBoxLayout(this);
    layout08->setContentsMargins(0, 0, 0, 0);
    layout08->setSpacing(3);

    // ����������
    layout08->addWidget(buildRibbon08(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("�����������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout08->addWidget(placeholder, 1);
}

QWidget* AnalysisPage::buildRibbon08(QWidget* parent)
{
    // ��������������
    auto* ribbon08 = new QFrame(parent);
    ribbon08->setObjectName(QStringLiteral("analysisRibbon"));
    ribbon08->setStyleSheet(QStringLiteral(
        "QFrame#analysisRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout08 = new QHBoxLayout(ribbon08);
    layout08->setContentsMargins(4, 4, 4, 4);
    layout08->setSpacing(1);

    /*const QIcon placeholderIcon = buildIcon(); */// Ԥ����ռλͼ�꣬�����а�ť����


    struct RibbonAction08
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction08> actions08 = {
        { QStringLiteral("ע��"), 0 },
        { QStringLiteral("ʵʱֵ"), 0 },
        { QStringLiteral("��϶/������"), 1 },
        { QStringLiteral("P203"), 0 },
        { QStringLiteral("P202/P201"), 0 },
        { QStringLiteral("��Ƽ�/ʵ��Ա�"), 0 },
        { QStringLiteral("�ں�"), 2 },
        { QStringLiteral("λ��"), 0 },
        { QStringLiteral("��ά���ϲ���"), 0 },
        { QStringLiteral("��״/��ĩ�ṹ"), 0 },
        { QStringLiteral("���������ؼ���"), 0 },
        { QStringLiteral("�Ҷ�ֵ"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("��Ƭͼ���"), 0 },
        { QStringLiteral("OCR"), 0 },
        { QStringLiteral("�н�ģ��"), 3 },
        { QStringLiteral("�ṹ��ѧģ��"), 0 },
        { QStringLiteral("��������"), 4 },
        { QStringLiteral("��ؼ�Ƭ�������"), 0 },
        { QStringLiteral("���뼯������"), 0 },
        { QStringLiteral("������������"), 5 },
        { QStringLiteral("����"), 6 },
        { QStringLiteral("�������з���"),0 }
    };
    for (const auto& action : actions08) {
        // ÿ�����ܶ�ʹ��ͼ��,���ֵ���ʽչʾ
        auto* button = new QToolButton(ribbon08);
        QString wrappedText = wrapByWidth(action.text, button->font(), 55);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));
        if (action.hasMenu == 1) {
            // ת��Ϊ���� ��Ҫ������չ
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/Pores_and_inclusions_pull_down_menu/EasyPore.PNG"), QStringLiteral("EASYPORE�㷨"));
            menu->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/Pores_and_inclusions_pull_down_menu/defeat_ROI.PNG"), QStringLiteral("����ȱ�ݵ�ROI"));
            menu->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/Pores_and_inclusions_pull_down_menu/DefX_threshold.PNG"), QStringLiteral("DefX/����ֵ�㷨"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 2) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/analyisis_icons/icons_other/analysis_icons/wall_thickness.PNG"), QStringLiteral("���߷�"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/ball.png"), QStringLiteral("���巨"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 3) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/analyisis_icons/icons_other/analysis_icons/clip_simulation.PNG"), QStringLiteral("�н�ģ��"));
            menu02->addAction(QIcon(":/analyisis_icons/icons_other/analysis_icons/clip_simulation.PNG"), QStringLiteral("���н�������ڳ�����"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 4) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/absolute_shentou_experiment.PNG"), QStringLiteral("������͸��ʵ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/absolute_shentou_tensor.PNG"), QStringLiteral("������͸������"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/molecule_diffusion_experical.PNG"), QStringLiteral("������ɢʵ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/molecule_diffusion_tensor.PNG"), QStringLiteral("������ɢ����"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/daore_experiment.PNG"), QStringLiteral("������ʵ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/daore_tensor.PNG"), QStringLiteral("����������"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/diandaolv_experiment.PNG"), QStringLiteral("�絼��ʵ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/diandaolv_tensor.PNG"), QStringLiteral("�絼������"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/transfor_phenomenon_pull_down_menu/line.PNG"), QStringLiteral("ëϸ��ѹ������"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 5) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/create_integration_mesh_pull_down_menu/create_regular_mesh.PNG"), QStringLiteral("�������򼯳�����"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/create_integration_mesh_pull_down_menu/geometry_create_integration_mesh.PNG"), QStringLiteral("�Ӽ���Ԫ���д�����������"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/create_integration_mesh_pull_down_menu/mesh.PNG"), QStringLiteral("��������������񴴽���������"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 6) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/evaluate_pull_down_menu/input_evaluate_mold.PNG"), QStringLiteral("��������ģ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/evaluate_pull_down_menu/output_evaluate_mold.PNG"), QStringLiteral("��������ģ��"));
            menu02->addAction(QIcon(":/analysis_icons02/icons_other/analysis_icons/evaluate_pull_down_menu/evaluate_property.PNG"), QStringLiteral("��������"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        layout08->addWidget(button);
    }
    layout08->addStretch();
    return ribbon08;
}

