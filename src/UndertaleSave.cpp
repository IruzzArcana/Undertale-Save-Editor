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
mINI::INIStructure UndertaleSave::inidata;

void UndertaleSave::Load(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI * ini, bool &is_xbox)
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
            fs::path ini_path = fs::path(dir) / "undertale.ini";
            if (fs::exists(ini_path))
            {
                mINI::INIFile file(ini_path.string());
                file.read(inidata);
                INIFileToStruct(ini, is_xbox);
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

int UndertaleSave::INIFileToStruct(UndertaleCommon::UndertaleINI * ini, bool is_xbox)
{
    INIRead(&ini->general.Name, "General", "Name");
    INIRead(&ini->general.Time, "General", "Time");
    INIRead(&ini->general.Room, "General", "Room");
    INIRead(&ini->general.Gameover, "General", "Gameover");
    INIRead(&ini->general.Kills, "General", "Kills");
    INIRead(&ini->general.Love, "General", "Love");
    INIRead(&ini->general.Tale, "General", "Tale");
    INIRead(&ini->general.Won, "General", "Won");
    INIRead(&ini->general.BW, "General", "BW");
    INIRead(&ini->general.BC, "General", "BC");
    INIRead(&ini->general.CP, "General", "CP");
    INIRead(&ini->general.BP, "General", "BP");
    INIRead(&ini->general.CH, "General", "CH");
    INIRead(&ini->general.BH, "General", "BH");
    INIRead(&ini->general.DB, "General", "DB");

    INIRead(&ini->reset.reset, "Reset", "reset");
    INIRead(&ini->reset.s_key, "Reset", "s_key");

    INIRead(&ini->flowey.Met1, "Flowey", "Met1");
    INIRead(&ini->flowey.K, "Flowey", "K");
    INIRead(&ini->flowey.NK, "Flowey", "NK");
    INIRead(&ini->flowey.IK, "Flowey", "IK");
    INIRead(&ini->flowey.SK, "Flowey", "SK");
    INIRead(&ini->flowey.FloweyExplain1, "Flowey", "FloweyExplain1");
    INIRead(&ini->flowey.EX, "Flowey", "EX");
    INIRead(&ini->flowey.CHANGE, "Flowey", "CHANGE");
    INIRead(&ini->flowey.AK, "Flowey", "AK");
    INIRead(&ini->flowey.AF, "Flowey", "AF");
    INIRead(&ini->flowey.Alter, "Flowey", "Alter");
    INIRead(&ini->flowey.alter2, "Flowey", "alter2");
    INIRead(&ini->flowey.truename, "Flowey", "truename");
    INIRead(&ini->flowey.SPECIALK, "Flowey", "SPECIALK");

    INIRead(&ini->toriel.Bscotch, "Toriel", "Bscotch");
    INIRead(&ini->toriel.TS, "Toriel", "TS");
    INIRead(&ini->toriel.TK, "Toriel", "TK");

    INIRead(&ini->sans.M1, "Sans", "M1");
    INIRead(&ini->sans.EndMet, "Sans", "EndMet");
    INIRead(&ini->sans.MeetLv1, "Sans", "MeetLv1");
    INIRead(&ini->sans.MeetLv2, "Sans", "MeetLv2");
    INIRead(&ini->sans.MeetLv, "Sans", "MeetLv");
    INIRead(&ini->sans.Pass, "Sans", "Pass");
    INIRead(&ini->sans.Intro, "Sans", "Intro");
    INIRead(&ini->sans.F, "Sans", "F");
    INIRead(&ini->sans.MP, "Sans", "MP");
    INIRead(&ini->sans.SK, "Sans", "SK");
    INIRead(&ini->sans.SS, "Sans", "SS");
    INIRead(&ini->sans.SS2, "Sans", "SS2");

    INIRead(&ini->papyrus.M1, "Papyrus", "M1");
    INIRead(&ini->papyrus.PS, "Papyrus", "PS");
    INIRead(&ini->papyrus.PD, "Papyrus", "PD");
    INIRead(&ini->papyrus.PK, "Papyrus", "PK");

    INIRead(&ini->fffff.F, "FFFFF", "F");
    INIRead(&ini->fffff.D, "FFFFF", "D");
    INIRead(&ini->fffff.P, "FFFFF", "P");
    INIRead(&ini->fffff.E, "FFFFF", "E");

    INIRead(&ini->undyne.UD, "Undyne", "UD");

    INIRead(&ini->mettaton.BossMet, "Mettaton", "BossMet");

    INIRead(&ini->mett.O, "Mett", "O");

    INIRead(&ini->mtt.EssayNo, "MTT", "EssayNo");

    INIRead(&ini->asgore.KillYou, "Asgore", "KillYou");

    INIRead(&ini->alphys.AD, "Alphys", "AD");

    INIRead(&ini->f7.F7, "F7", "F7");

    INIRead(&ini->endf.EndF, "EndF", "EndF");

    INIRead(&ini->dogshrine.Donated, "Dogshrine", "Donated");

    return 0;
}

void UndertaleSave::INIRead(std::string * value, std::string section, std::string key)
{
    if (inidata.has(section))
    {
        if (inidata.get(section).has(key))
        {
            *value = inidata[section][key].substr(1, inidata[section][key].size() - 2);
        }
    }
}

void UndertaleSave::INIRead(bool * value, std::string section, std::string key)
{
    if (inidata.has(section))
    {
        if (inidata.get(section).has(key))
        {
            std::string str = inidata[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stoi(str);
        }
    }
}

void UndertaleSave::INIRead(int * value, std::string section, std::string key)
{
    if (inidata.has(section))
    {
        if (inidata.get(section).has(key))
        {
            std::string str = inidata[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stoi(str);
        }
    }
}

void UndertaleSave::INIRead(double * value, std::string section, std::string key)
{
    if (inidata.has(section))
    {
        if (inidata.get(section).has(key))
        {
            std::string str = inidata[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stod(str);
        }
    }
}