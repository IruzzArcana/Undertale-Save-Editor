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

        UndertaleGUI(UndertaleCommon::UndertaleSaveFile (&save)[3], UndertaleCommon::UndertaleINI * ini, UndertaleCommon::UndertaleConfigINI *config, bool& is_xbox);

        void ManageState();
        void DrawGUI(bool enabled);
        void DrawMenuBar(bool enabled);
        void DrawAboutPage(bool enabled);
        void DrawSaveEditor(bool enalbed, UndertaleCommon::UndertaleSaveFile * save);
        bool Quit() { return state.quit; };
        int ShowFileDialog() { return state.show_file_dialog; };
        void HideFileDialog() { state.show_file_dialog = FILE_NONE; };
    
    private:
        typedef struct GUIState
        {
            int mode = SHOW_NONE;
            bool show_file0 = false;
            bool show_file8 = false;
            bool show_file9 = false;
            bool show_ini = false;
            bool show_configini = false;
            bool show_about_page = false;
            int current_file = 0;
            int show_file_dialog = FILE_NONE;
            bool quit = false;
        } state_t;

        void DrawFileEditor(const char * title, UndertaleCommon::UndertaleSaveFile * save, bool is_xbox);
        void DrawINIEditor(const char * title, UndertaleCommon::UndertaleINI * ini, bool is_xbox);
        void DrawConfigINIEditor(const char * title, UndertaleCommon::UndertaleConfigINI * config);
        int DrawCombo(const char * label, const char * state[], int size, int selected_state, int index_override[] = {}, int index_size = 0);
        void HelpMarker(const char* desc);
        UndertaleCommon::UndertaleSaveFile (&save)[3];
        UndertaleCommon::UndertaleINI * ini;
        UndertaleCommon::UndertaleConfigINI *config;
        bool& is_xbox;
        state_t state;
};