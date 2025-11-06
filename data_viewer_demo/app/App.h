#pragma once

#include <memory>

namespace app {

/**
 * @brief App 封装了应用的生命周期管理（初始化、样式、日志等）。
 */
class App
{
public:
    App();

    /**
     * @brief 启动 Qt 应用主循环。
     */
    int run(int argc, char** argv);

private:
    /**
     * @brief 初始化 Qt 的日志分类配置。
     */
    void initializeLogging() const;

    /**
     * @brief 应用全局样式和字体配置。
     */
    void applyGlobalStyle() const;
};

} // namespace app
