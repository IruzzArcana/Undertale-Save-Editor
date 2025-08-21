#include "UndertaleApp.hpp"
#include <memory>

std::unique_ptr<UndertaleApp> app = nullptr;

int main(int argc, char **argv) 
{
    app = std::make_unique<UndertaleApp>();
    while (app->IsRunning())
    {
        app->HandleEvents();
        app->Loop();
        app->Render();
    }
    return 0;
}