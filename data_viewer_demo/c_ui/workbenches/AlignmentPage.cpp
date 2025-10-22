#include "AlignmentPage.h"
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
        { QStringLiteral("�����϶���"),  ":/alignment_icons/icons_other/align_icons/best_fit_align.png" },
        { QStringLiteral("3-2-1����"),  ":/alignment_icons/icons_other/align_icons/3-2-1_align.png" },
        { QStringLiteral("���������Ķ���"), ":/alignment_icons/icons_other/align_icons/based_on_feature_align.png" },
        { QStringLiteral("���������"),  ":/alignment_icons/icons_other/align_icons/in_order_align.png" },
        { QStringLiteral("RPS����"),  ":/alignment_icons/icons_other/align_icons/RPS_align.png" },
        { QStringLiteral("���ڼ���Ԫ�ص����"),  ":/alignment_icons/icons_other/align_icons/based_on_geometry_element_best_fit.png" },
        { QStringLiteral("�༭��ǰ����"),  ":/alignment_icons/icons_other/align_icons/edit_current_align.png" },
        { QStringLiteral("��3-2-1����"),  ":/alignment_icons/icons_other/align_icons/simple_3-2-1_align.png" },
        { QStringLiteral("�򵥶���"),  ":/alignment_icons/icons_other/align_icons/simple_align.png" },
        { QStringLiteral("����Ƭͼ���뵽����"),      ":/alignment_icons/icons_other/align_icons/slice_images_align_to_obj.png" },
        { QStringLiteral("����ϵԭ��"),        ":/alignment_icons/icons_other/align_icons/coordinate_origin.png" },
        { QStringLiteral("����ϵ�༭��"),        ":/alignment_icons/icons_other/align_icons/coordinate_system_editor.png" },
        { QStringLiteral("�洢����"),        ":/alignment_icons/icons_other/align_icons/store_align.png" },
        { QStringLiteral("Ӧ�ö���"),":/alignment_icons/icons_other/align_icons/apply_align.png" },
        { QStringLiteral("����ת��"),":/alignment_icons/icons_other/align_icons/copy_transformation.png" },
        { QStringLiteral("ճ��ת��"),  ":/alignment_icons/icons_other/align_icons/paste_transformation.png" },
        { QStringLiteral("����ת��"),  ":/alignment_icons/icons_other/align_icons/reset_transformation.png" },
        { QStringLiteral("����"),  ":/alignment_icons/icons_other/align_icons/lock.png" },
        { QStringLiteral("����"),  ":/alignment_icons/icons_other/align_icons/unlock.png" },
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


AlignmentPage::AlignmentPage(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("alignmentEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout04 = new QVBoxLayout(this);
    layout04->setContentsMargins(0, 0, 0, 0);
    layout04->setSpacing(3);

    // ����������
    layout04->addWidget(buildRibbon04(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("���빦������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout04->addWidget(placeholder, 1);
}

QWidget* AlignmentPage::buildRibbon04(QWidget* parent)
{
    auto* ribbon04 = new QFrame(parent);//ribbon04��"  "������������
    ribbon04->setObjectName(QStringLiteral("alignmentRibbon"));
    ribbon04->setStyleSheet(QStringLiteral(
        "QFrame#alignmentRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout04 = new QHBoxLayout(ribbon04);
    layout04->setContentsMargins(4, 4, 4, 4);
    layout04->setSpacing(1);

    struct RibbonAction04 {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction04> actions04 = {
        { QStringLiteral("�����϶���"), 0 },
        { QStringLiteral("3-2-1����"), 0 },
        { QStringLiteral("���������Ķ���"),0 },
        { QStringLiteral("���������"), 0 },
        { QStringLiteral("RPS����"), 0 },
        { QStringLiteral("���ڼ���Ԫ�ص����"), 0 },
        { QStringLiteral("�༭��ǰ����"), 0 },
        { QStringLiteral("��3-2-1����"), 0 },
        { QStringLiteral("�򵥶���"), 0 },
        { QStringLiteral("����Ƭͼ���뵽����"), 0 },
        { QStringLiteral("����ϵԭ��"), 0 },
        { QStringLiteral("����ϵ�༭��"), 0 },
        { QStringLiteral("�洢����"), 0 },
        { QStringLiteral("Ӧ�ö���"), 0 },
        { QStringLiteral("����ת��"), 0 },
        { QStringLiteral("ճ��ת��"), 0 },
        { QStringLiteral("����ת��"), 0 },
        { QStringLiteral("����"), 0 },
        { QStringLiteral("����"), 0 },
    };
    for (const auto& action : actions04)
    {
        auto* button = new QToolButton(ribbon04);
        button->setIcon(loadIconFor(action.text));
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setText(wrapByWidth(action.text, button->font(), 60));
        layout04->addWidget(button);
    }
    layout04->addStretch();
    return ribbon04;
}
