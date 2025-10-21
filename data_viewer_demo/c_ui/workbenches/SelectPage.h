#pragma once
#include <QWidget>
#include <QPointer>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QIcon>
#include <QDebug>
#include <QFile>

class QToolButton;

class SelectPage : public QWidget
{
	Q_OBJECT
public:
	explicit SelectPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon03(QWidget* parent);

};
