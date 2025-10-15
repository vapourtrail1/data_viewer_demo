#include "c_ui/action/ActionManager.h"
#include "c_ui/PageBase.h"
#include "c_ui/action/ActionIDs.h"
#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QDebug>

ActionManager::ActionManager(QObject* parent) : QObject(parent) {}

QAction* ActionManager::action(int id) {
    if (map_.contains(id)) {
        return map_[id];
    }
    auto a = new QAction(this);
    // ---- 根据动作编号填充易读的中文标题，避免乱码导致的不可用菜单 ----
    switch (id) {
    case Act_NewProject:    a->setText(QObject::tr("新建"));            break;
    case Act_Open:          a->setText(QObject::tr("打开"));            break;
    case Act_Save:          a->setText(QObject::tr("保存"));            break;
    case Act_SaveAs:        a->setText(QObject::tr("另存为"));          break;
    case Act_Exit:          a->setText(QObject::tr("退出"));            break;
    case Act_QuickImport:   a->setText(QObject::tr("快速导入"));        break;
    case Act_Import:        a->setText(QObject::tr("导入"));            break;
    case Act_Export:        a->setText(QObject::tr("导出"));            break;
    case Act_CTReconstruct: a->setText(QObject::tr("CT 重建"));         break;
    case Act_Undo:          a->setText(QObject::tr("撤销"));            break;
    case Act_Redo:          a->setText(QObject::tr("重做"));            break;
    case Act_VolumeCrop:    a->setText(QObject::tr("体积裁剪"));        break;
    case Act_ResetView:     a->setText(QObject::tr("重置视图"));        break;
    case Act_FitAll:        a->setText(QObject::tr("适配全景"));        break;
    case Act_Preferences:   a->setText(QObject::tr("首选项"));          break;
    default:                a->setText(QObject::tr("未命名操作"));      break;
    }

    connect(a, &QAction::triggered, this, [this, id] {
        emit triggered(id);
    });
    map_[id] = a;
    return a;
}

void ActionManager::buildMenuBar(QMenuBar* bar, const QList<MenuSpec>& spec) {
    if (!bar) {
        return;
    }

    bar->clear();
    // ---- 依据提供的菜单规格构造菜单结构，并复用 action() 创建出的 QAction ----
    for (const auto& m : spec) {
        auto menu = bar->addMenu(m.menuTitle);
        for (int actionId : m.actionIDs) {
            menu->addAction(action(actionId));
        }
    }
}
