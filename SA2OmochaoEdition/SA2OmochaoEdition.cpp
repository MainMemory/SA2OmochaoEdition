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

// void __usercall sub_4273B0(NJS_VECTOR *a1@<eax>, NJS_VECTOR *a2@<edx>, float *a3@<ecx>)
void *sub_4273B0_ptr = (void*)0x4273B0;
void sub_4273B0(NJS_VECTOR *a1, NJS_VECTOR *a2, float *a3)
{
	__asm
	{
		mov eax, [a1]
		mov edx, [a2]
		mov ecx, [a3]
		call sub_4273B0_ptr
	}
}

// void __usercall sub_4274E0(float *a1@<ebx>, signed int a2)
void *sub_4274E0_ptr = (void*)0x4274E0;
void sub_4274E0(float *a1, signed int a2)
{
	__asm
	{
		mov ebx, [a1]
		push [a2]
		call sub_4274E0_ptr
		add esp, 4
	}
}

// void __usercall sub_427680(float *a1@<ebx>, signed int a2)
void *sub_427680_ptr = (void*)0x427680;
void sub_427680(float *a1, signed int a2)
{
	__asm
	{
		mov ebx, [a1]
		push [a2]
		call sub_427680_ptr
		add esp, 4
	}
}

// void __usercall(int a1@<esi>, int a2, char a3, char a4)
static const void *const PlaySoundProbablyPtr = (void*)0x437260;
static inline void PlaySoundProbably(int a1, int a2, int a3, int a4)
{
	__asm
	{
		push [a4]
		push [a3]
		push [a2]
		mov esi, [a1]
		call PlaySoundProbablyPtr
		add esp, 12
	}
}

FunctionPointer(CharObj1 *, AllocateEntityData1, (), 0x470B40);
ObjectMaster *LoadOmochao(NJS_VECTOR *position)
{
	ObjectMaster *obj = LoadObject(Omochao_Main, 2, "ObjectMessenger");
	if (obj)
	{
		CharObj1 *v7 = AllocateEntityData1();
		if (v7)
		{
			obj->Data1 = v7;
			void *v10 = AllocateEntityData2();
			if (v10)
			{
				obj->field_38 = v10;
				v7->Position = *position;
				v7->Scale.x = (float)(rand() % StageMessageCount); // select a random hint from the level's hint message file
				v7->Scale.y = 15;
				v7->Scale.z = 0;
				return obj;
			}
			else
				DeleteObject_(obj);
		}
		else
			DeleteObject_(obj);
	}
	return nullptr;
}

ObjectMaster *LoadOmochao2(float x, float y, float z)
{
	NJS_VECTOR pos = { x, y, z };
	return LoadOmochao(&pos);
}

short maxringloss;
VoidFunc(sub_429000, 0x429000);
VoidFunc(sub_429710, 0x429710);
FastcallFunctionPointer(float, sub_42AAB0, (Angle ang), 0x42AAB0);
FastcallFunctionPointer(float, sub_42AC30, (Angle ang), 0x42AC30);
DataPointer(float*, dword_1A557FC, 0x1A557FC);
DataPointer(Angle, GravityAngle_X, 0x1DE949C);
DataPointer(NJS_VECTOR, Gravity, 0x1DE94A0);
DataPointer(Angle, GravityAngle_Z, 0x1DE94AC);
void __cdecl DropOmochao(int playerNum)
{
	double v3 = (double)rand();
	int id = playerNum & 1;
	short rings = RingCount[id];
	if (rings > 0)
	{
		if (rings > maxringloss)
			rings = maxringloss;
		AddRings(playerNum, -rings);
		NJS_VECTOR *pos = &MainCharObj1[playerNum]->Position;
		int v5 = 0;
		do
		{
			ObjectMaster *scatterObject = LoadOmochao(pos);
			if (scatterObject)
			{
				CharObj1 *v7 = scatterObject->Data1;
				v7->Rotation.y = (signed int)(((double)(v5 / rings) + v3 * 0.000030517578125 * 360.0) * 182.0444488525391);
				NJS_VECTOR v61;
				v61.y = 1.8f;
				v61.x = sub_42AAB0(v7->Rotation.y) * 0.4000000059604645f;
				v61.z = sub_42AC30(v7->Rotation.y) * 0.4000000059604645f;
				sub_429710();
				float *v20 = dword_1A557FC;
				if (dword_1A557FC)
				{
					memset(dword_1A557FC, 0, 0x30u);
					*v20 = 1.0;
					v20[5] = 1.0;
					v20[10] = 1.0;
				}
				if (GravityAngle_X)
				{
					sub_427680(v20, GravityAngle_X);
				}
				if (GravityAngle_Z)
				{
					sub_4274E0(v20, GravityAngle_Z);
				}
				sub_4273B0(&v61, (NJS_VECTOR*)scatterObject->field_38, v20);
				sub_429000();
			}
			v5 += 360;
			--rings;
		} while (rings);
		PlaySoundProbably(32788, 0, 0, 0);
		return;
	}
	char v9 = MainCharObj2[playerNum]->CharID;
	if (v9 != Characters_MechEggman && v9 != Characters_MechTails)
	{
		KillPlayer(playerNum);
	}
}

static void __declspec(naked) ReplaceChaosDrive()
{
	__asm
	{
		push ebx // a1

		// Call your __cdecl function here:
		call LoadOmochao

		pop ebx // a1
		retn
	}
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
		if (settings->getBool("", "ReplaceDroppedRings"))
		{
			WriteJump(DropRings, DropOmochao);
			maxringloss = settings->getInt("", "MaxDroppedOmochao", 20);
		}
		if (settings->getBool("", "ReplaceEnemyDrops"))
		{
			WriteCall((void*)0x47ADC0, ReplaceChaosDrive);
			WriteCall((void*)0x47ADE5, LoadOmochao2);
		}
		if (settings->getBool("", "ReplaceChaoKey"))
			WriteCall((void*)0x6DE63B, ReplaceChaosDrive);
		delete settings;
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer };
}