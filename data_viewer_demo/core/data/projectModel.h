#pragma once
#include <QString>
#include "core/common/type.h"
#include "core/data/volumeModel.h"

namespace core::data {

    /**
     保存当前项目的体数据与显示参数。
     */
    class ProjectModel
    {
    public:
        ProjectModel();

        void setVolume(const VolumeModel& volume);
        const VolumeModel& volume() const;

        void setWindowLevel(core::common::WL wl);
        core::common::WL windowLevel() const;

        void setPresetName(const QString& name);
        QString presetName() const;

    private:
        VolumeModel m_volume;
        core::common::WL m_windowLevel{ core::common::DefaultWL };
        QString m_presetName;
    };

} // namespace core::data