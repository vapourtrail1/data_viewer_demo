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

class MeasurePage : public QWidget
{
	Q_OBJECT
public:
	explicit MeasurePage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon06(QWidget* parent);

};
