#pragma once
#include <memory>
#include <QString>
#include "core/common/error.h"
#include "core/data/projectModel.h"
#include "core/io/iVolumeIOService.h"

namespace core::services {

    /**
    VolumeService 封装体数据的打开与访问。
    */
    class VolumeService
    {
    public:
        VolumeService();
        ~VolumeService();

        core::common::Result<const core::data::VolumeModel*> openDicomDir(const QString& dir);
        core::common::Result<const core::data::VolumeModel*> openNifti(const QString& file);
        core::common::Result<const core::data::VolumeModel*> openRaw(const core::io::RawParam& param, const QString& file);

        const core::data::VolumeModel* current() const;

    private:
        core::data::ProjectModel m_project;
        std::unique_ptr<core::io::IVolumeIOService> m_io;
    };

} // namespace core::services