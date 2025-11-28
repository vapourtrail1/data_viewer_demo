#pragma once
#include <optional>
#include <QStringList>

#include "core/common/type.h"
#include "core/io/rawParam.h"

namespace app {
    /**
     管理应用级持久化设置（最近文件、默认窗宽窗位等）
     */
    class Settings
    {
    public:
        Settings();

        //从磁盘读取设置
        void load();

        //将当前设置写入磁盘
        void save() const;

        //最近打开的体数据路径列表。
        const QStringList& recentFiles() const;

        //添加最近文件
        void addRecentFile(const QString& path);

        //默认窗宽/窗位
        core::common::WL defaultWL() const;

        //更新默认窗宽/窗位
        void setDefaultWL(core::common::WL wl);
       
        //最近一次 RAW 参数
        std::optional<core::io::RawParam> lastRawParam() const;

        //保存新的 RAW 参数
        void setLastRawParam(const core::io::RawParam& param);

    private:
        QStringList m_recentFiles;
        core::common::WL m_defaultWL;
        std::optional<core::io::RawParam> m_lastRaw;
    };
} // namespace app