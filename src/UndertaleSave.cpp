#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include "UndertaleSave.hpp"
#include "nfd.hpp"
#include "nfd_sdl2.h"

using json = nlohmann::json;
namespace fs = std::filesystem;
std::string UndertaleSave::dir;
bool UndertaleSave::is_json = false;
nlohmann::json UndertaleSave::jsondata;

void UndertaleSave::Load(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], bool &is_xbox)
{
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdfilteritem_t filterItems[2] = {{"PC Save", "*"}, {"Console Save", "sav"}};
    nfdresult_t result = NFD::OpenDialog(outPath, filterItems, 2);
    if (result == NFD_OKAY)
    {
        fs::path filepath = outPath.get();
        std::string fileext = filepath.extension().string();
        is_json = (fileext == ".sav");

        if (!is_json)
        {
            dir = filepath.parent_path().string();
            std::vector<std::string> matchingFiles;

            for (const auto &entry : fs::directory_iterator(dir))
            {
                std::string filename = entry.path().filename().string();
                if ((filename.rfind("file0", 0) == 0) || (filename.rfind("file8", 0) == 0) || (filename.rfind("file9", 0) == 0))
                {
                    matchingFiles.push_back(filename);
                }
            }
            for (std::string file : matchingFiles)
            {
                fs::path path = fs::path(dir) / file;
                if (file == "file0")
                {
                    std::ifstream infile(path);
                    std::string line;
                    int num_lines;
                    if (!fs::exists(path))
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not find file0!", window);
                        return;
                    }
                    if (!infile.is_open())
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load file0!", window);
                        return;
                    }
                    while (getline(infile, line))
                        num_lines++;

                    is_xbox = (num_lines == 551);
                    infile.close();

                    if (FileToStruct(path.string(), &save[0], is_xbox) != 0)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load file0!", window);
                        return;
                    }
                }
                else if (file == "file9")
                {
                    if (FileToStruct(path.string(), &save[1], is_xbox) != 0)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load file9!", window);
                        return;
                    }
                }
                else if (file == "file8")
                {
                    if (FileToStruct(path.string(), &save[2], is_xbox) != 0)
                    {
                        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load file8!", window);
                        return;
                    }
                }
            }
        }
        else
        {
            dir = filepath.string();
            std::ifstream jsonfile(dir);        
            int num_lines;

            if (!jsonfile.is_open())
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not load undertale.sav!", window);
            }
            try
            {
                jsonfile >> jsondata;
                jsonfile.close();
                if (!jsondata.contains("file0"))
                {
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not find file0!", window);
                    return;
                }
                std::string file0 = jsondata["file0"].get<std::string>();
                std::stringstream buffer = ReplaceStringLiterals(file0);
                std::string line;
                
                while (std::getline(buffer, line))
                    num_lines++;
                
                is_xbox = (num_lines == 551);
                JSONToStruct(file0, &save[0], is_xbox);
                
                if (jsondata.contains("file9"))
                {
                    std::string file9 = jsondata["file9"].get<std::string>();
                    JSONToStruct(file9, &save[1], is_xbox);
                }

                if (jsondata.contains("file8"))
                {
                    std::string file9 = jsondata["file8"].get<std::string>();
                    JSONToStruct(file9, &save[2], is_xbox);
                }
            }
            catch (const std::exception& e)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to parse *.sav file!", window);
                jsonfile.close();
            }
        }
    }
}

void UndertaleSave::Save(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], bool is_xbox, bool save_as)
{
    std::string files[3] = {"file0", "file9", "file8"};
    fs::path filepath;

    if (save_as)
    {
        NFD::Guard nfdGuard;
        NFD::UniquePath outPath;
        nfdfilteritem_t filterItems[2] = {{"PC Save", "*"}, {"Console Save", "sav"}};
        nfdresult_t result = NFD::SaveDialog(outPath, filterItems, 2);
        if (result == NFD_OKAY)
        {
            filepath = outPath.get();
            std::string fileext = filepath.extension().string();
            is_json = (fileext == ".sav");
            if (!is_json)
                dir = filepath.parent_path().string();
            else
                dir = filepath.string();
        }
    }
    
    if (!is_json)
    {
        int i = 0;
        for (std::string file : files)
        {
            fs::path path = fs::path(dir) / file;
            if (StructToFile(path.string(), &save[i], is_xbox) != 0)
            {
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not save file!", window);
                return;
            }
            i++;
        }
    }
    else
    {
        std::ofstream outfile(dir);

        if (!outfile.is_open())
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could not save file!", window);
            return;
        }
        int i = 0;
        for (std::string file : files)
        {
            if (jsondata.contains(file))
            {
                std::stringstream buffer = StructToJSON(&save[i], is_xbox);
                jsondata[file] = buffer.str();
            }
            i++;
        }
        outfile << jsondata;
        outfile.close();
    }
}

