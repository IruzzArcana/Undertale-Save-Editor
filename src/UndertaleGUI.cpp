#include "UndertaleGUI.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"

bool UndertaleGUI::quit = false;
int UndertaleGUI::current_file = 0;
bool UndertaleGUI::show_about_page = false;
int UndertaleGUI::show_file_dialog = FILE_NONE;

void UndertaleGUI::DrawMenuBar(bool enabled, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI * ini, UndertaleCommon::UndertaleConfigINI *config, int mode)
{
    if (enabled)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open (PC Save)"))
                {
                    show_file_dialog = FILE_LOAD;
                }
                if (ImGui::MenuItem("Open (Console Save)"))
                {
                    show_file_dialog = FILE_LOAD_SAV;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "", false, mode > SHOW_NONE))
                {
                    show_file_dialog = FILE_SAVE;
                }
                if (ImGui::MenuItem("Save As... (PC Save)", "", false, mode > SHOW_NONE))
                {
                    show_file_dialog = FILE_SAVE_AS;
                }
                if (ImGui::MenuItem("Save As... (Console Save)", "", false, mode > SHOW_NONE))
                {
                    show_file_dialog = FILE_SAVE_AS_SAV;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Quit", "Alt+F4"))
                {
                    quit = true;
                }
                ImGui::EndMenu();
            }
            if (mode == SHOW_UNDERTALE)
            {
                if (ImGui::BeginMenu("Editor"))
                {
                    if (ImGui::MenuItem("file0", 0, current_file == 0))
                    {
                        current_file = 0;
                    }
                    if (save[1].initialized && ImGui::MenuItem("file9", 0, current_file == 1))
                    {
                        current_file = 1;
                    }
                    if (save[2].initialized && ImGui::MenuItem("file8", 0, current_file == 2))
                    {
                        current_file = 2;
                    }
                    if (ini->initialized && ImGui::MenuItem("undertale.ini", 0, current_file == 3))
                    {
                        current_file = 3;
                    }
                    if (config->initialized && ImGui::MenuItem("config.ini", 0, current_file == 4))
                    {
                        current_file = 4;
                    }

                    ImGui::EndMenu();
                }
            }
            if (ImGui::MenuItem("About"))
            {
                show_about_page = true;
            }
            ImGui::EndMainMenuBar();
        }
    }
}

void UndertaleGUI::DrawAboutPage(bool enabled)
{
    if (enabled)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("About", &show_about_page, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
        ImGui::Text(UndertaleCommon::title);
        ImGui::Text("v1.0.1");
        ImGui::End();
    }
}

void UndertaleGUI::DrawSaveEditor(bool enabled, UndertaleCommon::UndertaleSaveFile *save, UndertaleCommon::UndertaleINI *ini, UndertaleCommon::UndertaleConfigINI *config, bool is_xbox)
{
    if (enabled)
    {
        std::string title = "Save Editor";
        switch (current_file)
        {
        case 0:
            title += " - file0";
            break;
        case 1:
            title += " - file9";
            break;
        case 2:
            title += " - file8";
            break;
        case 3:
            title += " - undertale.ini";
            break;
        case 4:
            title += " - config.ini";
            break;
        }
        if (current_file < 3)
            DrawFileEditor(title.c_str(), save, is_xbox);
        else if (current_file == 3)
            DrawINIEditor(title.c_str(), ini, is_xbox);
        else
            DrawConfigINIEditor(title.c_str(), config);
    }
}

