#pragma once
#include <QWidget>
#include <QString>
#include <QList>

struct MenuSpec {
    QString menuTitle;
    QList<int> actionIDs;
};

class PageBase : public QWidget {
    Q_OBJECT
public:
    explicit PageBase(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~PageBase() = default;

    // 页面生命周期 钩子在进入或离开时可做资源准备与清理 
    virtual void onEnter() {}
    virtual void onLeave() {}

    //  页面菜单接口：返回需要在菜单栏展示的动作集合
    virtual QList<MenuSpec> menus() const { return {}; }

signals:
    void requestSwitchTo(const QString& pageId); //  提供信号请求主界面切换页面 
};
