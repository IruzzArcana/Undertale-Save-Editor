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
mINI::INIStructure UndertaleSave::configinidata;

void UndertaleSave::Load(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool &is_xbox)
{
    int num_lines = 0;
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdresult_t result = NFD::PickFolder(outPath);
    if (result == NFD_OKAY)
    {
        inidata.clear();
        configinidata.clear();
        fs::path filepath = outPath.get();

        dir = filepath.string();
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
            INIFileToStruct(ini);
        }

        fs::path config_path = fs::path(dir) / "config.ini";
        if (fs::exists(config_path))
        {
            mINI::INIFile configfile(config_path.string());
            configfile.read(configinidata);
            ConfigINIFileToStruct(config);
        }
    }
}

void UndertaleSave::ConsoleLoad(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool &is_xbox)
{
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdfilteritem_t filterItems[1] = {{"Console Save", "sav"}};
    nfdresult_t result = NFD::OpenDialog(outPath, filterItems, 1);
    if (result == NFD_OKAY)
    {
        inidata.clear();
        configinidata.clear();
        fs::path filepath = outPath.get();
        std::string fileext = filepath.extension().string();
        is_json = true;

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

            if (jsondata.contains("undertale.ini"))
            {
                std::stringstream inibuffer = ReplaceStringLiterals(jsondata["undertale.ini"]);
                mINI::INIFile file(&inibuffer);
                file.readbuffer(inidata);
                INIFileToStruct(ini);
            }

            if (jsondata.contains("config.ini"))
            {
                std::stringstream configinibuffer = ReplaceStringLiterals(jsondata["config.ini"]);
                mINI::INIFile configfile(&configinibuffer);
                configfile.readbuffer(configinidata);
                ConfigINIFileToStruct(config);
            }
        }
        catch (const std::exception &e)
        {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Failed to parse *.sav file!", window);
            jsonfile.close();
        }
    }
}

