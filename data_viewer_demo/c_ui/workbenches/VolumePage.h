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

class VolumePage : public QWidget
{
	Q_OBJECT
public:
	explicit VolumePage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon02(QWidget* parent);
};
