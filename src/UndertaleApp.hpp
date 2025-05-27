#include "SDL.h"
#include "SDL_image.h"
#include "imgui.h"
#include "UndertaleCommon.hpp"

class UndertaleApp
{
    public:
        UndertaleApp();
        ~UndertaleApp();
        void Init();
        void Loop();
        void HandleEvents();
        void Render();
        void Destroy();
        bool IsRunning() { return is_running; };

    private:
        static SDL_Surface * LoadImageFromResource(const char * name);
        SDL_Window * window;
        SDL_Renderer * renderer;
        SDL_Surface * background;
        SDL_Texture * bgtex;
        ImGuiIO * io;
        UndertaleCommon::UndertaleSaveFile save[3];
        UndertaleCommon::UndertaleINI ini;
        UndertaleCommon::UndertaleConfigINI config;
        bool is_running;
        bool is_xbox;
        bool show_editor;
};