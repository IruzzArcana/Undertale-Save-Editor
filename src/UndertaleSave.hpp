#define MINI_CASE_SENSITIVE

#include "SDL.h"
#include "UndertaleCommon.hpp"
#include "json.hpp"
#include "ini.h"

class UndertaleSave
{
    public:
        UndertaleSave(SDL_Window *window, std::string& title, UndertaleCommon::UndertaleSaveFile (&save)[3], UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool &is_xbox);
        void Load();
        void Save(bool save_as = false);
        void ConsoleLoad();
        void ConsoleSave();
    
    private:
        int FileToStruct(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        int StructToFile(std::string path, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        int JSONToStruct(std::string data, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        void INIFileToStruct(UndertaleCommon::UndertaleINI * ini);
        void StructToINIFile(UndertaleCommon::UndertaleINI * ini);
        void ConfigINIFileToStruct(UndertaleCommon::UndertaleConfigINI * config);
        void StructToConfigINIFile(UndertaleCommon::UndertaleConfigINI * config);

        void INIRead(mINI::INIStructure * ini, std::string * value, std::string section, std::string key);
        void INIRead(mINI::INIStructure * ini, bool * value, std::string section, std::string key);
        void INIRead(mINI::INIStructure * ini, int * value, std::string section, std::string key);
        void INIRead(mINI::INIStructure * ini, double * value, std::string section, std::string key);

        void INIWrite(mINI::INIStructure * ini, std::string * value, std::string section, std::string key);
        void INIWrite(mINI::INIStructure * ini, bool * value, std::string section, std::string key);
        void INIWrite(mINI::INIStructure * ini, int * value, std::string section, std::string key);
        void INIWrite(mINI::INIStructure * ini, double * value, std::string section, std::string key);

        std::stringstream StructToJSON(UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        std::stringstream ReplaceStringLiterals(std::string str);
        std::string dir;
        const char * path;
        bool is_json;
        nlohmann::json jsondata;
        mINI::INIStructure inidata;
        mINI::INIStructure configinidata;
        
        SDL_Window *window;
        std::string& title;
        UndertaleCommon::UndertaleSaveFile (&save)[3];
        UndertaleCommon::UndertaleINI *ini;
        UndertaleCommon::UndertaleConfigINI *config;
        bool &is_xbox;
};