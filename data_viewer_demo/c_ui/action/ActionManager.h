#pragma once
#include <QObject>
#include <QHash>

class QAction;
class QMenuBar;
struct MenuSpec;

class ActionManager : public QObject {
    Q_OBJECT
public:
    explicit ActionManager(QObject* parent = nullptr);
    QAction* action(int id);
    void buildMenuBar(QMenuBar* bar, const QList<MenuSpec>& spec);
signals:
    void triggered(int id);
private:
    QHash<int, QAction*> map_;
};
