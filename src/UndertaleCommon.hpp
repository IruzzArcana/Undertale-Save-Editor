#ifndef UNDERTALECOMMON_HPP
#define UNDERTALECOMMON_HPP

#include <string>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

class UndertaleCommon
{
    public:
        struct UndertaleSaveFile
        {
            bool initialized;
            std::string name;
            int lv;
            int maxhp;
            int maxen;
            int at;
            int wstrength;
            int df;
            int adef;
            int sp;
            int xp;
            int gold;
            int kills;
            int items[8];
            int phone[8];
            int weapon;
            int armor;
            double flags[512];
            int plot;
            int menuchoice[3];
            int currentsong;
            int currentroom;
            int time;
            int xbox_disconnect_counter;
            int xbox_coins_donated;
        };

        struct UndertaleINIGeneral
        {
            bool initialized;
            std::string Name;
            double Time;
            int Room;
            int Gameover;
            int Kills;
            int Love;
            int fun;
            bool Tale;
            int Won;
            int BW;
            int BC;
            bool CP;
            bool BP;
            bool CH;
            bool BH;
            bool DB;
        };
        struct UndertaleINIReset
        {
            bool initialized;
            bool reset;
            bool s_key;
        };
        struct UndertaleINIFlowey
        {
            bool initialized;
            int Met1;
            bool K;
            bool NK;
            bool IK;
            bool SK;
            bool FloweyExplain1;
            int EX;
            int CHANGE;
            bool AK;
            bool AF;
            int Alter;
            int alter2;
            int truename;
            int SPECIALK;
        };
        struct UndertaleINIToriel
        {
            bool initialized;
            int Bscotch;
            int TS;
            int TK;
        };
        struct UndertaleINISans
        {
            bool initialized;
            int M1;
            bool EndMet;
            int MeetLv1;
            int MeetLv2;
            int MeetLv;
            int Pass;
            int Intro;
            int F;
            int MP;
            int SK;
            int SS;
            int SS2;
        };
        struct UndertaleINIPapyrus
        {
            bool initialized;
            int M1;
            int PS;
            int PD;
            int PK;
        };
        struct UndertaleINIFFFFF
        {
            bool initialized;
            bool F;
            int D;
            int P;
            int E;
        };
        struct UndertaleINIUndyne
        {
            bool initialized;
            int UD;
        };
        struct UndertaleINIMettaton
        {
            bool initialized;
            bool BossMet;
        };
        struct UndertaleINIMett
        {
            bool initialized;
            bool O;
        };
        struct UndertaleINIMTT
        {
            bool initialized;
            int EssayNo;
        };
        struct UndertaleINIAsgore
        {
            bool initialized;
            int KillYou;
        };
        struct UndertaleINIAlphys
        {
            bool initialized;
            int AD;
        };
        struct UndertaleINIF7
        {
            bool initialized;
            bool F7;
        };
        struct UndertaleINIEndF
        {
            bool initialized;
            int EndF;
        };
        struct UndertaleINIDogshrine
        {
            bool initialized;
            bool Donated;
        };
        struct UndertaleINI
        {
            UndertaleINIGeneral general;
            UndertaleINIReset reset;
            UndertaleINIFlowey flowey;
            UndertaleINIToriel toriel;
            UndertaleINISans sans;
            UndertaleINIPapyrus papyrus;
            UndertaleINIFFFFF fffff;
            UndertaleINIUndyne undyne;
            UndertaleINIMettaton mettaton;
            UndertaleINIMett mett;
            UndertaleINIMTT mtt;
            UndertaleINIAsgore asgore;
            UndertaleINIAlphys alphys;
            UndertaleINIF7 f7;
            UndertaleINIEndF endf;
            UndertaleINIDogshrine dogshrine;
        };
        static const char * title;
};
#endif