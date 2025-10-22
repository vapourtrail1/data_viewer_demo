#pragma once
#include <QWidget>
#include <QPointer>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QIcon>
#include <QDebug>
#include <QFile>

//git t
class QToolButton;

class AlignmentPage : public QWidget
{
	Q_OBJECT
public:
	explicit AlignmentPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon04(QWidget* parent);

};
