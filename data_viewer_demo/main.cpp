#include "app/App.h"

int main(int argc, char* argv[])
{
    // 将应用入口委托给 app::App，集中处理初始化逻辑
    app::App application;
    return application.run(argc, argv);
}