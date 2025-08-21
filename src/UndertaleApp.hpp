#ifndef UNDERTALEAPP_HPP
#define UNDERTALEAPP_HPP

#include "SDL.h"
#include "SDL_image.h"
#include "imgui.h"
#include "UndertaleCommon.hpp"

#include "UndertaleGUI.hpp"
#include "UndertaleSave.hpp"

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
        std::unique_ptr<UndertaleSave> Save;
        std::unique_ptr<UndertaleGUI> GUI;
        bool is_running;
        bool is_xbox;
        bool show_editor;
};
#endif