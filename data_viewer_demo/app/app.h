#pragma once
#include <memory>

namespace app {
    class App
    {
    public:
        App();
        int run(int argc, char** argv);
    private:
        /**
        初始化 Qt 的日志分类配置
         */
        void initializeLogging() const;

        /**
        应用全局样式和字体配置
         */
        void applyGlobalStyle() const;
    };

} // namespace app