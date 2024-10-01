#include "UndertaleGUI.hpp"
#include "imgui.h"
#include "imgui_stdlib.h"

bool UndertaleGUI::quit = false;
int UndertaleGUI::current_file = 0;
bool UndertaleGUI::show_about_page = false;
int UndertaleGUI::show_file_dialog = FILE_NONE;

void UndertaleGUI::DrawMenuBar(bool enabled, UndertaleCommon::UndertaleSaveFile save[3], int mode)
{
    if (enabled)
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                {
                    show_file_dialog = FILE_LOAD;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "Ctrl+S", false, mode > SHOW_NONE))
                {
                    show_file_dialog = FILE_SAVE;
                }
                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S", false, mode > SHOW_NONE))
                {
                    show_file_dialog = FILE_SAVE_AS;
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
                    if (ImGui::MenuItem("undertale.ini", 0, current_file == 3))
                    {
                        current_file = 3;
                    }
                    if (ImGui::MenuItem("config.ini", 0, current_file == 4))
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
        ImGui::Text("v0.0.1");
        ImGui::End();
    }
}

void UndertaleGUI::DrawSaveEditor(bool enabled, UndertaleCommon::UndertaleSaveFile *save, bool is_xbox)
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
    }
}

void UndertaleGUI::DrawFileEditor(const char *title, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox)
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
                for (int i = 0; i < 8; i++)
                {
                    std::string label = "Slot " + std::to_string(i + 1);
                    ImGui::InputInt(label.c_str(), &save->phone[i]);
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