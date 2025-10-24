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

class AnalysisPage : public QWidget
{
	Q_OBJECT
public:
	explicit AnalysisPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon08(QWidget* parent);

};
