#include "UndertaleApp.hpp"

bool UndertaleApp::is_running = false;
bool UndertaleApp::is_xbox = false;
const char * UndertaleCommon::title = "Undertale Save Editor";
UndertaleCommon::UndertaleSaveFile UndertaleApp::save{};
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

    window = SDL_CreateWindow(UndertaleCommon::title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

    io = &ImGui::GetIO(); (void)*io;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io->IniFilename = NULL;
    
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);   

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
    
    UndertaleGUI::DrawMenuBar(true, UndertaleGUI::SHOW_UNDERTALE);
    UndertaleGUI::DrawAboutPage(UndertaleGUI::ShowAboutPage());
    UndertaleGUI::DrawSaveEditor(UndertaleGUI::ShowEditor(), &save, &is_xbox);
    
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
    if (UndertaleGUI::ShowFileDialog() > UndertaleGUI::FILE_NONE)
    {
        switch (UndertaleGUI::ShowFileDialog())
        {
            case UndertaleGUI::FILE_LOAD:
                UndertaleSave::Load(window, &save, &is_xbox);
                break;
            case UndertaleGUI::FILE_SAVE:
                UndertaleSave::Save(window, &save, is_xbox);
                break;
            case UndertaleGUI::FILE_SAVE_AS:
                UndertaleSave::Save(window, &save, is_xbox, true);
                break;
        }
        UndertaleGUI::HideFileDialog();
    }

    if (UndertaleGUI::Quit())
    {
        is_running = false;
    }
}

void UndertaleApp::Destroy()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

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