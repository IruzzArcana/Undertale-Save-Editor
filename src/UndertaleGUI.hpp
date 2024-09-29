#include "imgui.h"
#include "imgui_stdlib.h"
#include "UndertaleCommon.hpp"
#include <sstream>
#include <iomanip>

class UndertaleGUI
{
    public:
        enum menuModes
        {
            SHOW_NONE,
            SHOW_UNDERTALE
        };
        enum fileDialogModes
        {
            FILE_NONE,
            FILE_LOAD,
            FILE_SAVE,
            FILE_SAVE_AS
        };

        static void DrawMenuBar(bool enabled, UndertaleCommon::UndertaleSaveFile save[3], int mode);
        static void DrawAboutPage(bool enabled);
        static void DrawSaveEditor(bool enalbed, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static bool Quit() { return quit; };
        static bool ShowAboutPage() { return show_about_page; };
        static int GetFile() { return current_file; };
        static int ShowFileDialog() { return show_file_dialog; };
        static void HideFileDialog() { show_file_dialog = FILE_NONE; };
    private:
        static int DrawCombo(const char * label, const char * state[], int size, int selected_state, int index_override[] = {}, int index_size = 0);
        static bool quit;
        static bool show_about_page;
        static int current_file;
        static int show_file_dialog;
};