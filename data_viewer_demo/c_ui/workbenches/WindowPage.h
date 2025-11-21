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

class WindowPage : public QWidget
{
	Q_OBJECT
public:
	explicit WindowPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon12(QWidget* parent);
};