int UndertaleSave::FileToStruct(std::string path, UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
{
    std::ifstream infile(path);
    if (!infile.is_open())
        return 1;

    infile >> save->name;
    infile >> save->lv;
    infile >> save->maxhp;
    infile >> save->maxen;
    infile >> save->at;
    infile >> save->wstrength;
    infile >> save->df;
    infile >> save->adef;
    infile >> save->sp;
    infile >> save->xp;
    infile >> save->gold;
    infile >> save->kills;
    for (int i = 0; i < 8; i++)
    {
        infile >> save->items[i];
        infile >> save->phone[i];
    }
    infile >> save->weapon;
    infile >> save->armor;
    for (int i = 0; i < 512; i++)
        infile >> save->flags[i];

    infile >> save->plot;
    for (int i = 0; i < 3; i++)
        infile >> save->menuchoice[i];
    infile >> save->currentsong;
    infile >> save->currentroom;
    infile >> save->time;
    if (is_xbox)
    {
        infile >> save->xbox_disconnect_counter;
        infile >> save->xbox_coins_donated;
    }
    infile.close();
    save->initialized = true;

    return 0;
}

int UndertaleSave::StructToFile(std::string path, UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
{
    if (save->initialized)
    {
        std::ofstream outfile(path);
        if (!outfile.is_open())
            return 1;

        outfile << save->name << std::endl;
        outfile << save->lv << std::endl;
        outfile << save->maxhp << std::endl;
        outfile << save->maxen << std::endl;
        outfile << save->at << std::endl;
        outfile << save->wstrength << std::endl;
        outfile << save->df << std::endl;
        outfile << save->adef << std::endl;
        outfile << save->sp << std::endl;
        outfile << save->xp << std::endl;
        outfile << save->gold << std::endl;
        outfile << save->kills << std::endl;
        for (int i = 0; i < 8; i++)
        {
            outfile << save->items[i] << std::endl;
            outfile << save->phone[i] << std::endl;
        }
        outfile << save->weapon << std::endl;
        outfile << save->armor << std::endl;
        for (int i = 0; i < 512; i++)
            outfile << save->flags[i] << std::endl;

        outfile << save->plot << std::endl;
        for (int i = 0; i < 3; i++)
            outfile << save->menuchoice[i] << std::endl;
        outfile << save->currentsong << std::endl;
        outfile << save->currentroom << std::endl;
        outfile << save->time << std::endl;
        if (is_xbox)
        {
            outfile << save->xbox_disconnect_counter << std::endl;
            outfile << save->xbox_coins_donated << std::endl;
        }
        outfile.close();
    }
    return 0;
}

int UndertaleSave::JSONToStruct(std::string data, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox)
{
    std::stringstream buffer = ReplaceStringLiterals(data);

    buffer >> save->name;
    buffer >> save->lv;
    buffer >> save->maxhp;
    buffer >> save->maxen;
    buffer >> save->at;
    buffer >> save->wstrength;
    buffer >> save->df;
    buffer >> save->adef;
    buffer >> save->sp;
    buffer >> save->xp;
    buffer >> save->gold;
    buffer >> save->kills;
    for (int i = 0; i < 8; i++)
    {
        buffer >> save->items[i];
        buffer >> save->phone[i];
    }
    buffer >> save->weapon;
    buffer >> save->armor;
    for (int i = 0; i < 512; i++)
        buffer >> save->flags[i];

    buffer >> save->plot;
    for (int i = 0; i < 3; i++)
        buffer >> save->menuchoice[i];
    buffer >> save->currentsong;
    buffer >> save->currentroom;
    buffer >> save->time;
    if (is_xbox)
    {
        buffer >> save->xbox_disconnect_counter;
        buffer >> save->xbox_coins_donated;
    }
    save->initialized = true;
    return 0;
}

std::stringstream UndertaleSave::StructToJSON(UndertaleCommon::UndertaleSaveFile * save, bool is_xbox)
{
    std::stringstream buffer;
    buffer << save->name << "\r\n";
    buffer << save->lv << "\r\n";
    buffer << save->maxhp << "\r\n";
    buffer << save->maxen << "\r\n";
    buffer << save->at << "\r\n";
    buffer << save->wstrength << "\r\n";
    buffer << save->df << "\r\n";
    buffer << save->adef << "\r\n";
    buffer << save->sp << "\r\n";
    buffer << save->xp << "\r\n";
    buffer << save->gold << "\r\n";
    buffer << save->kills << "\r\n";
    for (int i = 0; i < 8; i++)
    {
        buffer << save->items[i] << "\r\n";
        buffer << save->phone[i] << "\r\n";
    }
    buffer << save->weapon << "\r\n";
    buffer << save->armor << "\r\n";
    for (int i = 0; i < 512; i++)
        buffer << save->flags[i] << "\r\n";

    buffer << save->plot << "\r\n";
    for (int i = 0; i < 3; i++)
        buffer << save->menuchoice[i] << "\r\n";
    buffer << save->currentsong << "\r\n";
    buffer << save->currentroom << "\r\n";
    buffer << save->time << "\r\n";
    if (is_xbox)
    {
        buffer << save->xbox_disconnect_counter << "\r\n";
        buffer << save->xbox_coins_donated << "\r\n";
    }
    return buffer;
}

std::stringstream UndertaleSave::ReplaceStringLiterals(std::string str)
{
   size_t pos;
   while (pos = str.find("\\r\\n", pos) != std::string::npos)
   {
       str.replace(pos, 4, "\n");
       pos += 1;
   }
   std::stringstream buffer(str);
   return buffer;
}