void UndertaleSave::Save(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool is_xbox, bool save_as)
{
    std::string files[3] = {"file0", "file9", "file8"};
    fs::path filepath;

    if (save_as)
    {
        NFD::Guard nfdGuard;
        NFD::UniquePath outPath;
        nfdresult_t result = NFD::PickFolder(outPath);
        if (result == NFD_OKAY)
        {
            filepath = outPath.get();
            std::string fileext = filepath.extension().string();
            is_json = false;
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

        fs::path ini_path = fs::path(dir) / "undertale.ini";
        if (ini->initialized)
        {
            mINI::INIFile file(ini_path.string());
            StructToINIFile(ini);
            file.write(inidata);
        }

        fs::path config_path = fs::path(dir) / "config.ini";
        if (config->initialized)
        {
            mINI::INIFile configfile(config_path.string());
            StructToConfigINIFile(config);
            configfile.write(configinidata);
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
            if (save[i].initialized)
            {
                std::stringstream buffer = StructToJSON(&save[i], is_xbox);
                jsondata[file] = buffer.str();
            }
            i++;
        }

        if (ini->initialized)
        {
            StructToINIFile(ini);
            std::stringstream ini_buffer;
            mINI::INIFile file(&ini_buffer);
            file.writebuffer(inidata);
            jsondata["undertale.ini"] = ini_buffer.str();
        }

        if (config->initialized)
        {
            StructToINIFile(ini);
            std::stringstream configini_buffer;
            mINI::INIFile file(&configini_buffer);
            file.writebuffer(configinidata);
            jsondata["config.ini"] = configini_buffer.str();
        }

        outfile << jsondata;
        outfile.close();
    }
}

void UndertaleSave::ConsoleSave(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool is_xbox)
{
    std::string files[3] = {"file0", "file9", "file8"};
    fs::path filepath;

    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdfilteritem_t filterItems[1] = {{"Console Save", "sav"}};
    nfdresult_t result = NFD::SaveDialog(outPath, filterItems, 1);
    if (result == NFD_OKAY)
    {
        filepath = outPath.get();
        std::string fileext = filepath.extension().string();
        is_json = true;
        dir = filepath.string();
        Save(window, save, ini, config, is_xbox, false);
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

int UndertaleSave::JSONToStruct(std::string data, UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
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

std::stringstream UndertaleSave::StructToJSON(UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
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

void UndertaleSave::INIFileToStruct(UndertaleCommon::UndertaleINI *ini)
{
    INIRead(&inidata, &ini->general.Name, "General", "Name");
    INIRead(&inidata, &ini->general.Time, "General", "Time");
    INIRead(&inidata, &ini->general.Room, "General", "Room");
    INIRead(&inidata, &ini->general.Gameover, "General", "Gameover");
    INIRead(&inidata, &ini->general.Kills, "General", "Kills");
    INIRead(&inidata, &ini->general.Love, "General", "Love");
    INIRead(&inidata, &ini->general.Tale, "General", "Tale");
    INIRead(&inidata, &ini->general.Won, "General", "Won");
    INIRead(&inidata, &ini->general.BW, "General", "BW");
    INIRead(&inidata, &ini->general.BC, "General", "BC");
    INIRead(&inidata, &ini->general.CP, "General", "CP");
    INIRead(&inidata, &ini->general.BP, "General", "BP");
    INIRead(&inidata, &ini->general.CH, "General", "CH");
    INIRead(&inidata, &ini->general.BH, "General", "BH");
    INIRead(&inidata, &ini->general.DB, "General", "DB");

    INIRead(&inidata, &ini->reset.reset, "Reset", "reset");
    INIRead(&inidata, &ini->reset.s_key, "Reset", "s_key");

    INIRead(&inidata, &ini->flowey.Met1, "Flowey", "Met1");
    INIRead(&inidata, &ini->flowey.K, "Flowey", "K");
    INIRead(&inidata, &ini->flowey.NK, "Flowey", "NK");
    INIRead(&inidata, &ini->flowey.IK, "Flowey", "IK");
    INIRead(&inidata, &ini->flowey.SK, "Flowey", "SK");
    INIRead(&inidata, &ini->flowey.FloweyExplain1, "Flowey", "FloweyExplain1");
    INIRead(&inidata, &ini->flowey.EX, "Flowey", "EX");
    INIRead(&inidata, &ini->flowey.CHANGE, "Flowey", "CHANGE");
    INIRead(&inidata, &ini->flowey.AK, "Flowey", "AK");
    INIRead(&inidata, &ini->flowey.AF, "Flowey", "AF");
    INIRead(&inidata, &ini->flowey.Alter, "Flowey", "Alter");
    INIRead(&inidata, &ini->flowey.alter2, "Flowey", "alter2");
    INIRead(&inidata, &ini->flowey.truename, "Flowey", "truename");
    INIRead(&inidata, &ini->flowey.SPECIALK, "Flowey", "SPECIALK");

    INIRead(&inidata, &ini->toriel.Bscotch, "Toriel", "Bscotch");
    INIRead(&inidata, &ini->toriel.TS, "Toriel", "TS");
    INIRead(&inidata, &ini->toriel.TK, "Toriel", "TK");

    INIRead(&inidata, &ini->sans.M1, "Sans", "M1");
    INIRead(&inidata, &ini->sans.EndMet, "Sans", "EndMet");
    INIRead(&inidata, &ini->sans.MeetLv1, "Sans", "MeetLv1");
    INIRead(&inidata, &ini->sans.MeetLv2, "Sans", "MeetLv2");
    INIRead(&inidata, &ini->sans.MeetLv, "Sans", "MeetLv");
    INIRead(&inidata, &ini->sans.Pass, "Sans", "Pass");
    INIRead(&inidata, &ini->sans.Intro, "Sans", "Intro");
    INIRead(&inidata, &ini->sans.F, "Sans", "F");
    INIRead(&inidata, &ini->sans.MP, "Sans", "MP");
    INIRead(&inidata, &ini->sans.SK, "Sans", "SK");
    INIRead(&inidata, &ini->sans.SS, "Sans", "SS");
    INIRead(&inidata, &ini->sans.SS2, "Sans", "SS2");

    INIRead(&inidata, &ini->papyrus.M1, "Papyrus", "M1");
    INIRead(&inidata, &ini->papyrus.PS, "Papyrus", "PS");
    INIRead(&inidata, &ini->papyrus.PD, "Papyrus", "PD");
    INIRead(&inidata, &ini->papyrus.PK, "Papyrus", "PK");

    INIRead(&inidata, &ini->fffff.F, "FFFFF", "F");
    INIRead(&inidata, &ini->fffff.D, "FFFFF", "D");
    INIRead(&inidata, &ini->fffff.P, "FFFFF", "P");
    INIRead(&inidata, &ini->fffff.E, "FFFFF", "E");

    INIRead(&inidata, &ini->undyne.UD, "Undyne", "UD");

    INIRead(&inidata, &ini->mettaton.BossMet, "Mettaton", "BossMet");

    INIRead(&inidata, &ini->mett.O, "Mett", "O");

    INIRead(&inidata, &ini->mtt.EssayNo, "MTT", "EssayNo");

    INIRead(&inidata, &ini->asgore.KillYou, "Asgore", "KillYou");

    INIRead(&inidata, &ini->alphys.AD, "Alphys", "AD");
    INIRead(&inidata, &ini->alphys.M, "Alphys", "M");
    INIRead(&inidata, &ini->alphys.R, "Alphys", "R");

    INIRead(&inidata, &ini->f7.F7, "F7", "F7");

    INIRead(&inidata, &ini->endf.EndF, "EndF", "EndF");

    INIRead(&inidata, &ini->dogshrine.Donated, "Dogshrine", "Donated");

    ini->initialized = true;
}

void UndertaleSave::StructToINIFile(UndertaleCommon::UndertaleINI *ini)
{
    INIWrite(&inidata, &ini->general.Name, "General", "Name");
    INIWrite(&inidata, &ini->general.Time, "General", "Time");
    INIWrite(&inidata, &ini->general.Room, "General", "Room");
    INIWrite(&inidata, &ini->general.Gameover, "General", "Gameover");
    INIWrite(&inidata, &ini->general.Kills, "General", "Kills");
    INIWrite(&inidata, &ini->general.Love, "General", "Love");
    INIWrite(&inidata, &ini->general.Tale, "General", "Tale");
    INIWrite(&inidata, &ini->general.Won, "General", "Won");
    INIWrite(&inidata, &ini->general.BW, "General", "BW");
    INIWrite(&inidata, &ini->general.BC, "General", "BC");
    INIWrite(&inidata, &ini->general.CP, "General", "CP");
    INIWrite(&inidata, &ini->general.BP, "General", "BP");
    INIWrite(&inidata, &ini->general.CH, "General", "CH");
    INIWrite(&inidata, &ini->general.BH, "General", "BH");
    INIWrite(&inidata, &ini->general.DB, "General", "DB");

    INIWrite(&inidata, &ini->reset.reset, "Reset", "reset");
    INIWrite(&inidata, &ini->reset.s_key, "Reset", "s_key");

    INIWrite(&inidata, &ini->flowey.Met1, "Flowey", "Met1");
    INIWrite(&inidata, &ini->flowey.K, "Flowey", "K");
    INIWrite(&inidata, &ini->flowey.NK, "Flowey", "NK");
    INIWrite(&inidata, &ini->flowey.IK, "Flowey", "IK");
    INIWrite(&inidata, &ini->flowey.SK, "Flowey", "SK");
    INIWrite(&inidata, &ini->flowey.FloweyExplain1, "Flowey", "FloweyExplain1");
    INIWrite(&inidata, &ini->flowey.EX, "Flowey", "EX");
    INIWrite(&inidata, &ini->flowey.CHANGE, "Flowey", "CHANGE");
    INIWrite(&inidata, &ini->flowey.AK, "Flowey", "AK");
    INIWrite(&inidata, &ini->flowey.AF, "Flowey", "AF");
    INIWrite(&inidata, &ini->flowey.Alter, "Flowey", "Alter");
    INIWrite(&inidata, &ini->flowey.alter2, "Flowey", "alter2");
    INIWrite(&inidata, &ini->flowey.truename, "Flowey", "truename");
    INIWrite(&inidata, &ini->flowey.SPECIALK, "Flowey", "SPECIALK");

    INIWrite(&inidata, &ini->toriel.Bscotch, "Toriel", "Bscotch");
    INIWrite(&inidata, &ini->toriel.TS, "Toriel", "TS");
    INIWrite(&inidata, &ini->toriel.TK, "Toriel", "TK");

    INIWrite(&inidata, &ini->sans.M1, "Sans", "M1");
    INIWrite(&inidata, &ini->sans.EndMet, "Sans", "EndMet");
    INIWrite(&inidata, &ini->sans.MeetLv1, "Sans", "MeetLv1");
    INIWrite(&inidata, &ini->sans.MeetLv2, "Sans", "MeetLv2");
    INIWrite(&inidata, &ini->sans.MeetLv, "Sans", "MeetLv");
    INIWrite(&inidata, &ini->sans.Pass, "Sans", "Pass");
    INIWrite(&inidata, &ini->sans.Intro, "Sans", "Intro");
    INIWrite(&inidata, &ini->sans.F, "Sans", "F");
    INIWrite(&inidata, &ini->sans.MP, "Sans", "MP");
    INIWrite(&inidata, &ini->sans.SK, "Sans", "SK");
    INIWrite(&inidata, &ini->sans.SS, "Sans", "SS");
    INIWrite(&inidata, &ini->sans.SS2, "Sans", "SS2");

    INIWrite(&inidata, &ini->papyrus.M1, "Papyrus", "M1");
    INIWrite(&inidata, &ini->papyrus.PS, "Papyrus", "PS");
    INIWrite(&inidata, &ini->papyrus.PD, "Papyrus", "PD");
    INIWrite(&inidata, &ini->papyrus.PK, "Papyrus", "PK");

    INIWrite(&inidata, &ini->fffff.F, "FFFFF", "F");
    INIWrite(&inidata, &ini->fffff.D, "FFFFF", "D");
    INIWrite(&inidata, &ini->fffff.P, "FFFFF", "P");
    INIWrite(&inidata, &ini->fffff.E, "FFFFF", "E");

    INIWrite(&inidata, &ini->undyne.UD, "Undyne", "UD");

    INIWrite(&inidata, &ini->mettaton.BossMet, "Mettaton", "BossMet");

    INIWrite(&inidata, &ini->mett.O, "Mett", "O");

    INIWrite(&inidata, &ini->mtt.EssayNo, "MTT", "EssayNo");

    INIWrite(&inidata, &ini->asgore.KillYou, "Asgore", "KillYou");

    INIWrite(&inidata, &ini->alphys.AD, "Alphys", "AD");
    INIWrite(&inidata, &ini->alphys.M, "Alphys", "M");
    INIWrite(&inidata, &ini->alphys.R, "Alphys", "R");

    INIWrite(&inidata, &ini->f7.F7, "F7", "F7");

    INIWrite(&inidata, &ini->endf.EndF, "EndF", "EndF");

    INIWrite(&inidata, &ini->dogshrine.Donated, "Dogshrine", "Donated");
}

void UndertaleSave::ConfigINIFileToStruct(UndertaleCommon::UndertaleConfigINI *config)
{
    INIRead(&configinidata, &config->general.lang, "General", "lang");
    INIRead(&configinidata, &config->general.sb, "General", "sb");

    INIRead(&configinidata, &config->joypad1.b0, "joypad1", "b0");
    INIRead(&configinidata, &config->joypad1.b1, "joypad1", "b1");
    INIRead(&configinidata, &config->joypad1.b2, "joypad1", "b2");
    INIRead(&configinidata, &config->joypad1.as, "joypad1", "as");
    INIRead(&configinidata, &config->joypad1.jd, "joypad1", "jd");

    config->initialized = true;
}

void UndertaleSave::StructToConfigINIFile(UndertaleCommon::UndertaleConfigINI *config)
{
    INIWrite(&configinidata, &config->general.lang, "General", "lang");
    INIWrite(&configinidata, &config->general.sb, "General", "sb");

    INIWrite(&configinidata, &config->joypad1.b0, "joypad1", "b0");
    INIWrite(&configinidata, &config->joypad1.b1, "joypad1", "b1");
    INIWrite(&configinidata, &config->joypad1.b2, "joypad1", "b2");
    INIWrite(&configinidata, &config->joypad1.as, "joypad1", "as");
    INIWrite(&configinidata, &config->joypad1.jd, "joypad1", "jd");
}

void UndertaleSave::INIRead(mINI::INIStructure *ini, std::string *value, std::string section, std::string key)
{
    *value = "";
    if (ini->has(section))
    {
        if (ini->get(section).has(key))
        {
            *value = (*ini)[section][key].substr(1, (*ini)[section][key].size() - 2);
        }
    }
}

void UndertaleSave::INIRead(mINI::INIStructure *ini, bool *value, std::string section, std::string key)
{
    *value = 0;
    if (ini->has(section))
    {
        if (ini->get(section).has(key))
        {
            std::string str = (*ini)[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stoi(str);
        }
    }
}

void UndertaleSave::INIRead(mINI::INIStructure *ini, int *value, std::string section, std::string key)
{
    *value = 0;
    if (ini->has(section))
    {
        if (ini->get(section).has(key))
        {
            std::string str = (*ini)[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stoi(str);
        }
    }
}

void UndertaleSave::INIRead(mINI::INIStructure *ini, double *value, std::string section, std::string key)
{
    *value = 0;
    if (ini->has(section))
    {
        if (ini->get(section).has(key))
        {
            std::string str = (*ini)[section][key];
            str = str.substr(1, str.size() - 2);
            *value = std::stod(str);
        }
    }
}

void UndertaleSave::INIWrite(mINI::INIStructure *ini, std::string *value, std::string section, std::string key)
{
    std::string str;
    str = "\"" + *value + "\"";
    if ((ini->get(section).get(key) != str.c_str()) && (ini->get(section).get(key) != "" || *value != ""))
    {
        (*ini)[section][key] = str;
    }
}

void UndertaleSave::INIWrite(mINI::INIStructure *ini, bool *value, std::string section, std::string key)
{
    std::string str;
    double val = static_cast<double>(*value);
    str = "\"" + std::to_string(val) + "\"";
    if ((ini->get(section).get(key) != str.c_str()) && (ini->get(section).get(key) != "" || *value))
    {
        (*ini)[section][key] = str;
    }
}

void UndertaleSave::INIWrite(mINI::INIStructure *ini, int *value, std::string section, std::string key)
{
    std::string str;
    double val = static_cast<double>(*value);
    str = "\"" + std::to_string(val) + "\"";
    if ((ini->get(section).get(key) != str.c_str()) && (ini->get(section).get(key) != "" || *value > 0))
    {
        (*ini)[section][key] = str;
    }
}

void UndertaleSave::INIWrite(mINI::INIStructure *ini, double *value, std::string section, std::string key)
{
    std::string str;
    double val = *value;
    str = "\"" + std::to_string(val) + "\"";
    if ((ini->get(section).get(key) != str.c_str()) && (ini->get(section).get(key) != "" || *value > 0))
    {
        (*ini)[section][key] = str;
    }
}