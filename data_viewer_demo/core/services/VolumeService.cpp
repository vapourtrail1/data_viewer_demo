#include "core/services/VolumeService.h"

#include "core/common/Logging.h"
#include "core/io/VolumeIOServiceVtk.h"

namespace core::services {

VolumeService::VolumeService()
    : m_io(std::make_unique<core::io::VolumeIOServiceVtk>())
{
    // 默认使用 VTK 实现的 IO 服务，后续可替换为其他后端。
}

VolumeService::~VolumeService() = default;

core::common::Result<const core::data::VolumeModel*> VolumeService::openDicomDir(const QString& dir)
{
    // 委托 IO 服务读取数据，成功后更新当前项目模型。
    auto result = m_io->loadDicomDir(dir);
    if (result.ok()) {
        m_project.setVolume(result.value);
    }
    return { result.code, result.ok() ? &m_project.volume() : nullptr, result.message };
}

core::common::Result<const core::data::VolumeModel*> VolumeService::openNifti(const QString& file)
{
    auto result = m_io->loadNifti(file);
    if (result.ok()) {
        m_project.setVolume(result.value);
    }
    return { result.code, result.ok() ? &m_project.volume() : nullptr, result.message };
}

core::common::Result<const core::data::VolumeModel*> VolumeService::openRaw(const core::io::RawParam& param, const QString& file)
{
    auto result = m_io->loadRaw(param, file);
    if (result.ok()) {
        m_project.setVolume(result.value);
    }
    return { result.code, result.ok() ? &m_project.volume() : nullptr, result.message };
}

const core::data::VolumeModel* VolumeService::current() const
{
    // 返回当前项目持有的 VolumeModel，供渲染模块使用。
    return &m_project.volume();
}

} // namespace core::services
