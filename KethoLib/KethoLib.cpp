#include "KethoLib.h"


/// <summary>
///     Obtem o ProcessID de um processo
/// </summary>
/// <param name="processo">Nome do processo wchar_t para buscar</param>
/// <returns>Retorna o ProcessID do processo</returns>
DWORD KethoLib::getProcessInfo(const wchar_t* processo) {
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);


    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    do {
        _tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
        if (wcscmp(pe32.szExeFile, processo) == 0) {
            CloseHandle(hProcessSnap);
            return pe32.th32ProcessID;
        }

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return 0;

}

/// <summary>
///     Varre os modulos carregados por um processo para buscar um module base
///     Caso seja o pr�prio processo � devolvido seu pr�prio module base
/// </summary>
/// <param name="procId">Id do processo para buscar</param>
/// <param name="modName">Nome do processo wchar_t para buscar</param>
/// <returns>O Endere�o do Module Base</returns>
uintptr_t KethoLib::getModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}


/// <summary>
///     Vai abrir um processo com permiss�o de acesso total e retornar o HANDLE dele
/// </summary>
/// <param name="processName">Nome do processo wchar_t para buscar</param>
/// <returns>Handle para o processo</returns>
HANDLE KethoLib::abrirProcessoPeloNome(const wchar_t* processName) {
   return OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->getProcessInfo(processName));
}



/// <summary>
///     Essa fun��o L� dados de OPCODES da mem�ria baseado no MODBASE+RVA
/// </summary>
/// <param name="hProcess">Handle para o processo na mem�ria</param>
/// <param name="moduleBase">Endere�o base do modulo na mem�ria no qual ser� escrito dados</param>
/// <param name="x64PatchAddress">Endere�os RVA para os OPCODES em mem�ria</param>
/// <param name="bbyte">Array onde sera armazenado os dados dos opcodes lidos da mem�ria</param>
void KethoLib::readMemoryOpcodes(HANDLE hProcess, uintptr_t moduleBase, std::vector<unsigned int>x64PatchAddress, std::vector<uint8_t>* bbyte) {
    uintptr_t val = 0;
    for (unsigned int x : x64PatchAddress) {
        val = moduleBase + x;
        uint8_t local = 0;
        ReadProcessMemory(hProcess, (void*)val, &local, sizeof(uint8_t), 0);
        (*bbyte).push_back(local);
    }
}



/// <summary>
///     Essa fun��o � respons�vel por escrever dados na mem�ria de um processo
/// </summary>
/// <param name="hProcess">Handle para o processo na mem�ria</param>
/// <param name="moduleBase">Endere�o base do modulo na mem�ria no qual ser� escrito dados</param>
/// <param name="x64PatchAddress">Endere�os RVA para os OPCODES em mem�ria</param>
/// <param name="x64AssemblyOpcodes">Novos OPCODES que ser�o substituidos nas posi��es determinadas pelos MODBASE+RVA</param>
void KethoLib::writeMemoryOpcodes(HANDLE hProcess, uintptr_t moduleBase, std::vector<unsigned int>x64PatchAddress, std::vector<unsigned int>x64AssemblyOpcodes) {
    uintptr_t val = 0;
    for (int i = 0; i < static_cast<int>(x64AssemblyOpcodes.size()); ++i) {
        val = moduleBase + x64PatchAddress.at(i);
        WriteProcessMemory(hProcess, (void*)val, &x64AssemblyOpcodes.at(i), sizeof(unsigned int), 0);
    }
}
