#ifndef GUARD_SCRIPT_MENU_H
#define GUARD_SCRIPT_MENU_H

extern const u8 *const gStdStrings[];

bool8 ScriptMenu_Multichoice(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress);
u8 ScriptMenu_MultichoiceLength(u8 multichoiceId);
bool8 ScriptMenu_MultichoiceWithDefault(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 defaultChoice);
bool8 ScriptMenu_YesNo(u8 left, u8 top);
bool8 ScriptMenu_MultichoiceGrid(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 columnCount);
bool8 ScriptMenu_ShowPokemonPic(u16 species, u8 x, u8 y, bool8 isObscured);
bool8 (*ScriptMenu_HidePokemonPic(void))(void);
int ConvertPixelWidthToTileWidth(int width);
u8 CreateWindowFromRect(u8 x, u8 y, u8 width, u8 height);
void ClearToTransparentAndRemoveWindow(u8 windowId);
int DisplayTextAndGetWidth(const u8* str, int width);
int ScriptMenu_AdjustLeftCoordFromWidth(int left, int width);
bool16 ScriptMenu_CreatePCMultichoice(void);
void ScriptMenu_DisplayPCStartupPrompt(void);

void ScriptMenu_ScrollingMultichoiceDynamicBegin(u16 capacity);
void ScriptMenu_ScrollingMultichoiceDynamicAppendOption(u8 const* str, u16 value);
void ScriptMenu_ScrollingMultichoiceDynamicEnd(void);
void ScriptMenu_DisplayTextInWindow(const u8* str, u8 x, u8 y, u8 width, u8 height);

#endif //GUARD_SCRIPT_MENU_H
