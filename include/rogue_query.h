#ifndef ROGUE_QUERY_H
#define ROGUE_QUERY_H

//#define ROGUE_QUERY_MAX 128
//extern u16 gRogueQueryBufferSize;
//extern bool8 gRogueQuerySpeciesState[];
//extern u16 gRogueQueryBuffer[];



void RogueQuery_Clear(void);
void RogueQuery_CollapseBuffer(void);
u16* RogueQuery_BufferPtr(void);
u16 RogueQuery_BufferSize(void);


void RogueQuery_SpeciesOfType(u8 type);
void RogueQuery_SpeciesOfTypes(u8 type1, u8 type2);
void RogueQuery_EggSpeciesOnly(void);

#endif