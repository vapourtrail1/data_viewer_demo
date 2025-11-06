#include "core/data/ProjectModel.h"

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
