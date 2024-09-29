#include "UndertaleSave.hpp"
#include "nfd.hpp"
#include "nfd_sdl2.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

std::string UndertaleSave::dir;
bool UndertaleSave::is_json = false;

void UndertaleSave::Load(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], bool &is_xbox)
{
    NFD::Guard nfdGuard;
    NFD::UniquePath outPath;
    nfdfilteritem_t filterItems[2] = {{"PC Save", "*"}, {"Console Save", "sav"}};
    nfdresult_t result = NFD::OpenDialog(outPath, filterItems, 2);
    if (result == NFD_OKAY)
    {
        std::filesystem::path filepath = outPath.get();
        std::string fileext = filepath.extension().string();
        is_json = (fileext == ".sav");

        if (!is_json)
        {
            dir = filepath.parent_path().string();
            std::vector<std::string> matchingFiles;

            for (const auto &entry : std::filesystem::directory_iterator(dir))
            {
                std::string filename = entry.path().filename().string();
                if ((filename.rfind("file0", 0) == 0) || (filename.rfind("file8", 0) == 0) || (filename.rfind("file9", 0) == 0))
                {
                    matchingFiles.push_back(filename);
                }
            }
            for (std::string file : matchingFiles)
            {
                std::filesystem::path path = std::filesystem::path(dir) / file;
                if (file == "file0")
                {
                    std::ifstream infile(path);
                    std::string line;
                    int num_lines;
                    if (!std::filesystem::exists(path))
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
                    {
                        num_lines++;
                    }
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
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "WIP", "Not implemented yet lol", window);
            return;
        }
    }
}

void UndertaleSave::Save(SDL_Window *window, UndertaleCommon::UndertaleSaveFile save[3], bool is_xbox, bool save_as)
{
    if (save_as)
    {
        NFD::Guard nfdGuard;
        NFD::UniquePath outPath;
        nfdfilteritem_t filterItems[2] = {{"PC Save", "*"}, {"Console Save", "sav"}};
        nfdresult_t result = NFD::SaveDialog(outPath, filterItems, 2);
        if (result == NFD_OKAY)
        {
            std::filesystem::path filepath = outPath.get();
            std::string fileext = filepath.extension().string();
            is_json = (fileext == ".sav");
        }
    }
    if (!is_json)
    {
        std::string files[3] = {"file0", "file9", "file8"};
        int i = 0;
        for (std::string file : files)
        {
            std::filesystem::path path = std::filesystem::path(dir) / file;
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
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "WIP", "Also a work in progress lmao", window);
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