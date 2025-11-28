#pragma once
#include <QWidget>
#include <QPointer>

namespace core::services {
	class OrthogonalMprService;
}
class ReconstructPage : public QWidget
{
    Q_OBJECT
public:
    /// 构造函数：搭建 2x2 网格
    explicit ReconstructPage(QWidget* parent = nullptr);
    /// 让页面与后端的 MPR 服务建立连接，并用当前 QVTK 视口初始化 2D/3D 渲染。
    bool initializeWithService(core::services::OrthogonalMprService* service);

private:
    /// 构建页面布局（如未启用 VTK，用占位 QWidget）
    void buildUi();

private:
    QPointer<QWidget> viewAxial_;
    QPointer<QWidget> viewSagittal_;
    QPointer<QWidget> viewCoronal_;
    QPointer<QWidget> viewReserved_;
};
