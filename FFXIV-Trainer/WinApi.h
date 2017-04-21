#pragma once
#ifndef WINAPI_H
#define WINAPI_H

#include <Windows.h>
#include <TlHelp32.h>


MODULEENTRY32 GetModule( DWORD dwProcID, char *moduleName );













#endif