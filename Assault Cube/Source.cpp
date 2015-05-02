#include <windows.h>

//ADDRESSES AND ORIGINAL OPCODES

//004637E9   FF0E             DEC DWORD PTR DS:[ESI]                   ; Ammo

//00463781   8B16             MOV EDX,DWORD PTR DS:[ESI]               ; Start
//00463783   8B52 14          MOV EDX, DWORD PTR DS : [EDX + 14]
//00463786   50               PUSH EAX
//00463787   8D4C24 1C        LEA ECX, DWORD PTR SS : [ESP + 1C]
//0046378B   51               PUSH ECX
//0046378C   8BCE             MOV ECX, ESI
//0046378E   FFD2             CALL EDX;                                ; Finish

//00429D1F   297B 04          SUB DWORD PTR DS:[EBX+4],EDI

//00463710   389A 28010000    CMP BYTE PTR DS:[EDX+128],BL             ; Automatic

//00463378   FF08             DEC DWORD PTR DS:[EAX]                   ; Grenade

//004637E4   890A             MOV DWORD PTR DS:[EDX],ECX               ; RAPID FIRE



//Defining Addresses to Use

#define AMMO_ADDRESS 0x004637E9
#define RECOIL_ADDRESS 0x00463781
#define HEALTH_ADDRESS 0x00429D1F
#define SEMI_AUTOMATIC 0x00463710
#define GRENADE_ADDRESS 0x00463378
#define RAPID_FIRE 0x4637E4

//Creating Byte Array to NOP Functions 

BYTE NOP [29] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
BYTE INC[2] = { 0xFF, 0x00 };

//Infinite Ammo Function

void Ammo()
{
	DWORD dwOld;

	VirtualProtect((LPVOID)AMMO_ADDRESS, 2, PAGE_EXECUTE_READWRITE, &dwOld);
	memcpy((void *)AMMO_ADDRESS, NOP, 2);
	VirtualProtect((LPVOID)AMMO_ADDRESS, 2, dwOld, &dwOld);
}

//No Recoil Function

void Recoil()
{
	DWORD DwOld;

	VirtualProtect((LPVOID)RECOIL_ADDRESS,	15, PAGE_EXECUTE_READWRITE, &DwOld);
	memcpy((void *)RECOIL_ADDRESS, NOP, 15);
	VirtualProtect((LPVOID)RECOIL_ADDRESS, 15, DwOld, &DwOld);
}

//Health Freeze Function

void Health()
{
	DWORD DWOld;

	VirtualProtect((LPVOID)HEALTH_ADDRESS, 3, PAGE_EXECUTE_READWRITE, &DWOld);
	memcpy((void *)HEALTH_ADDRESS, NOP, 3);
	VirtualProtect((LPVOID)HEALTH_ADDRESS, 3, DWOld, &DWOld);
}

//Convert Semi to Auto

void SemiAuto()
{
	DWORD dWlod;

	VirtualProtect((LPVOID)SEMI_AUTOMATIC, 6, PAGE_EXECUTE_READWRITE, &dWlod);
	memcpy((void *)SEMI_AUTOMATIC, NOP, 6);
	VirtualProtect((LPVOID)SEMI_AUTOMATIC, 6, dWlod, &dWlod);
}

//Increasing Grenade Count

void Grenade()
{
	DWORD DwLod;

	VirtualProtect((LPVOID)GRENADE_ADDRESS, 2, PAGE_EXECUTE_READWRITE, &DwLod);
	memcpy((void *)GRENADE_ADDRESS, INC, 2);
	VirtualProtect((LPVOID)GRENADE_ADDRESS, 2, DwLod, &DwLod);
}

//Rapid Fire
void RapidFire()
{
	DWORD dWlod;
	
	VirtualProtect((LPVOID)RAPID_FIRE, 2, PAGE_EXECUTE_READWRITE, &dWlod);
	memcpy((void *)RAPID_FIRE, NOP, 2);
	VirtualProtect((LPVOID)RAPID_FIRE, 2, dWlod, &dWlod);

}


//DLL Main

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		MessageBoxA(NULL, "Mods Sucessfully Added!\n [!] Infinite Ammo\n [!] No Recoil\n [!] God Mode\n [!] All Weapons Auto\n [!] Infinite Grenades\n [!] Rapid Fire\n", "Injected!", MB_OK);
		Ammo();
		Recoil();
		Health();
		SemiAuto();
		Grenade();
		RapidFire();
	}
	return true;
}