void UndertaleGUI::DrawFileEditor(const char *title, UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
{
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::Begin(title, NULL, ImGuiWindowFlags_NoCollapse);
    if (ImGui::BeginTabBar("FileTabbar"))
    {
        if (ImGui::BeginTabItem("General"))
        {
            ImGui::InputTextWithHint("Name", "Name the fallen human.", &save->name);
            ImGui::InputDouble("FUN", &save->flags[5]);
            ImGui::InputInt("LOVE", &save->lv);
            ImGui::InputInt("Kills", &save->kills);
            ImGui::InputInt("Room", &save->currentroom);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Stats"))
        {
            ImGui::InputInt("HP", &save->maxhp);
            ImGui::InputInt("EXP", &save->xp);
            ImGui::InputInt("Gold", &save->gold);
            ImGui::InputInt("ATK", &save->at);
            ImGui::InputInt("Weapon ATK", &save->wstrength);
            ImGui::InputInt("Weapon", &save->weapon);
            ImGui::InputInt("Armor", &save->armor);

            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Inventory"))
        {
            for (int i = 0; i < 8; i++)
            {
                std::string label = "Item " + std::to_string(i + 1);
                ImGui::InputInt(label.c_str(), &save->items[i]);
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Cellphone"))
        {
            bool cellphone = save->menuchoice[2];
            ImGui::Checkbox("You have a cellphone", &cellphone);
            save->menuchoice[2] = cellphone;
            if (cellphone)
            {
                const char * cell_state[10] = {"NONE", "Say Hello", "Puzzle Help", "About Yourself", "Call Her \"Mom\"", "Flirt", "Toriel's Phone", "Papyrus's Phone", "Dimensional Box A", "Dimensional Box B"};
                int cell_index[IM_ARRAYSIZE(cell_state)] = {0, 201, 202, 203, 204, 205, 206, 210, 220, 221};
                for (int i = 0; i < 8; i++)
                {
                    std::string label = "Slot " + std::to_string(i + 1);
                    int selected_cell = save->phone[i];
                    selected_cell = DrawCombo(label.c_str(), cell_state, IM_ARRAYSIZE(cell_state), selected_cell, cell_index, IM_ARRAYSIZE(cell_index));
                    save->phone[i] = selected_cell;
                }
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Areas"))
        {
            if (ImGui::BeginTabBar("AreaTabs"))
            {
                if (ImGui::BeginTabItem("Ruins"))
                {
                    ImGui::InputDouble("Kills", &save->flags[202]);

                    const char *dummy_state[] = {"Initial State", "Killed", "Talked to", "Tired of your shenanigans"};
                    static int selected_dummy = save->flags[14];
                    selected_dummy = DrawCombo("Dummy", dummy_state, IM_ARRAYSIZE(dummy_state), selected_dummy);
                    save->flags[14] = selected_dummy;

                    const char *toriel_state[] = {"Initial State", "In basement", "Fled from", "Killed", "Spared"};
                    static int toriel_index[IM_ARRAYSIZE(toriel_state)] = {0, 1, 3, 4, 5};
                    static int selected_toriel = save->flags[45];
                    selected_toriel = DrawCombo("Toriel", toriel_state, IM_ARRAYSIZE(toriel_state), selected_toriel, toriel_index, IM_ARRAYSIZE(toriel_index));
                    save->flags[45] = selected_toriel;

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Snowdin"))
                {
                    ImGui::InputDouble("Kills", &save->flags[203]);

                    const char *doggo_state[] = {"Initial State", "Killed", "Played fetch (Spared)"};
                    static int selected_doggo = save->flags[53];
                    selected_doggo = DrawCombo("Doggo", doggo_state, IM_ARRAYSIZE(doggo_state), selected_doggo);
                    save->flags[53] = selected_doggo;

                    const char *dogi_state[] = {"Initial State", "Killed", "Played fetch (Spared)"};
                    static int selected_dogi = save->flags[54];
                    selected_dogi = DrawCombo("Dogamy and Dogaressa", dogi_state, IM_ARRAYSIZE(dogi_state), selected_dogi);
                    save->flags[54] = selected_dogi;

                    const char *comedian_state[] = {"Initial State", "Laughed at joke [Yellow Credits]", "Killed"};
                    static int selected_comedian = save->flags[57];
                    selected_comedian = DrawCombo("Snowdrake", comedian_state, IM_ARRAYSIZE(comedian_state), selected_comedian);
                    save->flags[57] = selected_comedian;

                    const char *papyrus_state[] = {"Lost to thrice", "Lost to twice", "Lost to once", "Initial State", "Killed"};
                    static int papyrus_index[IM_ARRAYSIZE(papyrus_state)] = {-3, -2, -1, 0, 1};
                    static int selected_papyrus = save->flags[67];
                    selected_papyrus = DrawCombo("Papyrus", papyrus_state, IM_ARRAYSIZE(papyrus_state), selected_papyrus, papyrus_index, IM_ARRAYSIZE(papyrus_index));
                    save->flags[67] = selected_papyrus;

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Waterfall"))
                {
                    ImGui::InputDouble("Kills", &save->flags[204]);

                    const char *shyren_state[] = {"Initial State", "Killed", "Continued humming [Yellow credit]"};
                    static int selected_shyren = save->flags[81];
                    selected_shyren = DrawCombo("Shyren", shyren_state, IM_ARRAYSIZE(shyren_state), selected_shyren);
                    save->flags[81] = selected_shyren;

                    const char *maddummy_state[] = {"Initial State", "Killed"};
                    static int selected_maddummy = save->flags[252];
                    selected_maddummy = DrawCombo("Mad Dummy", maddummy_state, IM_ARRAYSIZE(maddummy_state), selected_maddummy);
                    save->flags[252] = selected_maddummy;

                    const char *undyne_state[] = {"Initial State", "Killed", "Sick"};
                    static int selected_undyne = save->flags[350];
                    selected_undyne = DrawCombo("Undyne", undyne_state, IM_ARRAYSIZE(undyne_state), selected_undyne);
                    save->flags[350] = selected_undyne;

                    const char *undying_state[] = {"Initial State", "Killed"};
                    static int selected_undying = save->flags[251];
                    selected_undying = DrawCombo("Undyne the Undying", undying_state, IM_ARRAYSIZE(undying_state), selected_undying);
                    save->flags[251] = selected_undying;

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Hotland"))
                {
                    ImGui::InputDouble("Kills", &save->flags[205]);

                    const char *state[] = {"Initial State", "Killed"};

                    static int selected_guards = save->flags[402];
                    selected_guards = DrawCombo("RG01 & RG02", state, IM_ARRAYSIZE(state), selected_guards);
                    save->flags[402] = selected_guards;

                    static int selected_muffet = save->flags[397];
                    selected_muffet = DrawCombo("Muffet", state, IM_ARRAYSIZE(state), selected_muffet);
                    save->flags[397] = selected_muffet;

                    static int selected_mettaton = save->flags[425];
                    selected_mettaton = DrawCombo("Mettaton", state, IM_ARRAYSIZE(state), selected_mettaton);
                    save->flags[425] = selected_mettaton;

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Other"))
        {
            bool epilogue = (save->flags[7] == 1);
            ImGui::Checkbox("True Pacifist", &epilogue);
            save->flags[7] = epilogue;
            ImGui::InputDouble("Pacifist progression", &save->flags[493]);
            if (is_xbox)
            {
                ImGui::InputInt("Xbox Disconnect Count", &save->xbox_disconnect_counter);
                ImGui::InputInt("Xbox Coins Donated", &save->xbox_coins_donated);
                save->flags[299] = save->xbox_coins_donated;
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Flags"))
        {
            for (int i = 0; i < 512; i++)
            {
                std::string label = "Flag " + std::to_string(i);
                ImGui::InputDouble(label.c_str(), &save->flags[i]);
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
}

void UndertaleGUI::DrawINIEditor(const char *title, UndertaleCommon::UndertaleINI *ini, bool is_xbox)
{
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::Begin(title, NULL, ImGuiWindowFlags_NoCollapse);

    if (ImGui::CollapsingHeader("General"))
    {
        ImGui::InputTextWithHint("Name", "Name the fallen human.", &ini->general.Name);
        ImGui::InputDouble("Time", &ini->general.Time);
        ImGui::InputInt("Room", &ini->general.Room);
        ImGui::InputInt("Game Over", &ini->general.Gameover);
        ImGui::SameLine();
        HelpMarker("How many times you've died.");

        ImGui::InputInt("Kills", &ini->general.Kills);
        ImGui::InputInt("LOVE", &ini->general.Love);
        ImGui::InputInt("fun", &ini->general.fun);
        ImGui::InputInt("Won", &ini->general.Won);
        ImGui::SameLine();
        HelpMarker("Defeated Flowey in the Neutral route.");
        
        ImGui::InputInt("BW", &ini->general.BW);
        ImGui::SameLine();
        HelpMarker("The amount of times you had Won before the last True Reset.\nUsed for unlocking the Dynamic borders.");
       
        ImGui::InputInt("BC", &ini->general.BC);
        ImGui::SameLine();
        HelpMarker("The total number of items you have obtained.\nUsed for unlocking item achievements on PS.");
        
        ImGui::Checkbox("Tale", &ini->general.Tale);
        ImGui::SameLine();
        HelpMarker("Listened to the monsters tale in New Home.");
        ImGui::SameLine();
       
        ImGui::Checkbox("CP", &ini->general.CP);
        ImGui::SameLine();
        HelpMarker("Completed True Pacifist.\nUsed for unlocking the Dynamic borders.\nAlso unlocks the True Lab border.");
        ImGui::SameLine();
       
        ImGui::Checkbox("BP", &ini->general.BP);
        ImGui::SameLine();
        HelpMarker("True Reset with CP set to 1.\nUsed for unlocking the Dynamic borders.\nAlso unlocks the True Lab border.");
        ImGui::SameLine();
       
        ImGui::Checkbox("CH", &ini->general.CH);
        if (ini->general.CH && ini->general.BH)
            ini->general.BH = false;
        ImGui::SameLine();
        HelpMarker("Completed Hard Mode.\nUsed for unlocking the Beauty Border.");
        ImGui::SameLine();
    
        ImGui::Checkbox("BH", &ini->general.BH);
        if (ini->general.BH && ini->general.CH)
            ini->general.CH = false;
        ImGui::SameLine();
        HelpMarker("Pressed True Reset with Beauty Unlocked.");
        ImGui::SameLine();
        
        ImGui::Checkbox("DB", &ini->general.DB);
        
        ImGui::SameLine();
        HelpMarker("Interacted with the Legendary Artifact.\nUsed for unlocking Super Dog, Hoi.");
    }

    if (ImGui::CollapsingHeader("Reset"))
    {
        ImGui::Checkbox("reset", &ini->reset.reset);
        ImGui::SameLine();
        HelpMarker("Pressed True Reset.");
        ImGui::SameLine();

        ImGui::Checkbox("s_key", &ini->reset.s_key);
        ImGui::SameLine();
        HelpMarker("Dodged all of the credits.");
    }

    if (ImGui::CollapsingHeader("Flowey"))
    {
        ImGui::InputInt("Met1", &ini->flowey.Met1);
        ImGui::SameLine();
        HelpMarker("Number of times you've met Flowey at the start of the game.");        

        ImGui::InputInt("EX", &ini->flowey.EX);
        ImGui::SameLine();
        HelpMarker("How many times you've heard Flowey's post-neutral dialogue.");

        ImGui::InputInt("CHANGE", &ini->flowey.CHANGE);
        ImGui::SameLine();
        HelpMarker("Set to 1 after aborting a neutral-murderer path\n and 2 after going back to killing.");

        ImGui::InputInt("Alter", &ini->flowey.Alter);
        ImGui::SameLine();
        HelpMarker("Spared Photoshop Flowey.");

        ImGui::InputInt("alter2", &ini->flowey.alter2);
        ImGui::SameLine();
        HelpMarker("Triggered a genocide route and met Flowey in the room before\n the ruins exit.");

        ImGui::InputInt("truename", &ini->flowey.truename);
        ImGui::SameLine();
        HelpMarker("Same as alter2, but if set Flowey won't be in the room before the ruins exit.");

        ImGui::InputInt("SPECIALK", &ini->flowey.SPECIALK);
        ImGui::SameLine();
        HelpMarker("Prevents Flowey from showing up after the post-neutral phonecall again.");
        
        ImGui::Checkbox("K", &ini->flowey.K);
        ImGui::SameLine();
        HelpMarker("Killed Flowey.");
        ImGui::SameLine();

        ImGui::Checkbox("NK", &ini->flowey.NK);
        ImGui::SameLine();
        HelpMarker("Spared Flowey during a neutral-pacifist route.");
        ImGui::SameLine();

        ImGui::Checkbox("IK", &ini->flowey.IK);
        ImGui::SameLine();
        HelpMarker("Spared Flowey during a neutral-with-EXP route.");
        ImGui::SameLine();

        ImGui::Checkbox("SK", &ini->flowey.SK);
        ImGui::SameLine();
        HelpMarker("Asgore commited suicide.");
        ImGui::SameLine();

        ImGui::Checkbox("FloweyExplain1", &ini->flowey.FloweyExplain1);
        ImGui::SameLine();
        HelpMarker("Spared Toriel after Killing her, or vice-versa.");
        
        ImGui::Checkbox("AK", &ini->flowey.AK);
        ImGui::SameLine();
        HelpMarker("Killed Asgore during a neutral-pacifist route.");
        ImGui::SameLine();
        
        ImGui::Checkbox("AF", &ini->flowey.AF);
        ImGui::SameLine();
        HelpMarker("Completed Alphys date but reached a neutral ending anyway.");
    }

    if (ImGui::CollapsingHeader("Toriel"))
    {
        const char * bscotch_state[3] = {"Not set", "Butterscotch", "Cinnamon"};
        int selected_bscotch = ini->toriel.Bscotch;
        selected_bscotch = DrawCombo("Bscotch", bscotch_state, IM_ARRAYSIZE(bscotch_state), selected_bscotch);
        ini->toriel.Bscotch = selected_bscotch;
        ImGui::SameLine();
        HelpMarker("Pie preferences.");

        ImGui::InputInt("TS", &ini->toriel.TS);
        ImGui::SameLine();
        HelpMarker("Number of times you've spared Toriel.");

        ImGui::InputInt("TK", &ini->toriel.TK);
        ImGui::SameLine();
        HelpMarker("Number of times you've killed Toriel.");
    }

    if (ImGui::CollapsingHeader("Sans"))
    {
        ImGui::InputInt("M1", &ini->sans.M1);
        ImGui::SameLine();
        HelpMarker("Number of times you've met Sans after exiting the ruins.");

        ImGui::Checkbox("EndMet", &ini->sans.EndMet);
        ImGui::SameLine();
        HelpMarker("Met sans in the chapel near the end of the neutral run.");

        ImGui::InputInt("MeetLv1", &ini->sans.MeetLv1);
        ImGui::SameLine();
        HelpMarker("The number of times you've met Sans with no kills.");

        ImGui::InputInt("MeetLv2", &ini->sans.MeetLv2);
        ImGui::SameLine();
        HelpMarker("The number of times you've met Sans with kills.");

        ImGui::InputInt("MeetLv", &ini->sans.MeetLv);
        ImGui::SameLine();
        HelpMarker("The LOVE you last met Sans at.");

        const char * pass_state[4] = {"No password", "Secret codeword", "Secret secret codeword", "Basement key acquired"};
        int selected_pass = ini->sans.Pass;
        selected_pass = DrawCombo("Pass", pass_state, IM_ARRAYSIZE(pass_state), selected_pass);
        ini->sans.Pass = selected_pass;
        ImGui::SameLine();
        HelpMarker("Pacifist password rank.");

        ImGui::InputInt("Intro", &ini->sans.Intro);
        ImGui::SameLine();
        HelpMarker("The number of times you've heard Sans' intro in a Genocide route.");

        ImGui::InputInt("F", &ini->sans.F);
        ImGui::SameLine();
        HelpMarker("The number of times you've fought Sans in a Genocide route.");

        ImGui::InputInt("F", &ini->sans.MP);
        ImGui::SameLine();
        HelpMarker("The number of times you've reached the \"midpoint\" of\nSans' fight in a Genocide Route.");
        
        ImGui::InputInt("SK", &ini->sans.SK);
        ImGui::SameLine();
        HelpMarker("The number of times you've killed Sans in a Genocide route.");

        ImGui::InputInt("SS", &ini->sans.SS);
        ImGui::SameLine();
        HelpMarker("The number of times you've spared Sans in a Genocide route.");

        ImGui::InputInt("SS2", &ini->sans.SS2);
        ImGui::SameLine();
        HelpMarker("The number of times you've attacked Sans after getting dunked on.");
    }

    if (ImGui::CollapsingHeader("Papyrus"))
    {
        ImGui::InputInt("M1", &ini->papyrus.M1);
        ImGui::SameLine();
        HelpMarker("The number of times you've met Papyrus after meeting\nSans just outside of the ruins.");

        ImGui::InputInt("PS", &ini->papyrus.PS);
        ImGui::SameLine();
        HelpMarker("The number of times you've spared Papyrus.");

        ImGui::InputInt("PD", &ini->papyrus.PD);
        ImGui::SameLine();
        HelpMarker("The number of times you've dated Papyrus.");

        ImGui::InputInt("PK", &ini->papyrus.PK);
        ImGui::SameLine();
        HelpMarker("The number of times you've killed Papyrus.");
    }

    if (ImGui::CollapsingHeader("FFFFF"))
    {
        ImGui::InputInt("D", &ini->fffff.D);
        ImGui::SameLine();
        HelpMarker("The number of times you've died to Flowey.");

        const char * phase_state[8] = {"Initial state", "Light blue", "Orange", "Blue", "Purple", "Green", "Yellow", "Finished battle"};
        int selected_phase = ini->fffff.P;
        selected_phase = DrawCombo("P", phase_state, IM_ARRAYSIZE(phase_state), selected_phase);
        ini->fffff.P = selected_phase;
        ImGui::SameLine();
        HelpMarker("Battle phase.");

        ImGui::InputInt("E", &ini->fffff.E);
        ImGui::SameLine();
        HelpMarker("Reached the end of the battle.");

        ImGui::Checkbox("F", &ini->fffff.F);
        ImGui::SameLine();
        HelpMarker("Trapped in Flowey's World.");
    }

    if (ImGui::CollapsingHeader("Undyne"))
    {
        ImGui::InputInt("UD", &ini->undyne.UD);
        ImGui::SameLine();
        HelpMarker("Befriended Undyne.");
    }

    if (ImGui::CollapsingHeader("Mettaton"))
    {
        ImGui::Checkbox("BossMet", &ini->mettaton.BossMet);
        ImGui::SameLine();
        HelpMarker("Faced Mettaton EX.");
    }

    if (ImGui::CollapsingHeader("Mett"))
    {
        ImGui::Checkbox("O", &ini->mett.O);
        ImGui::SameLine();
        HelpMarker("Completed Mettaton's opera section.");
    }

    if (ImGui::CollapsingHeader("MTT"))
    {
        ImGui::InputInt("EssayNo", &ini->mtt.EssayNo);
        ImGui::SameLine();
        HelpMarker("The number of times you've completed Mettaton's Essay.\nShortens the time you get to write subsequently.");
    }

    if (ImGui::CollapsingHeader("Asgore"))
    {
        ImGui::InputInt("KillYou", &ini->asgore.KillYou);
        ImGui::SameLine();
        HelpMarker("The number of times Asgore has killed you.");
    }

    if (ImGui::CollapsingHeader("Alphys"))
    {
        ImGui::InputInt("AD", &ini->alphys.AD);
        ImGui::SameLine();
        HelpMarker("Dated Alphys.");

        const char * mewmew_state[3] = {"Initial State", "Mew Mew Spared", "Mew Mew Killed"};
        int mewmew_index[IM_ARRAYSIZE(mewmew_state)] = {0, 2, 3};
        int selected_mewmew = ini->alphys.M;
        selected_mewmew = DrawCombo("M", mewmew_state, IM_ARRAYSIZE(mewmew_state), selected_mewmew, mewmew_index, IM_ARRAYSIZE(mewmew_index));
        ini->alphys.M = selected_mewmew;
        ImGui::SameLine();
        HelpMarker("Defeated Mad Mew Mew.\nUsed for unlocking the Mew Mew Border.");

        const char * adate_state[3] = {" ", "Real", "Not Real"};
        int selected_adate = ini->alphys.R;
        selected_adate = DrawCombo("R", adate_state, IM_ARRAYSIZE(adate_state), selected_adate);
        ini->alphys.R = selected_adate;
        ImGui::SameLine();
        HelpMarker("ANIME'S REAL, RIGHT?!?!\nUsed for naming the Mew Mew Border.");
    }

    if (ImGui::CollapsingHeader("F7"))
    {
        ImGui::Checkbox("F7 ", &ini->f7.F7);
        ImGui::SameLine();
        HelpMarker("Spared Asriel.\nPushes the start menu to its fullest stage.");
    }

    if (ImGui::CollapsingHeader("EndF"))
    {
        const char * endf_state[3] = {"Initial State", "Post Pacifist Credits", "Listened to Flowey's warning"};
        int selected_endf = ini->endf.EndF;
        selected_endf = DrawCombo("EndF ", endf_state, IM_ARRAYSIZE(endf_state), selected_endf);
        ini->endf.EndF = selected_endf;
    }

    if (is_xbox && ImGui::CollapsingHeader("Dogshrine"))
    {
        ImGui::Checkbox("Donated", &ini->dogshrine.Donated);
        ImGui::SameLine();
        HelpMarker("Donated to the Dog Casino.\nUnlocks the Casino border.");
    }
    ImGui::End();
}

void UndertaleGUI::DrawConfigINIEditor(const char * title, UndertaleCommon::UndertaleConfigINI * config)
{
    ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::Begin(title, NULL, ImGuiWindowFlags_NoCollapse);

    ImGui::SeparatorText("General");
    const char * languages[2] = {"English", "Japanese"};
    const char * lang[2] = {"en", "ja"};
    int selected_language = 0;
    for (int i = 0; i < IM_ARRAYSIZE(lang); i++)
    {
        if (config->general.lang == lang[i])
        {
            selected_language = i;
        }
    }
    selected_language = DrawCombo("LANGUAGE", languages, IM_ARRAYSIZE(languages), selected_language);
    config->general.lang = lang[selected_language];

    const char * borders[14] = {"NONE", "Simple", "Sepia", "Dynamic", "Ruins", "Snowdin", "Waterfall", "Hotland", "Castle", "True lab", "Beauty", "Real/Not Real", "Super Dog, Hoi", "Casino"};
    int selected_border = config->general.sb;
    selected_border = DrawCombo("BORDER", borders, IM_ARRAYSIZE(borders), selected_border);
    config->general.sb = selected_border;

    ImGui::SeparatorText("joypad1");
    ImGui::InputInt("CONFIRM", &config->joypad1.b0);
    ImGui::InputInt("CANCEL", &config->joypad1.b1);
    ImGui::InputInt("MENU", &config->joypad1.b2);
    ImGui::InputDouble("ANALOG SENSITIVITY", &config->joypad1.as);

    const char * joy_dir[3] = {"NORMAL", "ANALOG ONLY", "POV ONLY"};
    int selected_joy = config->joypad1.jd;
    selected_joy = DrawCombo("DIR CHOICE", joy_dir, IM_ARRAYSIZE(joy_dir), selected_joy);
    config->joypad1.jd = selected_joy;
    
    ImGui::End();
}

// kill me
int UndertaleGUI::DrawCombo(const char *label, const char *state[], int size, int selected_state, int index_override[], int index_size)
{
    int selected = selected_state;
    if (index_size > 0)
    {
        for (int j = 0; j < index_size; j++)
        {
            if (index_override[j] == selected_state)
            {
                selected = j;
                break;
            }
        }
    }

    if (selected < 0 || selected > size)
        selected = 0;

    if (ImGui::BeginCombo(label, state[selected]))
    {
        for (int i = 0; i < size; i++)
        {
            const bool is_selected = (selected == i);
            if (ImGui::Selectable(state[i], is_selected))
            {
                if (index_size > 0)
                {
                    selected_state = index_override[i];
                }
                else
                    selected_state = i;
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return selected_state;
}

void UndertaleGUI::HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}