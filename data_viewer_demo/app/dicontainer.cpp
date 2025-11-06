#include "app/dicontainer.h"
#include "core/render/renderService.h"
#include "core/services/OrthogonalMprService.h"
#include "core/services/VolumeService.h"

namespace app {

    DIContainer& DIContainer::instance()
    {
        static DIContainer g_instance;
        return g_instance;
    }

    core::services::VolumeService* DIContainer::volume()
    {
        return m_volumeService.get();
    }

    core::services::OrthogonalMprService* DIContainer::mpr()
    {
		return m_mprService.get();
    }

    core::render::RenderService* DIContainer::render()
    {
        return m_renderService.get();
    }

    DIContainer::DIContainer()
        : m_volumeService(std::make_unique<core::services::VolumeService>())
        , m_mprService(std::make_unique<core::services::OrthogonalMprService>())
        , m_renderService(std::make_unique<core::render::RenderService>())
    {
        // 构造函数中完成依赖的集中创建，方便全局获取。
    }

} // namespace app