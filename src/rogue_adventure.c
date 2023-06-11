#include "global.h"
//#include "constants/abilities.h"
//#include "constants/battle.h"
//#include "constants/event_objects.h"
//#include "constants/heal_locations.h"
//#include "constants/hold_effects.h"
//#include "constants/items.h"
//#include "constants/layouts.h"
//#include "constants/rogue.h"
//#include "constants/weather.h"
//#include "data.h"
//#include "gba/isagbprint.h"
//
//#include "battle.h"
//#include "battle_setup.h"
//#include "berry.h"
//#include "event_data.h"
//#include "graphics.h"
//#include "item.h"
//#include "load_save.h"
//#include "main.h"
//#include "money.h"
//#include "overworld.h"
//#include "party_menu.h"
//#include "palette.h"
//#include "play_time.h"
//#include "player_pc.h"
//#include "pokemon.h"
//#include "pokemon_icon.h"
//#include "pokemon_storage_system.h"
//#include "random.h"
//#include "rtc.h"
//#include "safari_zone.h"
//#include "script.h"
//#include "siirtc.h"
//#include "strings.h"
//#include "string_util.h"
//#include "text.h"

//#include "rogue.h"
#include "rogue_adventure.h"
//#include "rogue_automation.h"
//#include "rogue_adventurepaths.h"
//#include "rogue_campaign.h"
//#include "rogue_charms.h"
#include "rogue_controller.h"
//#include "rogue_followmon.h"
//#include "rogue_popup.h"
//#include "rogue_query.h"
//#include "rogue_quest.h"

#define TODO_ADVENTURE_TYPE 0

const struct RogueAdventureSettings* Rogue_GetAdventureSettings()
{
    return &gRogueAdventureSettings[TODO_ADVENTURE_TYPE];
}

const struct RogueAdventurePhase* Rogue_GetAdventurePhase()
{
    const struct RogueAdventureSettings* settings = Rogue_GetAdventureSettings();
    u8 phase = min(gRogueRun.currentDifficulty, settings->phaseCount);

    return &settings->phases[phase];
}