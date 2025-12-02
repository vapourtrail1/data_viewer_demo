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

class StartPagePage : public QWidget
{
	Q_OBJECT
public:
	explicit StartPagePage(QWidget* parent = nullptr);

signals:
	// 点击“距离”按钮时向外通知，让主窗口去处理实际测量逻辑。
	void distanceRequested();

private:
	QWidget* buildRibbon01(QWidget* parent);
	/*void openDicomDistance();*/

	//成员
	/*QPointer<QFrame> ribbon01_;*/
};
