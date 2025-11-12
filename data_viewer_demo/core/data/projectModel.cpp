#include "core/data/projectModel.h"

/*
    本页描述一个"工程"的显示状态
    包括volumeModel
	包括WL(怎么显示) presentName(用什么预设显示)
*/
namespace core::data {

    ProjectModel::ProjectModel() = default;

    void ProjectModel::setVolume(const VolumeModel& volume)
    {
        m_volume = volume;
    }

    const VolumeModel& ProjectModel::volume() const
    {
        return m_volume;
    }

    void ProjectModel::setWindowLevel(core::common::WL wl)
    {
        m_windowLevel = wl;
    }

    core::common::WL ProjectModel::windowLevel() const
    {
        return m_windowLevel;
    }

    void ProjectModel::setPresetName(const QString& name)
    {
        m_presetName = name;
    }

    QString ProjectModel::presetName() const
    {
        return m_presetName;
    }

} // namespace core::data