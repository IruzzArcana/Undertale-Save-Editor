#include "UndertaleApp.hpp"

UndertaleApp * app = nullptr;

int main(int argc, char **argv) 
{
    app = new UndertaleApp();
    while (app->IsRunning())
    {
        app->HandleEvents();
        app->Loop();
        app->Render();
    }
    delete app;
    app = nullptr;
    return 0;
}