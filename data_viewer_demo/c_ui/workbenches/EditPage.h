#pragma once
#include <QWidget>
#include <QPointer>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QIcon>
#include <QDebug>

class QToolButton;

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon(QWidget* parent);//��������

	QWidget* buildRibbonGroup(const QString& title, const QStringList& actions, QWidget* parent);//���캯���Ĺ��ܷ���

	QIcon buildIcon() const;//ͼ����غ���
    
};

