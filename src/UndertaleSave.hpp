#define MINI_CASE_SENSITIVE

#include "SDL.h"
#include "UndertaleCommon.hpp"
#include "json.hpp"
#include "ini.h"

class UndertaleSave
{
    public:
        static void Load(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI * ini, bool &is_xbox);
        static void Save(SDL_Window * window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI * ini, bool is_xbox, bool save_as = false);
    
    private:
        static int FileToStruct(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static int StructToFile(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static int JSONToStruct(std::string data, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static void INIFileToStruct(UndertaleCommon::UndertaleINI * ini);
        static void StructToINIFile(UndertaleCommon::UndertaleINI * ini);

        static void INIRead(std::string * value, std::string section, std::string key);
        static void INIRead(bool * value, std::string section, std::string key);
        static void INIRead(int * value, std::string section, std::string key);
        static void INIRead(double * value, std::string section, std::string key);

        static void INIWrite(std::string * value, std::string section, std::string key);
        static void INIWrite(bool * value, std::string section, std::string key);
        static void INIWrite(int * value, std::string section, std::string key);
        static void INIWrite(double * value, std::string section, std::string key);

        static std::stringstream StructToJSON(UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static std::stringstream ReplaceStringLiterals(std::string str);
        static std::string dir;
        static const char * path;
        static bool is_json;
        static nlohmann::json jsondata;
        static mINI::INIStructure inidata;
};