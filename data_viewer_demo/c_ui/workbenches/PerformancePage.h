#pragma once

#include <QWidget>
#include <QPointer>
#include <QString>
#include <memory>

#include "core/common/VtkMacros.h" // 自动检测 VTK 是否可用，避免手动定义遗漏。

class QLabel;
class QLineEdit;
class QPushButton;

#if USE_VTK
class QVTKOpenGLNativeWidget;
using PerformancePageViewerWidget = QVTKOpenGLNativeWidget;
#else
class QWidget;
using PerformancePageViewerWidget = QWidget;
#endif

namespace core::services {
    class OrthogonalMprService;
}

/**
提供四视图（轴状/矢状/冠状/原始体数据）的三向正交联动展示。
 */
class PerformancePage : public QWidget
{
    Q_OBJECT
public:
    explicit PerformancePage(QWidget* parent = nullptr);
    ~PerformancePage() override;

public slots:
    /**
     加载指定目录下的 DICOM 序列并刷新四个视图。
     */
    void loadDicomDirectory(const QString& directory);

private:
    /// 构建顶部操作栏和四视图布局。
    void buildUi();

    /// 连接按钮、输入框等信号槽。
    void wireSignals();

    /// 更新状态提示文本。
    void updateStatusLabel(const QString& text, bool isError);

#if USE_VTK
    /// 绑定 VTK 管线至四个 QVTK 窗口。
    void initializeVtkViews();
#endif

private:
    QPointer<QLineEdit> inputDirectory_;
    QPointer<QPushButton> btnBrowse_;
    QPointer<QPushButton> btnLoad_;
    QPointer<QLabel> statusLabel_;

	QPointer<PerformancePageViewerWidget> viewAxial_;//用于显示轴状视图。
    QPointer<PerformancePageViewerWidget> viewSagittal_;
    QPointer<PerformancePageViewerWidget> viewCoronal_;
    QPointer<PerformancePageViewerWidget> viewVolume_;

    std::unique_ptr<core::services::OrthogonalMprService> mprService_;
};