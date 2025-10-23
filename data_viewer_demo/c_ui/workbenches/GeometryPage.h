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

class GeometryPage : public QWidget
{
	Q_OBJECT
public:
	explicit GeometryPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon05(QWidget* parent);

};
