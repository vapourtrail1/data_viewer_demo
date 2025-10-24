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

class CADAndThen : public QWidget
{
	Q_OBJECT
public:
	explicit CADAndThen(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon07(QWidget* parent);

};
