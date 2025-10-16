#include "EditPage.h"
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

//static���������޶�������ʾ�ú������ڵ�ǰ�ļ��ڿɼ�����ֹ������ͻ
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
        if (lineWidth + (w*1.2) > maxWidthPx) {
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

EditPage::EditPage(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("pageEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#pageEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(3);

    // ����������
    layout->addWidget(buildRibbon(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("�༭��������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();

    layout->addWidget(placeholder, 1);
}

QWidget* EditPage::buildRibbon(QWidget* parent)
{
    // ��������������
    auto* ribbon = new QFrame(parent);
    ribbon->setObjectName(QStringLiteral("editRibbon"));
    ribbon->setStyleSheet(QStringLiteral(
        "QFrame#editRibbon{background-color:#1f1f1f; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout = new QHBoxLayout(ribbon);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(1);

    const QIcon placeholderIcon = buildIcon(); // Ԥ����ռλͼ�꣬�����а�ť����

    
    struct RibbonAction
    {
        QString text;
        bool hasMenu;
    };

    const QList<RibbonAction> actions = {
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


	for (const auto& action : actions) {
        // ÿ�����ܶ�ʹ��ͼ��,���ֵ���ʽչʾ
        auto* button = new QToolButton(ribbon); 
        QString wrappedText = wrapByWidth(action.text, button->font(), 70);
        button->setText(wrappedText);
        button->setIcon(placeholderIcon);
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(56, 56));
        button->setMinimumSize(QSize(85, 90));

       
        if (action.hasMenu) {
            // ת��Ϊ���� ��Ҫ������չ
            auto* menu = new QMenu(button);
            menu->addAction(QStringLiteral("ռλѡ�� A"));
            menu->addAction(QStringLiteral("ռλѡ�� B"));
            button->setMenu(menu);
            button->setPopupMode(QToolButton::InstantPopup);
        }

        layout->addWidget(button);
    }

    layout->addStretch();
    return ribbon;
}

QIcon EditPage::buildIcon() const
{
    // ����һ����ɫ�ķ���ռλͼ�꣬���Ѻ����滻Ϊ��ʵ��Դ
    QPixmap pixmap(48, 48);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(QStringLiteral("#4a4a4a")));
    painter.setPen(QPen(QColor(QStringLiteral("#6c6c6c")), 2));
    painter.drawRoundedRect(pixmap.rect().adjusted(3, 3, -3, -3), 8, 8);

    painter.setPen(QPen(QColor(QStringLiteral("#bdbdbd"))));
    painter.setFont(QFont(QStringLiteral("Microsoft YaHei"), 8, QFont::Bold));
    painter.drawText(pixmap.rect(), Qt::AlignCenter, QStringLiteral("ICON"));

    painter.end();
    return QIcon(pixmap);
}