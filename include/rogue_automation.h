#ifdef ROGUE_FEATURE_AUTOMATION

enum
{
    AUTO_INPUT_STATE_UNKNOWN,
    AUTO_INPUT_STATE_TITLE_MENU,
    AUTO_INPUT_STATE_OVERWORLD,
    AUTO_INPUT_STATE_BATTLE,
};

enum
{
    AUTO_FLAG_TRAINER_FORCE_COMP_MOVESETS,
    AUTO_FLAG_TRAINER_DISABLE_PARTY_GENERATION,
    AUTO_FLAG_TRAINER_RANDOM_AI,
    AUTO_FLAG_TRAINER_LVL_5,
    AUTO_FLAG_PLAYER_AUTO_PICK_MOVES,
    AUTO_FLAG_COUNT
};

u16 Rogue_AutomationBufferSize(void);
u16 Rogue_ReadAutomationBuffer(u16 offset);
void Rogue_WriteAutomationBuffer(u16 offset, u16 value);

void Rogue_AutomationInit(void);
void Rogue_AutomationCallback(void);

void Rogue_PushAutomationInputState(u16 state);

bool8 Rogue_AutomationGetFlag(u16 flag);
void Rogue_AutomationSetFlag(u16 flag, bool8 state);


#endif