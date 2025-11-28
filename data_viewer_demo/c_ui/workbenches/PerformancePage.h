#pragma once

#include <QWidget>
#include <QPointer>

#if USE_VTK
class QVTKOpenGLNativeWidget;
using PerformancePageViewerWidget = QVTKOpenGLNativeWidget;
#else
class QWidget;
using PerformancePageViewerWidget = QWidget;
#endif

//提供四视图（轴状/矢状/冠状/原始体数据）的三向正交联动展示。
class PerformancePage : public QWidget
{
    Q_OBJECT
public:
    explicit PerformancePage(QWidget* parent = nullptr);
    ~PerformancePage() override;

private:
    /// 构建顶部操作栏和四视图布局。
    void buildUi();

private:
	QPointer<PerformancePageViewerWidget> viewAxial_;//用于显示轴状视图。
    QPointer<PerformancePageViewerWidget> viewSagittal_;
    QPointer<PerformancePageViewerWidget> viewCoronal_;
    QPointer<PerformancePageViewerWidget> viewVolume_;
};