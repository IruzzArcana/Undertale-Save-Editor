#include "UndertaleCommon.hpp"

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
            FILE_SAVE_AS,
            FILE_LOAD_SAV,
            FILE_SAVE_AS_SAV
        };

        static void DrawMenuBar(bool enabled, UndertaleCommon::UndertaleSaveFile save[3], UndertaleCommon::UndertaleINI * ini, UndertaleCommon::UndertaleConfigINI *config, int mode);
        static void DrawAboutPage(bool enabled);
        static void DrawSaveEditor(bool enalbed, UndertaleCommon::UndertaleSaveFile * save, UndertaleCommon::UndertaleINI * ini, UndertaleCommon::UndertaleConfigINI *config, bool is_xbox);
        static bool Quit() { return quit; };
        static bool ShowAboutPage() { return show_about_page; };
        static int GetFile() { return (current_file > 2 ? 0 : current_file); };
        static int ShowFileDialog() { return show_file_dialog; };
        static void HideFileDialog() { show_file_dialog = FILE_NONE; };
    
    private:
        static void DrawFileEditor(const char * title, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        static void DrawINIEditor(const char * title, UndertaleCommon::UndertaleINI * ini, bool is_xbox);
        static void DrawConfigINIEditor(const char * title, UndertaleCommon::UndertaleConfigINI * config);
        static int DrawCombo(const char * label, const char * state[], int size, int selected_state, int index_override[] = {}, int index_size = 0);
        static void HelpMarker(const char* desc);
        static bool quit;
        static bool show_about_page;
        static int current_file;
        static int show_file_dialog;
};