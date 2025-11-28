#pragma once
#include <QWidget>
#include <QPointer>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QDebug>
#include <QDialog>
#include <memory>

class QVBoxLayout;

namespace core::services {
    class OrthogonalMprService;
}

class DocumentPage : public QWidget
{
    Q_OBJECT
public:
    explicit DocumentPage(QWidget* parent = nullptr);

signals:
    void requestSwitchTo(const QString& page);
    //可以 emit requestSwithTo("EditPage")
    void moduleClicked(const QString& info);
    void recentOpenRequested(const QString& projectName);
	void dicomLoaded(core::services::OrthogonalMprService* service);

private:
    void buildUi();
    void buildLeftDock();
    QWidget* buildRightContent(QWidget* parent);
    void wireLeftDockSignals();
    void wireRightContentSignals();
	void showOpenDicomDialog();
    void updateDicomStatusLabel(const QString& text, bool isError);
    void loadDicomDirectory(const QString& dictory);

    QPointer<QListWidget> listNav_;
    QPointer<QPushButton> btnUndo_;
    QPointer<QPushButton> btnKeep_;
    QPointer<QPushButton> btnVisCheck_;
    QPointer<QPushButton> btnPorosity_;
    QPointer<QPushButton> btnMetrology_;
    QPointer<QPushButton> btnMaterial_;
    QPointer<QPushButton> btnDicomEntry_;
    QPointer<QTableWidget> tableRecent_;
    QPointer<QLineEdit> inputDicomDirectory_;
    QPointer<QPushButton> btnDicomBrowse_;
    QPointer<QPushButton> btnDicomLoad_;
    QPointer<QLabel> dicomStatusLabel_;
    QPointer<QDialog> dicomDialog_;

    std::unique_ptr<core::services::OrthogonalMprService> mprService_;
};

