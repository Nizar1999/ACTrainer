#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"


int main() {
	//get procID of process
	DWORD procId = GetProcId(L"ac_client.exe");
	//get module base address
	uintptr_t modBaseAddr = GetModuleBaseAddress(procId, L"ac_client.exe");
	//get handle to the process
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	//resolve the base address of the pointer chain
	uintptr_t dynamicPtrBaseAddr = modBaseAddr + 0x10f4f4;
	std::cout << "Dyanmic ptr base addr: 0x" << std::hex << dynamicPtrBaseAddr << '\n';
	//resolve the pointer chain using findDMAaddy()
	std::vector<unsigned int> offsets = {0xf8};
	uintptr_t hpAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, offsets);
	std::cout << "hp addr: 0x" << std::hex << hpAddr << '\n';
	//print old hp value
	int hpValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)hpAddr, &hpValue, sizeof(hpValue), nullptr);
	std::cout << "Old HP value: " << std::dec << hpValue << '\n';
	//write to the address
	int newHpValue = 1337;
	WriteProcessMemory(hProcess, (BYTE*)hpAddr, &newHpValue, sizeof(newHpValue), nullptr);
	//print new hp value
	std::cout << "New HP value: " << std::dec << newHpValue << '\n';

	return 0;
}