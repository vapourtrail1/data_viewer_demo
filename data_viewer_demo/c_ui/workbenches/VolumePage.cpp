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


//git 
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
        if (lineWidth + (w * 1.2) > maxWidthPx) {
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
        { QStringLiteral("����"),  ":/icons/icons/undo.png" },
        { QStringLiteral("����"),  ":/icons/icons/redo.png" },
        { QStringLiteral("�ͷ��ڴ�/�����������"), ":/icons/icons/free_memory.png" },
        { QStringLiteral("����"),  ":/icons/icons/cut.png" },
        { QStringLiteral("����"),  ":/icons/icons/copy.png" },
        { QStringLiteral("ճ��"),  ":/icons/icons/paste.png" },
        { QStringLiteral("ɾ��"),  ":/icons/icons/delete.png" },
        { QStringLiteral("����������"),  ":/icons/icons/create_obj_group.png" },
        { QStringLiteral("ȡ��������"),  ":/icons/icons/cancel_obj_group.png" },
        { QStringLiteral("ת��Ϊ"),      ":/icons/icons/trans_pull_down_menu/trans.png" },
        { QStringLiteral("����"),        ":/icons/icons/property.png" },
        { QStringLiteral("��ת"),        ":/icons/icons/spin.png" },
        { QStringLiteral("�ƶ�"),        ":/icons/icons/move.png" },
        { QStringLiteral("���ƿ���״̬"),":/icons/icons/copy_visible_status.png" },
        { QStringLiteral("ճ������״̬"),":/icons/icons/paste_visible_status.png" },
        { QStringLiteral("����Ԫ��Ϣ"),  ":/icons/icons/copy_meta.png" },
        { QStringLiteral("ճ��Ԫ��Ϣ"),  ":/icons/icons/paste_meta.png" },
        { QStringLiteral("��̬������"),  ":/icons/icons/dynamic_rename.png" },
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
    auto* placeholder02 = new QFrame(this);
    placeholder02->setObjectName(QStringLiteral("volumeContentPlaceholder"));
    placeholder02->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout02 = new QVBoxLayout(placeholder02);
    placeholderLayout02->setContentsMargins(0, 0, 0, 0);
    placeholderLayout02->setSpacing(1);

    auto* title02 = new QLabel(QStringLiteral("�����������������"), placeholder02);
    title02->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout02->addWidget(title02);

    auto* desc02 = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder02);
    desc02->setWordWrap(true);
    desc02->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout02->addWidget(desc02);
    placeholderLayout02->addStretch();
    layout02->addWidget(placeholder02, 1);
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
        bool hasMenu;
    };

    const QList<RibbonAction02> actions02 = {
        { QStringLiteral("����"), false },
        { QStringLiteral("����"), false },
        { QStringLiteral("�ͷ��ڴ�/�����������"), false },
        { QStringLiteral("����"), false },
        { QStringLiteral("����"), false },
        { QStringLiteral("ճ��"), false },
        { QStringLiteral("ɾ��"), false },
        { QStringLiteral("����������"), false },
        { QStringLiteral("ȡ��������"), false },
        { QStringLiteral("ת��Ϊ"), true },
        { QStringLiteral("����"), false },
        { QStringLiteral("��ת"), false },
        { QStringLiteral("�ƶ�"), false },
        { QStringLiteral("���ƿ���״̬"), false },
        { QStringLiteral("ճ������״̬"), false },
        { QStringLiteral("����Ԫ��Ϣ"), false },
        { QStringLiteral("ճ��Ԫ��Ϣ"), false },
        { QStringLiteral("��̬������"), false }
    };


    for (const auto& action : actions02) {
        // ÿ�����ܶ�ʹ��ͼ��,���ֵ���ʽչʾ
        auto* button = new QToolButton(ribbon02);
        QString wrappedText = wrapByWidth(action.text, button->font(), 70);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(48, 48));
        button->setMinimumSize(QSize(85, 95));


        if (action.hasMenu) {
            // ת��Ϊ���� ��Ҫ������չ
            auto* menu = new QMenu(button);
            menu->setStyleSheet(QStringLiteral(
                "QMenu{background:#2b2b2b; border:1px solid #3a3a3a;}"
                "QMenu::item{color:#e0e0e0; padding:6px 24px;}"
                "QMenu::item:selected{background:#3a3a3a;}"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/volume.png"), QStringLiteral("���"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/volume_grid.png"), QStringLiteral("�������������"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/surface_grid.png"), QStringLiteral("��������"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/CAD.png"), QStringLiteral("CAD"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/golden_surface.png"), QStringLiteral("�ƽ����"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/analysis_surface.png"), QStringLiteral("��������е���ɫ��������"));
            menu->addAction(QIcon(":/icons/icons/trans_pull_down_menu/integration_grid.png"), QStringLiteral("�����������������ļ�������"));
            button->setMenu(menu);
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