#pragma once
#include "pch.h"

typedef StructuredDataDef* (*StructuredDataDef_GetAsset_t)(const char* filename, unsigned int maxSize);
typedef const char* (*SL_ConvertToString_t)(unsigned int stringValue);
typedef unsigned int (*SL_GetStringForInt_t)(int);
typedef unsigned int (*SL_FindString_t)(const char* str);
typedef bool (*LiveStorage_PlayerDataSetIntByNameArray_t)(int controllerIndex, const unsigned int* navStrings, int navStringCount, int value, StatsGroup statsGroup);
typedef StringTable* (*StringTable_GetAsset_t)(const char* fileName, const StringTable** tablePtr);
typedef const char* (*StringTable_GetColumnValueForRow_t)(const StringTable* table, const int row, const int column);
typedef std::uint8_t* (*LiveStorage_GetPersistentDataBuffer_t)(int controllerIndex);
typedef bool (*LiveStorage_PlayerDataSetReservedInt_t)(std::uint8_t* persistentData, const char* lookupString, int value, std::uint8_t* modifiedFlags, StatsGroup statsGroup);
typedef bool (*LiveStorage_PlayerDataSetIntByName_t)(int controllerIndex, unsigned int name, int value, StatsGroup statsGroup);
typedef void (*Cbuf_AddText_t)(int localClientNum, const char* text);


extern StructuredDataDef_GetAsset_t StructuredDataDef_GetAsset;
extern SL_ConvertToString_t SL_ConvertToString;
extern SL_GetStringForInt_t SL_GetStringForInt;
extern SL_FindString_t SL_FindString;
extern LiveStorage_PlayerDataSetIntByNameArray_t LiveStorage_PlayerDataSetIntByNameArray;
extern StringTable_GetAsset_t StringTable_GetAsset;
extern StringTable_GetColumnValueForRow_t StringTable_GetColumnValueForRow;
extern LiveStorage_GetPersistentDataBuffer_t LiveStorage_GetPersistentDataBuffer;
extern LiveStorage_PlayerDataSetReservedInt_t LiveStorage_PlayerDataSetReservedInt;
extern LiveStorage_PlayerDataSetIntByName_t LiveStorage_PlayerDataSetIntByName;
extern Cbuf_AddText_t Cbuf_AddText;


extern void unlock_all_extinction();
extern void unlock_all_multiplayer();
extern void unlock_all_challenges();
extern void unlock_past_title_backgrounds();

extern void keyBinds();