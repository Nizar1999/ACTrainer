#include <iostream>
#include <vector>
#include <conio.h>
#include <future>
#include "proc.h"

void godmode(uintptr_t, HANDLE, int);
void infiniteAmmo(uintptr_t, HANDLE, bool);
void noRecoil(uintptr_t, HANDLE, bool);
void giveStuff(unsigned int, uintptr_t, HANDLE);
bool godmodeEnabled = false, infiniteAmmoEnabled = false, NoRecoilEnabled = false;
const unsigned int PLAYEROFFSET = 0x10f4f4;

int getch_noblock() {
	if (_kbhit())
		return _getch();
	else
		return -1;
}

int main() {

	//get procID of process
	DWORD procId = GetProcId(L"ac_client.exe");
	//get module base address
	uintptr_t modBaseAddr = GetModuleBaseAddress(procId, L"ac_client.exe");
	//get handle to the process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	uintptr_t hpAddr = FindDMAAddy(hProcess, (modBaseAddr + PLAYEROFFSET), { 0xf8 }); //find HP addr for god mode
	if (hProcess == 0) {
		std::cout << "Could not get handle to Assault Cube" << std::endl;
		return 0;
	}

	std::cout << "Assault Cube Trainer v2.0" << std::endl;
	std::cout << "1 - Godmode\n"
			  << "2 - Infinite Ammo\n"
			  << "3 - No recoil\n"
			  << "4 - Give Grenades\n"
			  << "5 - Give Armor\n"
		      << "q - quit\n" << std::endl;
	int choice;
	do {
		choice = getch_noblock();
		switch (choice) {
			case 49:
				if (!godmodeEnabled)
					std::cout << "Godmode Enabled" << std::endl;
				else
					std::cout << "Godmode disabled" << std::endl;
				godmodeEnabled = !godmodeEnabled;
				break;
			case 50:
				infiniteAmmo(modBaseAddr, hProcess, infiniteAmmoEnabled);
				infiniteAmmoEnabled = !infiniteAmmoEnabled;
				break;
			case 51:
				noRecoil(modBaseAddr, hProcess, NoRecoilEnabled);
				NoRecoilEnabled = !NoRecoilEnabled;
				break;
			case 52:
				std::cout << "Grenades given" << std::endl;
				giveStuff(0x158, modBaseAddr, hProcess); //GrenadeAmmo offset
				break;
			case 53:
				std::cout << "Armor given" << std::endl;
				giveStuff(0xfc, modBaseAddr, hProcess); //Armor offset
				break;
		}
		if (godmodeEnabled) {
			std::async(godmode, hpAddr, hProcess, 1337);
		}
	} while (choice != 'q');

	return 0;
}
void godmode(uintptr_t hpAddr, HANDLE hProcess, int newValue) {
	WriteProcessMemory(hProcess, (BYTE*)hpAddr, &newValue, sizeof(newValue), nullptr);
}

void infiniteAmmo(uintptr_t modBaseAddr, HANDLE hProcess, bool enabled) {
	uintptr_t cmpInstruction = modBaseAddr + 0x637E9;
	if (enabled) {
		WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\xFF\x0E", 2, nullptr);
		std::cout << "Infinite Ammo Disabled\n";
	}
	else {
		WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\x90\x90", 2, nullptr);
		std::cout << "Infinite Ammo Enabled\n";
	}
}

void noRecoil(uintptr_t modBaseAddr, HANDLE hProcess, bool enabled) {
	uintptr_t cmpInstruction = modBaseAddr + 0x63781;
	if (enabled) {
		WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\x8B\x16\x8B\x52\x14\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2\x8B\x46\x08", 18, nullptr);
		std::cout << "NoRecoil Disabled\n";
	}
	else {
		WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 18, nullptr);
		std::cout << "NoRecoil Enabled\n";
	}
}


void giveStuff(unsigned int offset, uintptr_t modBaseAddr, HANDLE hProcess) {
	//resolve the base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = modBaseAddr + PLAYEROFFSET;

	std::vector<unsigned int> offsets = {offset};
	//resolve the pointer chain using findDMAaddy()
	uintptr_t stuffAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);

	//get oldValue from the address
	int oldValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)stuffAddr, &oldValue, sizeof(oldValue), nullptr);

	//write to the address
	int newValue = 99 + oldValue;
	WriteProcessMemory(hProcess, (BYTE*)stuffAddr, &newValue, sizeof(newValue), nullptr);
}
