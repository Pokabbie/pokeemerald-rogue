const struct MonCoords gTrainerBackPicCoords[] =
{
    [TRAINER_BACK_PIC_NONE] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_BRENDAN] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_MAY] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_RED] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_LEAF] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_ETHAN] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_LYRA] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_RUBY_SAPPHIRE_BRENDAN] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_RUBY_SAPPHIRE_MAY] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_WALLY] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_STEVEN] = {.size = 8, .y_offset = 4},
    [TRAINER_BACK_PIC_LUCAS] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_DAWN] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_COMMUNITY_ZEFA] = {.size = 8, .y_offset = 5},
    [TRAINER_BACK_PIC_COMMUNITY_NACHOLORD] = {.size = 8, .y_offset = 5},
};

// this table goes functionally unused, since none of these pics are compressed
// and the place they would get extracted to gets overwritten later anyway
// the casts are so they'll play nice with the strict struct definition
#define TRAINER_BACK_SPRITE(trainerPic, sprite, size) [TRAINER_BACK_PIC_##trainerPic] = {(const u32 *)sprite, size, TRAINER_BACK_PIC_##trainerPic}

const struct CompressedSpriteSheet gTrainerBackPicTable[] =
{
    TRAINER_BACK_SPRITE(NONE, gTrainerBackPic_None, 0x2000),
    TRAINER_BACK_SPRITE(BRENDAN, gTrainerBackPic_Brendan, 0x2000),
    TRAINER_BACK_SPRITE(MAY, gTrainerBackPic_May, 0x2000),
    TRAINER_BACK_SPRITE(RED, gTrainerBackPic_Red, 0x2800),
    TRAINER_BACK_SPRITE(LEAF, gTrainerBackPic_Leaf, 0x2800),
    TRAINER_BACK_SPRITE(ETHAN, gTrainerBackPic_Ethan, 0x2800),
    TRAINER_BACK_SPRITE(LYRA, gTrainerBackPic_Lyra, 0x2800),
    TRAINER_BACK_SPRITE(RUBY_SAPPHIRE_BRENDAN, gTrainerBackPic_RubySapphireBrendan, 0x2000),
    TRAINER_BACK_SPRITE(RUBY_SAPPHIRE_MAY, gTrainerBackPic_RubySapphireMay, 0x2000),
    TRAINER_BACK_SPRITE(WALLY, gTrainerBackPic_Wally, 0x2000),
    TRAINER_BACK_SPRITE(STEVEN, gTrainerBackPic_Steven, 0x2000),
    TRAINER_BACK_SPRITE(LUCAS, gTrainerBackPic_Lucas, 0x2800),
};

#define TRAINER_BACK_PAL(trainerPic, pal) [TRAINER_BACK_PIC_##trainerPic] = {pal, TRAINER_BACK_PIC_##trainerPic}

const struct CompressedSpritePalette gTrainerBackPicPaletteTable[] =
{
    TRAINER_BACK_PAL(NONE, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(BRENDAN, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(MAY, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(RED, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(LEAF, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(ETHAN, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(LYRA, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(RUBY_SAPPHIRE_BRENDAN, gTrainerPalette_RubySapphireBrendan),
    TRAINER_BACK_PAL(RUBY_SAPPHIRE_MAY, gTrainerPalette_RubySapphireMay),
    TRAINER_BACK_PAL(WALLY, gTrainerPalette_Wally),
    TRAINER_BACK_PAL(STEVEN, gTrainerPalette_Steven),
    TRAINER_BACK_PAL(LUCAS, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(DAWN, gTrainerPalette_PlayerBackPlaceholder),
    TRAINER_BACK_PAL(COMMUNITY_ZEFA, gTrainerPalette_PlayerZefaBackBase),
    TRAINER_BACK_PAL(COMMUNITY_NACHOLORD, gTrainerPalette_PlayerNacholordBackBase),
};
