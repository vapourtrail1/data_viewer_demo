#include "VolumePage.h"
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
        if (lineWidth + w  > maxWidthPx) {
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
        { QStringLiteral("������"),  ":/volume_icons/icons_other/volume_icons/split_volume.png" },
        { QStringLiteral("����ⶨ"),  ":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/surface_measure_and_based_on_iosvalue.png" },
        { QStringLiteral("ɾ������ⶨ"), ":/volume_icons/icons_other/volume_icons/delete_surface_measure.png" },
        { QStringLiteral("�������"),  ":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/volume_data.png" },
        { QStringLiteral("��������������"),  ":/volume_icons/icons_other/volume_icons/feature_scale.png" },
        { QStringLiteral("�ֶ�����"),  ":/volume_icons/icons_other/volume_icons/manual_scale.png" },
        { QStringLiteral("��������"),  ":/icons/icons/delete.png" },
        { QStringLiteral("ѡ����ɫ"),  ":/volume_icons/icons_other/volume_icons/choose_color.png" },
        { QStringLiteral("���"),  ":/volume_icons/icons_other/volume_icons/fill.png" },
        { QStringLiteral("����Ӧ��˹"),      ":/volume_icons/icons_other/volume_icons/adaptive_gaussian.png" },
        { QStringLiteral("�Ǿֲ���ֵ"),        ":/volume_icons/icons_other/volume_icons/non_local_mean.png" },
        { QStringLiteral("���"),        ":/volume_icons/icons_other/volume_icons/convolution.png" },
        { QStringLiteral("��˹"),        ":/volume_icons/icons_other/volume_icons/gaussian.png" },
        { QStringLiteral("��"),":/volume_icons/icons_other/volume_icons/frame.png" },
        { QStringLiteral("ƫ��"),":/volume_icons/icons_other/volume_icons/deviation.png" },
        { QStringLiteral("��ֵ"),  ":/volume_icons/icons_other/volume_icons/mid_value.png" },
        { QStringLiteral("��ʴ"),  ":/volume_icons/icons_other/volume_icons/erosion.png" },
        { QStringLiteral("����"),  ":/volume_icons/icons_other/volume_icons/dilation.png" },
        { QStringLiteral("Ӧ�ò�͸��ӳ��"),  ":/volume_icons/icons_other/volume_icons/apply_opacity_mapping.png" },
        { QStringLiteral("FIB-SEM����"),  ":/volume_icons/icons_other/volume_icons/FIB-SEM_fix.png" },
        { QStringLiteral("�ϲ������²���"),  ":/volume_icons/icons_other/volume_icons/merge_and_resample.png" },
        { QStringLiteral("���ͶӰ��"),  ":/volume_icons/icons_other/volume_icons/volume_projector.png" },
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


VolumePage::VolumePage(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("volumeEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout02 = new QVBoxLayout(this);
    layout02->setContentsMargins(0, 0, 0, 0);
    layout02->setSpacing(3);

    // ����������
    layout02->addWidget(buildRibbon02(this));

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
    layout02->addWidget(placeholder, 1);
}

QWidget* VolumePage::buildRibbon02(QWidget* parent)
{
    // ��������������
    auto* ribbon02 = new QFrame(parent);
    ribbon02->setObjectName(QStringLiteral("editRibbon"));
    ribbon02->setStyleSheet(QStringLiteral(
        "QFrame#editRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout02 = new QHBoxLayout(ribbon02);
    layout02->setContentsMargins(4, 4, 4, 4);
    layout02->setSpacing(1);

    /*const QIcon placeholderIcon = buildIcon(); */// Ԥ����ռλͼ�꣬�����а�ť����


    struct RibbonAction02
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction02> actions02 = {
        { QStringLiteral("������"), 0 },
        { QStringLiteral("����ⶨ"), 1 },
        { QStringLiteral("ɾ������ⶨ"), 0 },
        { QStringLiteral("�������"), 2 },
        { QStringLiteral("��������������"), 0 },
        { QStringLiteral("�ֶ�����"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("ѡ����ɫ"), 0 },
        { QStringLiteral("���"), 0 },
        { QStringLiteral("����Ӧ��˹"), 0 },
        { QStringLiteral("�Ǿֲ���ֵ"), 0 },
        { QStringLiteral("���"), 0 },
        { QStringLiteral("��˹"), 0 },
        { QStringLiteral("��"), 0 },
        { QStringLiteral("ƫ��"), 0 },
        { QStringLiteral("��ֵ"), 0 },
        { QStringLiteral("��ʴ"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("Ӧ�ò�͸����"), 0 },
        { QStringLiteral("FIB-SEM ����"), 0 },
        { QStringLiteral("�ϲ������²���"), 0 },
        { QStringLiteral("���ͶӰ��"), 0 }
    };


    for (const auto& action : actions02) {
        // ÿ�����ܶ�ʹ��ͼ��,���ֵ���ʽչʾ
        auto* button = new QToolButton(ribbon02);
        QString wrappedText = wrapByWidth(action.text, button->font(), 51);
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
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/surface_measure_and_based_on_iosvalue.png"), QStringLiteral("���ڵ�ֵ"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/advanced_classic.png"), QStringLiteral("�߼�(����)"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/advanced_multi_material.png"), QStringLiteral("�߼�(�����)"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/fixed_contour.png"), QStringLiteral("�̶�����"));
            menu->addAction(QIcon(":/volume_icons/icons_other/volume_icons/surface_measure_pull_down_menu/edit_surface_measure.png"), QStringLiteral("�༭����ⶨ"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        if (action.hasMenu == 2) {
            auto* menu02 = new QMenu(button);
            menu02->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu02->addAction(QIcon(":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/volume_data.png"), QStringLiteral("�������"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/create_synthetic_volume_data.png"), QStringLiteral("�����ϳ��������"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/delete_volume_data.png"), QStringLiteral("ɾ���������"));
            menu02->addAction(QIcon(":/volume_icons_2/icons_other/volume_icons/volume_data_pull_down_menu/uninstall_volume_data.png"), QStringLiteral("ж���������"));
            menu02->addAction(QIcon(":/volume_icons/icons_other/volume_icons/volume_data_pull_down_menu/reload_volume_data.png"), QStringLiteral("���¼����������"));
            button->setMenu(menu02);
            button->setPopupMode(QToolButton::InstantPopup);//�����ťʱֱ�ӵ����˵�
        }
        layout02->addWidget(button);
    }
    layout02->addStretch();
    return ribbon02;
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