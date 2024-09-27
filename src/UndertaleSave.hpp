#include "SDL.h"
#include "UndertaleCommon.hpp"
#include <vector>
#include "tinyfiledialogs.h"
#include <fstream>
#include <iostream>

class UndertaleSave
{
    public:
        static void Load(SDL_Window * window, UndertaleCommon::UndertaleSaveFile * save, bool * is_xbox);
        static void Save(SDL_Window * window, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox, bool save_as = false);
    private:
        static const char * path;
};