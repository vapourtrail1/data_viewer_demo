#include "CADAndThen.h"
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
        { QStringLiteral("�򻯱�������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/simplify_surface_mesh.PNG" },
        { QStringLiteral("ɾ�������ķ���"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/delete_lonely_component.PNG" },
        { QStringLiteral("��ת���淽��"), ":/cad_icons/icons_other/CAD_surfacemesh_icons/reverse_surface_direction.PNG" },
        { QStringLiteral("���¼���CAD����"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/re-cal_CAD_mesh.PNG" },
        { QStringLiteral("�ϲ������������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/merge_surface_mesh_obj.PNG" },
        { QStringLiteral("��������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/deformation_mesh.PNG" },
        { QStringLiteral("ģ������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/mold_modify.PNG" },
        { QStringLiteral("��������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/compensation_mesh.PNG" },
        { QStringLiteral("������������"),  ":/cad_icons/icons_other/CAD_surfacemesh_icons/lteration_compensation_mesh.PNG" },
        { QStringLiteral("���γ�"),      ":/cad_icons/icons_other/CAD_surfacemesh_icons/deformation_field.PNG" },
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


CADAndThen::CADAndThen(QWidget* parent)
    : QWidget(parent)
{
    // ����ҳ�����
    setObjectName(QStringLiteral("CADEdit"));
    setStyleSheet(QStringLiteral(
        "QWidget#CADEdit{background-color:#2b2b2b;}"
        "QLabel{color:#f0f0f0;}"
        "QToolButton{color:#f7f7f7; border-radius:6px; padding:6px;}"
        "QToolButton:hover{background-color:#3a3a3a;}"));

    auto* layout07 = new QVBoxLayout(this);
    layout07->setContentsMargins(0, 0, 0, 0);
    layout07->setSpacing(3);

    // ����������
    layout07->addWidget(buildRibbon07(this));

    // Ԥ����������ռλ�����ں���������ı༭���߽���
    auto* placeholder = new QFrame(this);
    placeholder->setObjectName(QStringLiteral("editContentPlaceholder"));
    placeholder->setStyleSheet(QStringLiteral(
        "QFrame#editContentPlaceholder{background-color:#1d1d1d; border-radius:8px; border:1px solid #313131;}"
        "QFrame#editContentPlaceholder QLabel{color:#cccccc;}"));

    auto* placeholderLayout = new QVBoxLayout(placeholder);
    placeholderLayout->setContentsMargins(0, 0, 0, 0);
    placeholderLayout->setSpacing(1);

    auto* title = new QLabel(QStringLiteral("CAD/��������������������"), placeholder);
    title->setStyleSheet(QStringLiteral("font-size:16px; font-weight:600;"));
    placeholderLayout->addWidget(title);

    auto* desc = new QLabel(QStringLiteral("������Լ�����չ����༭�����ε����Ȳ������档"), placeholder);
    desc->setWordWrap(true);
    desc->setStyleSheet(QStringLiteral("font-size:13px;"));
    placeholderLayout->addWidget(desc);
    placeholderLayout->addStretch();
    layout07->addWidget(placeholder, 1);
}

QWidget* CADAndThen::buildRibbon07(QWidget* parent)
{
    // ��������������
    auto* ribbon07 = new QFrame(parent);
    ribbon07->setObjectName(QStringLiteral("CADRibbon"));
    ribbon07->setStyleSheet(QStringLiteral(
        "QFrame#CADRibbon{background-color:#322F30; border-radius:8px; border:1px solid #2b2b2b;}"
        "QToolButton{color:#e0e0e0; font-weight:600;}"));

    auto* layout07 = new QHBoxLayout(ribbon07);
    layout07->setContentsMargins(4, 4, 4, 4);
    layout07->setSpacing(1);

    /*const QIcon placeholderIcon = buildIcon(); */// Ԥ����ռλͼ�꣬�����а�ť����


    struct RibbonAction07
    {
        QString text;
        int hasMenu;
    };

    const QList<RibbonAction07> actions07 = {
        { QStringLiteral("�򻯱�������"), 0 },
        { QStringLiteral("ɾ�������ķ���"), 0 },
        { QStringLiteral("��ת���淽��"), 0 },
        { QStringLiteral("���¼���CAD����"), 0 },
        { QStringLiteral("�ϲ������������"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("ģ������"), 0 },
        { QStringLiteral("��������"), 0 },
        { QStringLiteral("������������"), 0 },
        { QStringLiteral("���γ�"), 0 }
    };


    for (const auto& action : actions07) {
        // ÿ�����ܶ�ʹ��ͼ��,���ֵ���ʽչʾ
        auto* button = new QToolButton(ribbon07);
        QString wrappedText = wrapByWidth(action.text, button->font(), 51);
        button->setText(wrappedText);
        button->setIcon(loadIconFor(action.text));
        button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        button->setIconSize(QSize(40, 40));
        button->setMinimumSize(QSize(70, 90));
        layout07->addWidget(button);
    }
    layout07->addStretch();
    return ribbon07;
}

