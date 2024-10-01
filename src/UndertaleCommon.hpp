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
            std::string Name;
            double Time;
            double Room;
            double Gameover;
            double Kills;
            double Love;
            double fun;
            bool Tale;
            double Won;
            double BW;
            double BC;
            bool CP;
            bool BP;
            bool CH;
            bool BH;
            bool DB;
        };
        struct UndertaleINIFlowey
        {
            double Met1;
            bool K;
            bool NK;
            bool IK;
            bool SK;
            bool FloweyExplain1;
            double EX;
            double CHANGE;
            bool AK;
            bool AF;
            double Alter;
            double alter2;
            double truename;
            double SPECIALK;
        };
        struct UndertaleINIToriel
        {
            double Bscotch;
            double TS;
            double TK;
        };
        struct UndertaleINISans
        {
            bool EndMet;
            double MeetLv1;
            double MeetLv2;
            double MeetLv;
            double Pass;
            double Intro;
            double F;
            double MP;
            double SK;
            double SS;
            double SS2;
        };
        struct UndertaleINIPapyrus
        {
            double M1;
            double PS;
            double PD;
            double PK;
        };
        struct UndertaleINIFFFFF
        {
            bool F;
            double D;
            double P;
            double E;
        };
        struct UndertaleINIUndyne
        {
            double UD;
        };
        struct UndertaleINIMettaton
        {
            bool BossMet;
        };
        struct UndertaleINIMett
        {
            bool O;
        };
        struct UndertaleINIMTT
        {
            double EssayNo;
        };
        struct UndertaleINIAsgore
        {
            double KillYou;
        };
        struct UndertaleINIAlphys
        {
            double AD;
        };
        struct UndertaleINIF7
        {
            bool F7;
        };
        struct UndertaleINIEndF
        {
            double EndF;
        };
        struct UndertaleINIDogshrine
        {
            double Donated;
        };
        struct UndertaleINI
        {
            UndertaleINIGeneral general;
            UndertaleINIFlowey flowey;
            UndertaleINIToriel toriel;
            UndertaleINISans sans;
            UndertaleINIPapyrus papyrus;
            UndertaleINIFFFFF fffff;
            UndertaleINIUndyne undyne;
            UndertaleINIMettaton mettaton;
            UndertaleINIMett mett;
            UndertaleINIMTT MTT;
            UndertaleINIAsgore asgore;
            UndertaleINIAlphys alphys;
            UndertaleINIF7 f7;
            UndertaleINIEndF endf;
            UndertaleINIDogshrine dogshrine;
        };
        static const char * title;
};
#endif