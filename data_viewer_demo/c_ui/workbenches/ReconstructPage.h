#pragma once
#include <QWidget>
#include <QPointer>

class ReconstructPage : public QWidget
{
    Q_OBJECT
public:
    /// 构造函数：搭建 2x2 网格（轴状/矢状/冠状 + 预留）
    explicit ReconstructPage(QWidget* parent = nullptr);

private:
    /// 构建页面布局（如未启用 VTK，用占位 QWidget）
    void buildUi();

private:
    QPointer<QWidget> viewAxial_;
    QPointer<QWidget> viewSagittal_;
    QPointer<QWidget> viewCoronal_;
    QPointer<QWidget> viewReserved_;
};
