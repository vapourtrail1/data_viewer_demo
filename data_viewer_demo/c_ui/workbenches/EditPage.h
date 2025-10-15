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
	QWidget* buildRibbon(QWidget* parent);//辅助函数

	QWidget* buildRibbonGroup(const QString& title, const QStringList& actions, QWidget* parent);//构造函数的功能分区

	QIcon buildIcon() const;//图标加载函数
    
};

