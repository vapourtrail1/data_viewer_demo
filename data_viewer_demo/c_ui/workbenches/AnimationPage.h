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

class AnimationPage : public QWidget
{
	Q_OBJECT
public:
	explicit AnimationPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon11(QWidget* parent);

};
