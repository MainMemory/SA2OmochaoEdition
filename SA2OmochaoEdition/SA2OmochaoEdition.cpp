// SA2OmochaoEdition.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ctime>
#include "IniFile.hpp"
#include "SA2ModLoader.h"

bool allmsg;
DataPointer(int, StageMessageCount, 0xB5D200);
ObjectFunc(sub_6BE2E0, 0x6BE2E0);
void Omochao_HijackAction(ObjectMaster *obj)
{
	obj->Data1->Action = 6; // force Omochao into the talking action
	if (allmsg) 
		obj->Data1->Scale.x = (float)(rand() % StageMessageCount); // select a random hint from the level's hint message file
	sub_6BE2E0(obj);
}

void Omochao_ForceFollow(ObjectMaster *obj)
{
	obj->Data1->NextAction |= 3; // force Omochao to follow the player
	*(float*)((char*)obj->Data2 + 0x30) = 1; // force propeller to full size
	obj->MainSub = Omochao_HijackAction;
	Omochao_HijackAction(obj);
}

void Omochao_AutoActivate(ObjectMaster *obj)
{
	obj->Data1->NextAction |= 3; // force Omochao to follow the player
	obj->Data1->Action = 5; // force Omochao into the talking action
	if (allmsg)
		obj->Data1->Scale.x = (float)(rand() % StageMessageCount); // select a random hint from the level's hint message file
	obj->MainSub = sub_6BE2E0;
	sub_6BE2E0(obj);
}

void Omochao_RandomMessage(ObjectMaster *obj)
{
	obj->Data1->Scale.x = (float)(rand() % StageMessageCount); // select a random hint from the level's hint message file
	obj->MainSub = sub_6BE2E0;
	sub_6BE2E0(obj);
}

FunctionPointer(void *, AllocateEntityData2, (), 0x470B70);
void RingToOmochao(ObjectMaster *obj)
{
	obj->Data1->Action = 0;
	obj->Data1->Scale.x = (float)(rand() % StageMessageCount); // select a random hint from the level's hint message file
	obj->Data1->Scale.y = 15;
	obj->Data1->Scale.z = 0;
	obj->field_38 = AllocateEntityData2();
	Omochao_Main(obj);
}

DataPointer(char **, StageMessageList, 0x1AEFD6C);
DataPointer(char **, SysMessageList, 0x1AEFD70);
DataPointer(int, SysMessageCount, 0xB5D204);
FunctionPointer(char **, LoadMessageFile, (const char *filename, int *count), 0x6F1E80);
VoidFunc(ReleaseMessageLists, 0x6BD5D0);
int LoadMessageLists_All(const char *filename)
{
	if (!StageMessageList && !SysMessageList)
	{
		if (!strcmp("mh0066", filename) || !strcmp("mh0068", filename)) // keep the FinalHazard and tutorial message files
			StageMessageList = LoadMessageFile(filename, &StageMessageCount);
		else
			StageMessageList = LoadMessageFile("mhall", &StageMessageCount);
		SysMessageList = LoadMessageFile("mhsys", &SysMessageCount);
		if (StageMessageList && SysMessageList)
			return SysMessageCount + StageMessageCount;
		ReleaseMessageLists();
	}
	return -1;
}

__declspec(naked) void LoadMessageLists_r()
{
	__asm
	{
		push eax
		call LoadMessageLists_All
		add esp, 4
		retn
	}
}

void *loc_43C978 = (void*)0x43C978;
__declspec(naked) void ShowBossHint()
{
	*(short*)0x1934BE8 = 1;
	__asm {		jmp loc_43C978	}
}

extern "C"
{
	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		const IniFile *settings = new IniFile(std::string(path) + "\\config.ini");
		if (settings->getBool("", "LoadAllMessages"))
		{
			WriteJump((void*)0x6BD650, LoadMessageLists_r);
			allmsg = true;
		}
		if (settings->getBool("", "TalkForever"))
		{
			WriteData((ObjectFuncPtr*)0x6C0879, Omochao_ForceFollow);
			WriteData((char*)0x6BE3EE, (char)0xEBu); // disable the limit on how long Omochao can follow you
		}
		else if (settings->getBool("", "AutoActivate"))
			WriteData((ObjectFuncPtr*)0x6C0879, Omochao_AutoActivate);
		else if (allmsg)
			WriteData((ObjectFuncPtr*)0x6C0879, Omochao_RandomMessage);
		if (settings->getBool("", "TalkOverEachOther"))
			WriteData((void*)0x6BEA14, 0x90u, 6);
		if (settings->getBool("", "AlwaysShowBossHint"))
			WriteJump((void*)0x43C970, ShowBossHint);
		if (settings->getBool("", "ReplaceRings"))
		{
			srand(_time32(nullptr));
			WriteJump((void*)0x6C0F80, RingToOmochao); // turn rings into Omochao
			WriteData((short*)0x6C605B, (short)0xE990); // prevent ring groups from crashing the game
		}
		delete settings;
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}