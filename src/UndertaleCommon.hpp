#pragma once

#include <string>

#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

class UndertaleCommon
{
    public:
        struct UndertaleSaveFile
        {
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
        enum class UndertaleItems
        {
            None,
            Monster_Candy,
            Croquet_Roll,
            Stick,
            Bandage,
            Rock_Candy,
            Pumpkin_Rings,
            Spider_Donut,
            Stoic_Onion,
            Ghost_Fruit,
            Spider_Cider,
            Butterscotch_Pie,
            Faded_Ribbon,
            Toy_Knife,
            Tough_Glove,
            Manly_Bandana,
            Snowman_Piece,
            Nice_Cream,
            Puppydough_Icecream,
            Bisicle,
            Unisicle,
            Cinnamon_Bun,
            Temmie_Flakes,
            Abandoned_Quiche,
            Old_Tutu,
            Ballet_Shoes,
            Punch_Card,
            Annoying_Dog,
            Dog_Salad,
            Dog_Residue_1,
            Dog_Residue_2,
            Dog_Residue_3,
            Dog_Residue_4,
            Dog_Residue_5,
            Dog_Residue_6,
            Astronaut_Food,
            Instant_Noodles,
            Crab_Apple,
            Hot_Dog,
            Hot_Cat,
            Glamburger,
            Sea_Tea,
            Starfait,
            Legendary_Hero,
            Cloudy_Glasses,
            Torn_Notebook,
            Stained_Apron,
            Burnt_Pan,
            Cowboy_hat,
            Empty_Gun,
            Heart_Locket,
            Worn_Dagger,
            Real_Knife,
            The_Locket,
            Bad_Memory,
            Dream,
            Undyne_Letter,
            Undyne_Letter_EX,
            Potato_Chisps,
            Junk_Food,
            Mystery_Key,
            Face_Steak,
            Hush_Puppy,
            Snail_Pie,
            Temmie_Armor
        };

        enum class UndertaleWeapons
        {
            None,
            Stick = 4,
            Toy_Knife = 13,
            Tough_Glove = 14,
            Ballet_Shoes = 25,
            Torn_Notebook = 45,
            Burnt_Pan = 47,
            Empty_Gun = 49,
            Worn_Dagger = 51,
            Real_Knife = 52
        };
        static const char * title;
};