#ifndef BAKE_HELPERS_H
#define BAKE_HELPERS_H

#ifndef __attribute__
#define __attribute__(...)
#endif

#ifndef ANIMCMD_FRAME
#define ANIMCMD_FRAME(...) {}
#endif

#ifndef ANIMCMD_LOOP
#define ANIMCMD_LOOP(...) {}
#endif

#ifndef ANIMCMD_JUMP
#define ANIMCMD_JUMP(...) {}
#endif

#ifndef ANIMCMD_END
#define ANIMCMD_END {}
#endif

#include "gba/gba.h"
#include "sprite.h"
#include "constants/global.h"
#include "constants/rogue.h"

#include "constants/abilities.h"
#include "constants/battle.h"
#include "constants/battle_move_effects.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "constants/items.h"


#include "global.h"
#include "item.h"
#include "rogue.h"
#include "rogue_baked.h"

#ifdef ROGUE_EXPANSION
#include "BakeHelpers_EX.h"
#else
#include "BakeHelpers_Vanilla.h"
#endif

#endif // BAKE_HELPERS_H