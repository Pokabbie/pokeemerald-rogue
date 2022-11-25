#ifdef ROGUE_FEATURE_AUTOMATION

enum
{
    AUTO_INPUT_STATE_UNKNOWN,
    AUTO_INPUT_STATE_TITLE_MENU,
    AUTO_INPUT_STATE_OVERWORLD,
    AUTO_INPUT_STATE_BATTLE,
};

u16 Rogue_AutomationBufferSize(void);
u16 Rogue_ReadAutomationBuffer(u16 offset);
void Rogue_WriteAutomationBuffer(u16 offset, u16 value);

void Rogue_AutomationInit(void);
void Rogue_AutomationCallback(void);

void Rogue_PushAutomationInputState(u16 state);

bool8 Rogue_AutomationSkipTrainerPartyCreate(void);
bool8 Rogue_AutomationAutoPickBattleMove(void);
bool8 Rogue_AutomationForceRandomAI(void);


#endif