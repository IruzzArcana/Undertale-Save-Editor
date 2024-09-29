#include "SDL.h"
#include "UndertaleCommon.hpp"

class UndertaleSave
{
    public:
        static void Load(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], bool &is_xbox);
        static void Save(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], bool is_xbox, bool save_as = false);
    
    private:
        static int FileToStruct(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static int StructToFile(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static std::string dir;
        static const char * path;
        static bool is_json;
};