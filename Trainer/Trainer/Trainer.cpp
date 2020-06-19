#include <iostream>
#include <vector>
#include <Windows.h>
#include <conio.h>
#include "proc.h"


void godmode(uintptr_t, HANDLE);
void infiniteAmmo(uintptr_t, HANDLE);
void noRecoil(uintptr_t, HANDLE);
void giveStuff(unsigned int, uintptr_t, HANDLE);
int main() {

	//get procID of process
	DWORD procId = GetProcId(L"ac_client.exe");
	//get module base address
	uintptr_t modBaseAddr = GetModuleBaseAddress(procId, L"ac_client.exe");
	//get handle to the process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

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
		choice = _getch();
		switch (choice) {
			case 49:
				std::cout << "Godmode Enabled" << std::endl;
				godmode(modBaseAddr, hProcess);
				break;
			case 50:
				std::cout << "Infinite Ammo Enabled" << std::endl;
				infiniteAmmo(modBaseAddr, hProcess);
				break;
			case 51:
				std::cout << "No recoil Enabled" << std::endl;
				noRecoil(modBaseAddr, hProcess);
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
	} while (choice != 'q');

	return 0;
}
void godmode(uintptr_t modBaseAddr, HANDLE hProcess) {
	//Find address of instruction
	uintptr_t cmpInstruction = modBaseAddr + 0x26BE0;
	//Change instruction
	WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\xEB\x2C", 2, nullptr);
}

void infiniteAmmo(uintptr_t modBaseAddr, HANDLE hProcess) {
	//Find address of instruction
	uintptr_t cmpInstruction = modBaseAddr + 0x637E9;
	//Change instruction
	WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\x90\x90", 2, nullptr);
}

void noRecoil(uintptr_t modBaseAddr, HANDLE hProcess) {
	//Find address of instruction
	uintptr_t cmpInstruction = modBaseAddr + 0x63781;
	//Change instruction
	WriteProcessMemory(hProcess, (LPVOID)cmpInstruction, "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 18, nullptr);
}


void giveStuff(unsigned int offset, uintptr_t modBaseAddr, HANDLE hProcess) {
	//resolve the base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = modBaseAddr + 0x10f4f4;

	std::vector<unsigned int> offsets = {offset};
	//resolve the pointer chain using findDMAaddy()
	uintptr_t hpAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);

	//get oldValue from the address
	int oldValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)hpAddr, &oldValue, sizeof(oldValue), nullptr);

	//write to the address
	int newValue = 99 + oldValue;
	WriteProcessMemory(hProcess, (BYTE*)hpAddr, &newValue, sizeof(newValue), nullptr);
}
