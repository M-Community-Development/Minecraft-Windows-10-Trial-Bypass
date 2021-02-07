#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <vector>

/// <summary>
///		Essa classe � respons�vel por trabalhar com OPCODES e Memory Assembly
///		Ela � capaz de recuperar OPCODES da mem�ria ou Substituir Opcodes da mem�ria
///		Ela � capaz de obter dados como PID de um processo e ModuleBase
/// </summary>
class KethoLib {

public:
    DWORD getProcessInfo(const wchar_t* processo);
	uintptr_t getModuleBaseAddress(DWORD procId, const wchar_t* modName);
	HANDLE abrirProcessoPeloNome(const wchar_t* processName);
	void readMemoryOpcodes(HANDLE hProcess, uintptr_t moduleBase, std::vector<unsigned int>x64PatchAddress, std::vector<uint8_t>* bbyte);
	void writeMemoryOpcodes(HANDLE hProcess, uintptr_t moduleBase, std::vector<unsigned int>x64PatchAddress, std::vector<unsigned int>x64AssemblyOpcodes);
};
