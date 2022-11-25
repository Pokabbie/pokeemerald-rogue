#ifdef ROGUE_FEATURE_AUTOMATION

u16 Rogue_AutomationBufferSize(void);
u16 Rogue_ReadAutomationBuffer(u16 offset);
void Rogue_WriteAutomationBuffer(u16 offset, u16 value);

void Rogue_AutomationInit(void);
void Rogue_AutomationCallback(void);

bool8 Rogue_AutomationSkipTrainerPartyCreate(void);

#endif