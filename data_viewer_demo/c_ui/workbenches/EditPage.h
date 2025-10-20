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

class EditPage : public QWidget
{
    Q_OBJECT
public:
    explicit EditPage(QWidget* parent = nullptr);

private:
	QWidget* buildRibbon(QWidget* parent);//¸¨Öúº¯Êý

};

