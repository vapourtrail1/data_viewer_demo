#pragma once
#include <QMainWindow>
#include <QPointer>
#include <QPoint>
#include <QToolButton>
#include <QLabel>
#include <QStackedWidget>
#include <QTabBar>

class DocumentPage;
class StartPagePage;
class EditPage;
class VolumePage;
class SelectPage;
class AlignmentPage;
class GeometryPage;
class MeasurePage;
class CADAndThen;
class AnalysisPage;
class WindowPage;
class ReportPage;
class AnimationPage;
class PerformancePage;
class ReconstructPage;

// 声明
namespace core::services {
	class OrthogonalMprService;
}

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
    void mountMprViewsToPage(QWidget* page);

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
    QPointer<StartPagePage> pageStart_;
	QPointer<EditPage> pageEdit_;
	QPointer<VolumePage> pageVolume_;
	QPointer<SelectPage> pageSelect_;
    QPointer<AlignmentPage> pageAlignment_;
    QPointer<GeometryPage> pageGeometry_;
    QPointer<MeasurePage> pageMeasure_;
    QPointer<CADAndThen> pageCAD_;
	QPointer<AnalysisPage> pageAnalysis_;
    QPointer<WindowPage> pageWindow_;
	QPointer<ReportPage> pageReport_;
	QPointer<AnimationPage> pageAnimation_;
	QPointer<PerformancePage> pagePerformance_;
    QPointer<ReconstructPage> mprViews_;
    //测量
	core::services::OrthogonalMprService* currentMprService_ = nullptr;
};

//