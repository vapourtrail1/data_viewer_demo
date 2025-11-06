#pragma once
#include <QString>

#include "core/common/error.h"
#include "core/data/volumeModel.h"
#include "core/io/rawParam.h"

namespace core::io {

    /**
     * @brief IVolumeIOService 定义体数据 IO 的抽象接口。
     */
    class IVolumeIOService
    {
    public:
        virtual ~IVolumeIOService() = default;

        virtual core::common::Result<core::data::VolumeModel> loadDicomDir(const QString& dir) = 0;
        virtual core::common::Result<core::data::VolumeModel> loadNifti(const QString& file) = 0;
        virtual core::common::Result<core::data::VolumeModel> loadRaw(const RawParam& param, const QString& file) = 0;
    };

} // namespace core::io