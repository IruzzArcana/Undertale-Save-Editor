#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include "UndertaleApp.hpp"
#include "UndertaleGUI.hpp"
#include "UndertaleSave.hpp"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "font.cpp"

UndertaleSave * Save = nullptr;
UndertaleGUI * GUI = nullptr;

UndertaleApp::UndertaleApp()
{
    Init();
}

UndertaleApp::~UndertaleApp()
{
    Destroy();
}

void UndertaleApp::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow(UndertaleCommon::title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        return;
    }

    background = LoadImageFromResource("bg");
    if (!background)
    {
        std::cout << "Failed to load image from resource" << std::endl;
        return;
    }
    bgtex = SDL_CreateTextureFromSurface(renderer, background);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->IniFilename = NULL;

    static ImVector<ImWchar> ranges;
    ImFontGlyphRangesBuilder builder;

    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesDefault());
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
    builder.AddRanges(ImGui::GetIO().Fonts->GetGlyphRangesJapanese());

    builder.BuildRanges(&ranges);

    io->Fonts->AddFontFromMemoryCompressedTTF(&font_compressed_data, font_compressed_size, 16, NULL, ranges.Data);
    
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);   
    Save = new UndertaleSave(window, UndertaleCommon::title, save, &ini, &config, is_xbox);
    GUI = new UndertaleGUI(save, &ini, &config, is_xbox);
    is_running = true;
}

void UndertaleApp::HandleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                is_running = false;
            break;
        default:
            break;
    }
    ImGui_ImplSDL2_ProcessEvent(&event);
}

void UndertaleApp::Render()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    GUI->DrawGUI(true);

    ImGui::EndFrame();
    ImGui::Render();

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgtex, NULL, NULL);
    SDL_RenderSetScale(renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    
    SDL_RenderPresent(renderer);
}

void UndertaleApp::Loop()
{
    GUI->ManageState();
    if (GUI->ShowFileDialog() > GUI->FILE_NONE)
    {
        switch (GUI->ShowFileDialog())
        {
            case UndertaleGUI::FILE_LOAD:
                Save->Load();
                break;
            case UndertaleGUI::FILE_LOAD_SAV:
                Save->ConsoleLoad();
                break;
            case UndertaleGUI::FILE_SAVE:
            case UndertaleGUI::FILE_SAVE_AS:
                Save->Save(GUI->ShowFileDialog() == UndertaleGUI::FILE_SAVE_AS);
                break;
            case UndertaleGUI::FILE_SAVE_AS_SAV:
                Save->ConsoleSave();
                break;
        }
        GUI->HideFileDialog();
    }
    SDL_SetWindowTitle(window, UndertaleCommon::title.c_str());

    if (GUI->Quit())
        is_running = false;
}

void UndertaleApp::Destroy()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    delete GUI;
    GUI = nullptr;
    delete Save;
    Save = nullptr;

    SDL_FreeSurface(background);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

SDL_Surface * UndertaleApp::LoadImageFromResource(const char * name)
{
    HRSRC resource = FindResource(NULL, name, _T("BACKGROUND"));
    if (!resource) {
        std::cout << "Could not find resource: " << name << std::endl;
        return NULL;
    }

    // Load the resource
    HGLOBAL resourceData = LoadResource(NULL, resource);
    if (!resourceData) {
        std::cout << "Could not load resource" << std::endl;
        return NULL;
    }

    // Get the size of the resource
    DWORD resourceSize = SizeofResource(NULL, resource);
    if (resourceSize == 0) {
        std::cout << "Resource is empty" << std::endl;
        return NULL;
    }

    // Get a pointer to the raw data
    void* resourceMemory = LockResource(resourceData);
    if (!resourceMemory) {
        std::cout << "Could not lock resource" << std::endl;
        return NULL;
    }

    // Create an SDL_RWops from the memory data
    SDL_RWops* rw = SDL_RWFromMem(resourceMemory, resourceSize);
    if (!rw) {
        std::cout << "Failed to create RWops" << std::endl;
        return NULL;
    }

    // Load the image using SDL_image
    SDL_Surface* image = IMG_Load_RW(rw, 1);
    if (!image) {
        std::cout << "Failed to load image from resource: " << IMG_GetError() << std::endl;
        return NULL;
    }

    return image;
}