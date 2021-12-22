#pragma once
#include <Windows.h>
#include <vector>
#include "Memory.h"

struct Vector3 { float x, y, z; };

namespace Game {

	class Player
	{
	public:
		Vector3 positionHead; //0x0004
		char pad_0010[36]; //0x0010
		Vector3 positionBody; //0x0034
		Vector3 viewAngles; //0x0040
		char pad_004C[172]; //0x004C
		int32_t health; //0x00F8
		char pad_00FC[256]; //0x00FC
		int32_t Kills; //0x01FC
		char pad_0200[4]; //0x0200
		int32_t Deaths; //0x0204
		char pad_0208[29]; //0x0208
		char Name[16]; //0x0225
		char pad_0235[319]; //0x0235
		class Weapon* currentWeapon; //0x0374
		char pad_0378[1228]; //0x0378

		virtual void Function0();
		virtual void Function1();
		virtual void Function2();
		virtual void Function3();
		virtual void Function4();
		virtual void Function5();
		virtual void Function6();
		virtual void Function7();
		virtual void Function8();
		virtual void Function9();
	}; //Size: 0x0844

	class Weapon
	{
	public:
		char pad_0000[12]; //0x0000
		char* type; //0x000C
		class AmmoClip* ammo; //0x0010
		char pad_0014[40]; //0x0014
	}; //Size: 0x003C

	class AmmoClip
	{
	public:
		int32_t clip; //0x0000
		char pad_0004[36]; //0x0004
		int32_t current; //0x0028
		char pad_002C[76]; //0x002C
		int32_t shotsFired; //0x0078
		char pad_007C[968]; //0x007C
	}; //Size: 0x0444



	struct EntityList {
		uintptr_t* baseAddress;
		int* count; // ac_client.exe+0x10F500
		EntityList(uintptr_t address);// address = ac_client.exe+0x10F4F8
		std::vector<Player*> All();
	};

	struct Engine {
		uintptr_t baseAddress;
		Player* player; // ac_client.exe+0x10F4F4
		EntityList* entities; // new

		Engine();
		~Engine();
	};

}