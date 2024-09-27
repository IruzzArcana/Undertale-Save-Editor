#include "UndertaleSave.hpp"

const char * UndertaleSave::path;

using namespace std;

void UndertaleSave::Load(SDL_Window * window, UndertaleCommon::UndertaleSaveFile * save, bool * is_xbox)
{
    path = tinyfd_openFileDialog("Select a Save File", "", 0, NULL, "all files", 0);
    if (path)
    {
        ifstream infile(path);
        string line;
        int num_lines;
        if (!infile.is_open())
        {
            tinyfd_messageBox("Error", "Could not open file!", "ok", "error", 1);
            return;
        }
        while (getline(infile, line))
        {
            num_lines++;
        }
        if (num_lines == 551)
            *is_xbox = true;
        infile.clear();
        infile.seekg(0, ios::beg);
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
    }
}

void UndertaleSave::Save(SDL_Window * window, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox, bool save_as)
{
    if (save_as)
    {
        const char * save_path = tinyfd_saveFileDialog("Save as", "", 0, NULL, "all files");
        if (save_path)
            path = save_path;
    }
    ofstream outfile(path);
    if (!outfile.is_open())
    {
        tinyfd_messageBox("Error", "Could not save the file!", "ok", "error", 1);
        return;
    }
        outfile << save->name << endl;
        outfile << save->lv << endl;
        outfile << save->maxhp << endl;
        outfile << save->maxen << endl;
        outfile << save->at << endl;
        outfile << save->wstrength << endl;
        outfile << save->df << endl;
        outfile << save->adef << endl;
        outfile << save->sp << endl;
        outfile << save->xp << endl;
        outfile << save->gold << endl;
        outfile << save->kills << endl;
        for (int i = 0; i < 8; i++)
        {
            outfile << save->items[i] << endl;
            outfile << save->phone[i] << endl;
        }
        outfile << save->weapon << endl;
        outfile << save->armor << endl;
        for (int i = 0; i < 512; i++)
            outfile << save->flags[i] << endl;
            
        outfile << save->plot << endl;
        for (int i = 0; i < 3; i++)
            outfile << save->menuchoice[i] << endl;
        outfile << save->currentsong << endl;
        outfile << save->currentroom << endl;
        outfile << save->time << endl;
        if (is_xbox)
        {
            outfile << save->xbox_disconnect_counter << endl;
            outfile << save->xbox_coins_donated << endl;
        }
    outfile.close();
}