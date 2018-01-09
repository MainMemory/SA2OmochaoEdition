// SA2OmochaoEdition.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <ctime>
#include "IniFile.hpp"
#include "SA2ModLoader.h"

ObjectFunc(sub_6BE2E0, 0x6BE2E0);
void Omochao_HijackAction(ObjectMaster *obj)
{
	obj->Data1->Action = 6; // force Omochao into the talking action
	sub_6BE2E0(obj);
}

void Omochao_ForceFollow(ObjectMaster *obj)
{
	obj->Data1->NextAction |= 3; // force Omochao to follow the player
	obj->Data1->Action = 6; // force Omochao into the talking action
	*(float*)((char*)obj->Data2 + 0x30) = 1; // force propeller to full size
	obj->MainSub = Omochao_HijackAction;
	sub_6BE2E0(obj);
}

void Omochao_AutoActivate(ObjectMaster *obj)
{
	obj->Data1->NextAction |= 3; // force Omochao to follow the player
	obj->Data1->Action = 5; // force Omochao into the talking action
	obj->MainSub = sub_6BE2E0;
	sub_6BE2E0(obj);
}

FunctionPointer(void *, AllocateEntityData2, (), 0x470B70);
void RingToOmochao(ObjectMaster *obj)
{
	obj->Data1->Action = 0;
	obj->Data1->Scale.x = (float)(rand() % *(int*)0xB5D200); // select a random hint from the level's hint message file
	obj->Data1->Scale.y = 15;
	obj->Data1->Scale.z = 0;
	obj->field_38 = AllocateEntityData2();
	Omochao_Main(obj);
}

extern "C"
{
	__declspec(dllexport) void Init(const char *path, const HelperFunctions &helperFunctions)
	{
		const IniFile *settings = new IniFile(std::string(path) + "\\config.ini");
		if (settings->getBool("", "TalkForever"))
		{
			WriteData((ObjectFuncPtr*)0x6C0879, Omochao_ForceFollow);
			WriteData((char*)0x6BE3EE, (char)0xEBu); // disable the limit on how long Omochao can follow you
		}
		else if (settings->getBool("", "AutoActivate"))
			WriteData((ObjectFuncPtr*)0x6C0879, Omochao_AutoActivate);
		if (settings->getBool("", "TalkOverEachOther"))
			WriteData((void*)0x6BEA14, 0x90u, 6);
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