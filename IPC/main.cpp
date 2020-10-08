#include <iostream>
#include <windows.h>
#include <memoryapi.h>
#include <string>

TCHAR szName[] = TEXT("MemoryCriticalSection");

int constexpr maxProcesses = 5;

typedef struct {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
} PROCESS_INFO;

std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

int main() {

	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CRITICAL_SECTION), szName);

	if (hMapFile == NULL) {
		std::cout << "Cannot create shared memory critical section" << std::endl;
		return -1;
	}

	auto cs = (CRITICAL_SECTION*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CRITICAL_SECTION));
	if (cs == NULL) {
		return -1;
	}

	InitializeCriticalSection(cs);
	std::cout << "Section is ready" << std::endl;

	PROCESS_INFO pInfos[maxProcesses];
	for (int i = 0; i < maxProcesses; i++) {
		ZeroMemory(&pInfos[i], sizeof(PROCESS_INFO));
		pInfos[i].si.cb = sizeof(STARTUPINFO);
		if (!CreateProcess(std::wstring(ExePath() + L"\\Process.exe").c_str(), NULL, 0, 0, TRUE, 0, 0, NULL, &pInfos[i].si, &pInfos[i].pi)) {
			std::cout << "Failed to create process Error:" << GetLastError() << std::endl;
		}
	}

	for (int i = 0; i < maxProcesses; i++) {
		DWORD res = WaitForSingleObject(pInfos[i].pi.hProcess, INFINITE);
		CloseHandle(pInfos[i].pi.hProcess);
		CloseHandle(pInfos[i].pi.hThread);
	}

	DeleteCriticalSection(cs);
	UnmapViewOfFile(cs);
	CloseHandle(hMapFile);

	return 0;
}