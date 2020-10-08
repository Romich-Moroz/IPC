#include <iostream>
#include <windows.h>
#include <memoryapi.h>


TCHAR szName[] = TEXT("MemoryCriticalSection");

int constexpr loopCycles = 5;

int main() { //arg0

	HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, szName);

	if (hMapFile == NULL) {
		std::cout << "Cannot open shared memory critical section" << std::endl;
		return -1;
	}

	auto cs = (CRITICAL_SECTION*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CRITICAL_SECTION));
	if (cs == NULL) {
		std::cout << "Cannot map view critical section" << std::endl;
		CloseHandle(hMapFile);
		return -1;
	}
	int procId = GetCurrentProcessId();
	
	for (int i = 0; i < loopCycles; i++) {
		while (!TryEnterCriticalSection(cs)) {
			Sleep(10);
		}
		std::cout << "Process " << procId << " entered" << std::endl;

		std::cout << "Process " << procId <<" is on "<< i <<" loop "  << std::endl;

		std::cout << "Process " << procId << " left" << std::endl;
		LeaveCriticalSection(cs);
		Sleep(10);
		
	}

	UnmapViewOfFile(cs);
	CloseHandle(hMapFile);
	return 0;
}