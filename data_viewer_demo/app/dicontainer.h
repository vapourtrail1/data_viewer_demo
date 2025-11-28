#pragma once
#include <memory>

namespace core::services {
    class VolumeService;
    class OrthogonalMprService;
}

namespace core::render {
    class RenderService;
}

namespace app {

    /**
    负责集中构造并持有核心服务的单例。
     */
    class DIContainer
    {
    public:
        static DIContainer& instance();

        core::services::VolumeService* volume();
        core::services::OrthogonalMprService* mpr();
        core::render::RenderService* render();

    private:
        DIContainer();

        std::unique_ptr<core::services::VolumeService> m_volumeService;
        std::unique_ptr<core::services::OrthogonalMprService> m_mprService;
        std::unique_ptr<core::render::RenderService> m_renderService;
    };
} // namespace app