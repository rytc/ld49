
struct Dialog_Sequence_Definition {
    u32 dialog_count;
    u32 author;
    const char *lines[10];
};

enum Dialog_Sequence_ID {
    DIALOG_SEQUENCE_INTRO,
};

static const Dialog_Sequence_Definition d_sequences[] = {
    {3, 0, {
            "Hello, Traveler.",
            "Do you see that Legendary Sword? It's much too powerful for you,\n Traveler.",
            "Perhaps if you bring me some loot, I'll offer you the chance to buy it.",
        }}
};