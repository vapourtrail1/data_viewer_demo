#pragma once
#include <QWidget>
#include <QPointer>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QDebug>

class DocumentPage : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentPage(QWidget* parent = nullptr);

signals:
    void requestSwitchTo(const QString& page);
    void moduleClicked(const QString& info);
    void recentOpenRequested(const QString& projectName);

private:
    void buildUi();
    void buildLeftDock();
    QWidget* buildRightContent(QWidget* parent);
    void wireLeftDockSignals();
    void wireRightContentSignals();

    QPointer<QListWidget> listNav_;
    QPointer<QPushButton> btnUndo_;
    QPointer<QPushButton> btnKeep_;
    QPointer<QPushButton> btnVisCheck_;
    QPointer<QPushButton> btnPorosity_;
    QPointer<QPushButton> btnMetrology_;
    QPointer<QPushButton> btnMaterial_;
    QPointer<QTableWidget> tableRecent_;
};

