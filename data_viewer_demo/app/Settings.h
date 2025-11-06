#pragma once

#include <optional>
#include <QStringList>

#include "core/common/Types.h"
#include "core/io/RawParam.h"

namespace app {

/**
 * @brief Settings 管理应用级持久化设置（最近文件、默认窗宽窗位等）。
 */
class Settings
{
public:
    Settings();

    /**
     * @brief 从磁盘读取设置。
     */
    void load();

    /**
     * @brief 将当前设置写入磁盘。
     */
    void save() const;

    /**
     * @brief 最近打开的体数据路径列表。
     */
    const QStringList& recentFiles() const;

    /**
     * @brief 添加最近文件（会去重并限制数量）。
     */
    void addRecentFile(const QString& path);

    /**
     * @brief 默认窗宽/窗位。
     */
    core::common::WL defaultWL() const;

    /**
     * @brief 更新默认窗宽/窗位。
     */
    void setDefaultWL(core::common::WL wl);

    /**
     * @brief 最近一次 RAW 参数（若存在）。
     */
    std::optional<core::io::RawParam> lastRawParam() const;

    /**
     * @brief 保存新的 RAW 参数。
     */
    void setLastRawParam(const core::io::RawParam& param);

private:
    QStringList m_recentFiles;
    core::common::WL m_defaultWL;
    std::optional<core::io::RawParam> m_lastRaw;
};

} // namespace app
