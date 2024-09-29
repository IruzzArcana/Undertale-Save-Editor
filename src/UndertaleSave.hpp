#include "SDL.h"
#include "UndertaleCommon.hpp"
#include <vector>
#include "nfd.hpp"
#include "nfd_sdl2.h"
#include <filesystem>
#include <fstream>
#include <iostream>

class UndertaleSave
{
    public:
        static void Load(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], bool * is_xbox);
        static void Save(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], bool is_xbox, bool save_as = false);
    private:
        static int FileToStruct(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static int StructToFile(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static std::string dir;
        static const char * path;
};