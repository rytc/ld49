
struct Dialog_Sequence_Definition {
    u32 dialog_count;
    u32 author;
    const char *lines[10];
};

enum Dialog_Sequence_ID {
    DIALOG_SEQUENCE_INTRO,
    DIALOG_SEQUENCE_SWORD_ASK,
    DIALOG_SEQUENCE_SWORD_GET,
    DIALOG_SEQUENCE_LEVELED,
    DIALOG_SEQUENCE_GOLD,
    DIALOG_SEQUENCE_QUALIFIED,
    DIALOG_SEQUENCE_LOSE,
    DIALOG_SEQUENCE_GAME_WIN,
    DIALOG_SEQUENCE_NO_LOOT,
    DIALOG_SEQUENCE_UNS_SUCCESS,
    DIALOG_SEQUENCE_WRONG,
    DIALOG_SEQUENCE_SEARCH_0,
    DIALOG_SEQUENCE_SEARCH_1,
    DIALOG_SEQUENCE_SEARCH_2,
    DIALOG_SEQUENCE_SEARCH_3,
    DIALOG_SEQUENCE_SEARCH_4,
    DIALOG_SEQUENCE_SEARCH_5,
    DIALOG_SEQUENCE_SEARCH_6,
    DIALOG_SEQUENCE_SEARCH_7,
    DIALOG_SEQUENCE_SEARCH_8,
    DIALOG_SEQUENCE_SEARCH_9,
    DIALOG_SEQUENCE_SEARCH_10,
    DIALOG_SEQUENCE_SEARCH_11,
    DIALOG_SEQUENCE_SEARCH_12,
    DIALOG_SEQUENCE_SEARCH_13,
    DIALOG_SEQUENCE_SEARCH_14,
    DIALOG_SEQUENCE_SEARCH_15,



};

static const Dialog_Sequence_Definition d_sequences[] = {
    {3, 0, {
            "Hello, Traveler.",
            "Do you see that Legendary Sword? It's much too powerful for you,\n Traveler.",
            "Perhaps if you bring me some loot, I'll offer you the chance to buy it.\n Talk to me to see what I need.",
        }},
    {1, 1, {
            "Requirements: level 15 and 100 gold.",
        }},
    {3, 0, {
            "Wow Traveler, you have impressed me. Maybe you are of the strongest.",
            "You have earned yourself the Legendary Sword.",
            "Now prove your self by defeating the dragon!"
        }},
    {1, 0, {
            "Congrats, you have reached the level requirement for the Legendary Sword.\n Now you need gold.",
        }},
    {1, 0, {
            "Congrats, you have enough gold for the Legendary Sword.\n Now you need the experience.",
        }},
    {1, 0, {
            "You have qualified for the sword, Traveler. Go ahead and take it.",
        }},

    {1, 0, {
            "You have failed me for the last time Traveler. Your adventers are over.",
        }},
    {1, 0, {
            "You have defeated the dragon! You truely are of the strongest! You Win!",
        }},
    {2, 0, {
            "Are the creatures of the forest too strong for you Traveler?",
            "The Legendary Sword is for the strongest, and you are of\n the weakest."
    }},
    {1, 0, {
        "I'm impressed, that is what I'm looking for. Now fetch me something else."
    }},
    {1, 0, {
        "Don't waste my time with this, bring me what I want!"
    }},
    {1, 0, { // Sword
        "Bring me a sword."
    }},
    {1, 0, { // Bow
        "Find a weapon that is stringed."
    }},
    {1, 0, { // Axe
        "Search for something that can fell trees and foes."
    }},
    {1, 0, { // Leather Armor
        "My stock is low on leather armor."
    }},
    {1, 0, { // Iron Armor
        "Iron armor is in high demand."
    }},
    {1, 0, { // Steel Armor
        "Some high-level clients are looking for Steel Armor."
    }},
    {1, 0, { // Feathers
        "I need material for a pillow." 
    }},
    {1, 0, { // Chicken Meat
        "Bring me chicken to cook for supper." 
    }},
    {1, 0, { // Bones
        "Have any chicken bones? I am going to make stew." 
    }},
    {1, 0, { // Ghost essence
        "I need ghost essence for my potions, Traveler." 
    }},
    {1, 0, { // Candy corn
        "The Trick-or-Treaters are upon us, find something sweet." 
    }},
    {1, 0, { // Garlic
        "Bring me a savoury ingredient." 
    }},
    {1, 0, { // Zombie Flesh
        "I have an order for zombie flesh." 
    }},
    {1, 0, { // Decaying Bones 
        "Bring me the bones of a zombie." 
    }},
    {1, 0, { // Decaying Bones 
        "One zombie brain, please." 
    }},
    {1, 0, { // Pumpkin 
        "Bring me an orange fuit" 
    }},



};