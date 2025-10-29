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

class ReportPage : public QWidget
{
	Q_OBJECT
public:
	explicit ReportPage(QWidget* parent = nullptr);
private:
	QWidget* buildRibbon10(QWidget* parent);
};
