#include "SDL.h"
#include "SDL_image.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include "UndertaleGUI.hpp"
#include "UndertaleSave.hpp"
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
        static UndertaleCommon::UndertaleSaveFile save;
        static bool is_running;
        static bool is_xbox;
};