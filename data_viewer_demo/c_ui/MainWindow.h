#pragma once
#include <QMainWindow>
#include <QPointer>
#include <QPoint>
#include <QToolButton>
#include <QLabel>
#include <QStackedWidget>
#include <QTabBar>

class DocumentPage;
class EditPage;
class VolumePage;
class SelectPage;

class CTViewer : public QMainWindow
{
    Q_OBJECT
public:
    explicit CTViewer(QWidget* parent = nullptr);
    ~CTViewer();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void buildTitleBar();
    void buildCentral();
    void wireSignals();
    void setDefaults();
    void updateMaximizeButtonIcon();

private:
    // ---- 标题栏控件 ----
    QPointer<QWidget> titleBar_;
    QPointer<QWidget> titleLeftArea_;
    QPointer<QWidget> titleCenterArea_;
    QPointer<QLabel>  titleLabel_;
    QPointer<QToolButton> btnTitleUndo_;
    QPointer<QToolButton> btnTitleUndo02_;
    QPointer<QToolButton> btnMinimize_;
    QPointer<QToolButton> btnMaximize_;
    QPointer<QToolButton> btnClose_;
	QPointer<QTabBar> ribbontabBar_;//主界面状态栏指针

    bool draggingWindow_ = false;
    QPoint dragOffset_;

    // ---- 中央区 ----
    QPointer<QStackedWidget> stack_;
	QPointer<DocumentPage> pageDocument_;//文档页面的指针
	QPointer<EditPage> pageEdit_;
	QPointer<VolumePage> pageVolume_;
	QPointer<SelectPage> pageSelect_;
};
