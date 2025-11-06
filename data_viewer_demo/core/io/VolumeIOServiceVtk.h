#pragma once

#include "core/io/IVolumeIOService.h"

class vtkImageData;

namespace core::io {

/**
 * @brief VolumeIOServiceVtk 使用 VTK 读取体数据文件。
 */
class VolumeIOServiceVtk : public IVolumeIOService
{
public:
    VolumeIOServiceVtk();
    ~VolumeIOServiceVtk() override;

    core::common::Result<core::data::VolumeModel> loadDicomDir(const QString& dir) override;
    core::common::Result<core::data::VolumeModel> loadNifti(const QString& file) override;
    core::common::Result<core::data::VolumeModel> loadRaw(const RawParam& param, const QString& file) override;

private:
    core::common::Result<core::data::VolumeModel> buildModelFromImage(vtkImageData* image, const QString& source);
};

} // namespace core::io
