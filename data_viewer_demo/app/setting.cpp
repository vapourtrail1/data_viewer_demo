#include "app/setting.h"
#include <QSettings>
#include <QVariant>
#include <QVariantMap>


namespace {
    constexpr int kMaxRecentFiles = 8;
    const char kOrgName[] = "CTViewer";
    const char kAppName[] = "CTViewer";
    const char kRecentFilesKey[] = "recentFiles";
    const char kWindowWidthKey[] = "windowWidth";
    const char kWindowLevelKey[] = "windowLevel";
    const char kRawParamKey[] = "rawParam";
}

namespace app {

    Settings::Settings()
        : m_defaultWL(core::common::DefaultWL)
    {
    }

    void Settings::load()
    {
        // 使用 Qt 自带的 QSettings 读取 ini/注册表配置。
        QSettings settings(QString::fromUtf8(kOrgName),QString::fromUtf8(kAppName));//QSettings的构造函数会根据平台选择合适的存储方式
        m_recentFiles = settings.value(QString::fromUtf8(kRecentFilesKey)).toStringList();
        
        const double window = settings.value(QString::fromUtf8(kWindowWidthKey), m_defaultWL.first).toDouble();
        const double level = settings.value(QString::fromUtf8(kWindowLevelKey), m_defaultWL.second).toDouble();
        m_defaultWL = { window, level };

        const QVariant rawVariant = settings.value(QString::fromUtf8(kRawParamKey));
        if (rawVariant.canConvert<QVariantMap>()) {
            const QVariantMap map = rawVariant.toMap();
            core::io::RawParam param;
            param.dimX = map.value(QStringLiteral("dimX"), 0).toInt();
            param.dimY = map.value(QStringLiteral("dimY"), 0).toInt();
            param.dimZ = map.value(QStringLiteral("dimZ"), 0).toInt();
            param.bytesPerPixel = map.value(QStringLiteral("bytesPerPixel"), 1).toInt();
            param.littleEndian = map.value(QStringLiteral("littleEndian"), true).toBool();
            param.isSigned = map.value(QStringLiteral("isSigned"), false).toBool();
            if (param.dimX > 0 && param.dimY > 0 && param.dimZ > 0) {
                m_lastRaw = param;
            }
        }
    }

    void Settings::save() const
    {
        QSettings settings(QString::fromUtf8(kOrgName), QString::fromUtf8(kAppName));
        settings.setValue(QString::fromUtf8(kRecentFilesKey), m_recentFiles);
        settings.setValue(QString::fromUtf8(kWindowWidthKey), m_defaultWL.first);
        settings.setValue(QString::fromUtf8(kWindowLevelKey), m_defaultWL.second);

        if (m_lastRaw.has_value()) {
            QVariantMap map;
            map.insert(QStringLiteral("dimX"), m_lastRaw->dimX);
            map.insert(QStringLiteral("dimY"), m_lastRaw->dimY);
            map.insert(QStringLiteral("dimZ"), m_lastRaw->dimZ);
            map.insert(QStringLiteral("bytesPerPixel"), m_lastRaw->bytesPerPixel);
            map.insert(QStringLiteral("littleEndian"), m_lastRaw->littleEndian);
            map.insert(QStringLiteral("isSigned"), m_lastRaw->isSigned);
            settings.setValue(QString::fromUtf8(kRawParamKey), map);
        }
    }

    const QStringList& Settings::recentFiles() const
    {
        return m_recentFiles;
    }

    void Settings::addRecentFile(const QString& path)
    {
        if (path.isEmpty()) {
            return;
        }

        // 去重后放到首位，保证最近文件列表有序。
        m_recentFiles.removeAll(path);
        m_recentFiles.prepend(path);

        while (m_recentFiles.size() > kMaxRecentFiles) {
            m_recentFiles.removeLast();
        }
    }

    core::common::WL Settings::defaultWL() const
    {
        return m_defaultWL;
    }

    void Settings::setDefaultWL(core::common::WL wl)
    {
        m_defaultWL = wl;
    }

    std::optional<core::io::RawParam> Settings::lastRawParam() const
    {
        return m_lastRaw;
    }

    void Settings::setLastRawParam(const core::io::RawParam& param)
    {
        m_lastRaw = param;
    }

} // namespace app