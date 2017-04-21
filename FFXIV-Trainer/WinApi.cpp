#include "WinApi.h"


MODULEENTRY32 GetModule( DWORD dwProcID, char *moduleName )
{
	MODULEENTRY32 modEntry = { 0 };
	bool bFound = false;
	HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID );
	if( hSnapshot != INVALID_HANDLE_VALUE ) {
		modEntry.dwSize = sizeof( MODULEENTRY32 );
		if( Module32First( hSnapshot, &modEntry ) ) {
			do {
				if( !strcmp( modEntry.szModule, moduleName ) ) {
					bFound = true;
					break;
				}
			} while( Module32Next( hSnapshot, &modEntry ) );
		}
		CloseHandle( hSnapshot );
	}
	if( bFound ){
		return modEntry;
	} else {
		modEntry.th32ModuleID = 0; // (This member is no longer used, and is always set to one.) -- using to check if it actually found the module. 
		return modEntry;
	}
	
}