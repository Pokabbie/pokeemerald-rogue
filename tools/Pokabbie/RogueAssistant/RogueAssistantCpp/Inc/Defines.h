#pragma once

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

typedef __int8 s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

#define ARRAY_COUNT(arr) (size_t)(sizeof(arr) / sizeof((arr)[0]))

#define IMGUI_SUPPORT 0