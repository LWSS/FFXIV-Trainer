#include <iostream>
#include <iomanip> // float fixed precision 
#include <Windows.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <math.h>

#include "Memory.hpp"
#include "HackThreads.hpp"
#include "WinApi.h"


const static float PI = 3.141592f;
const static float PI_HALF = 1.570796f;
float fStepScale = 1.0f;

bool IsMoveHack( ) /* Is player already using one movement hack? */
{
	for( int i = 0; i < ( sizeof( moveHacks_t.status ) / sizeof( bool ) ); i++ ) {
		if( moveHacks_t.iterate[i] ){ 
			std::cout << "Found Existing Hack on: " << i << std::endl;
			return true; 
		}
	}
	return false;
}

bool bShowPos = true;
bool bCommandMode = false;

void ReadCin(std::atomic<bool>& run)
{
    std::string buffer;

    while (run.load())
    {
        std::cin >> buffer;
        if (buffer == "quit" ) {
            run.store(false);
        } else if ( buffer == "togglepos"){
			if( bShowPos ){
				std::cout<<"Position Tracking[OFF]"<<std::endl;
				bShowPos = false;
			} else {
				std::cout<<"Position Tracking[ON]"<<std::endl;
				bShowPos = true;
			}
		} else if ( buffer == "clear" ){
			std::system("cls");
		} else if ( buffer == "c" ){ // C for Continue and blank entry
		} else if ( buffer == "tp"){ // Teleporation
			float xTo, yTo, zTo; 
			std::cin >> xTo;
			std::cin >> yTo;
			std::cin >> zTo;

			std::cout<< "X: " << xTo <<" Y: " << yTo << " Z: " << zTo << std::endl;
		} else if ( buffer == "noclip" || buffer == "nc" ){
			if( moveHacks_t.status.bNoClip ){
				std::cout<<"Noclip[OFF] - "<<std::endl;
				moveHacks_t.status.bNoClip = false;
				noClipThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"Noclip[ON] - "<< fStepScale <<std::endl;
					noClipCurrent_t = playerPos_t; // Set Initial Values for Position Locking.
					moveHacks_t.status.bNoClip = true;
					noClipThread = std::thread(MaintainNoClip);
				} else {
					std::cout<< "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if ( buffer == "ghostclip" || buffer == "gc" ) { /* No clip, don't set player's position until space is pressed in-game. */
			if (moveHacks_t.status.bGhostClip) {
				std::cout << "GhostClip[OFF] - " << std::endl;
				moveHacks_t.status.bGhostClip = false;
				ghostClipThread.join();
			}
			else {
				if( !IsMoveHack() ) {
					std::cout << "GhostClip[ON] - " << fStepScale << std::endl;
					noClipCurrent_t = playerPos_t; // Set Initial Values for Position Locking.
					moveHacks_t.status.bGhostClip = true;
					ghostClipThread = std::thread(MaintainGhostClip);
				} else {
					std::cout << "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if ( buffer == "step" ){
			std::cin >> fStepScale;
			std::cout<< "Step => " << fStepScale << std::endl;
		} else if ( buffer == "nofall" || buffer == "nf" ){
			if( moveHacks_t.status.bNoFall ){
				std::cout<<"NoFall[OFF] - "<<std::endl;
				moveHacks_t.status.bNoFall = false;
				noFallThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"NoFall[ON] - "<< fStepScale <<std::endl;
					moveHacks_t.status.bNoFall = true;
					noFallThread = std::thread(MaintainNoFalling);
				} else {
					std::cout<< "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if( buffer == "fly" || buffer == "f" ){
			if( moveHacks_t.status.bFly ){
				std::cout<<"Fly[OFF]"<<std::endl;
				moveHacks_t.status.bFly = false;
				flyThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"Fly[ON] - " << std::endl;
					flyCurrent_t = playerPos_t; // Set Initial Values for Position Locking.
					moveHacks_t.status.bFly = true;
					flyThread = std::thread(MaintainFlight);
				} else {
					std::cout<< "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if( buffer == "ghostfly" || buffer == "gf" ){ // tfw no gf
			if( moveHacks_t.status.bGhostFly ){
				std::cout<<"GhostFly[OFF]" <<std::endl;
				moveHacks_t.status.bGhostFly = false;
				ghostFlyThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"GhostFly[ON]"<<std::endl;
					flyCurrent_t = playerPos_t; // Set Initial Values for Position Locking.
					moveHacks_t.status.bGhostFly = true;
					ghostFlyThread = std::thread(MaintainGhostFlight);
				} else {
					std::cout << "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		}
		bCommandMode = false;
    }
}
void UpdatePlayerPos() /* Called in loop to Read in Player Coords */
{
	ReadProcessMemory(hFF, xAddr, &playerPos_t.x, 4, NULL); // Read X Value In
	ReadProcessMemory(hFF, yAddr, &playerPos_t.y, 4, NULL); // Read Y( Altitude ) Value In
	ReadProcessMemory(hFF, zAddr, &playerPos_t.z, 4, NULL); // Read Z Value In
	ReadProcessMemory(hFF, hAddr, &playerPos_t.h, 4, NULL); // Read Radians In

	ReadProcessMemory(hFF, xCamAddr, &playerCamPos_t.x, 4, NULL); // X - Camera
	ReadProcessMemory(hFF, yCamAddr, &playerCamPos_t.y, 4, NULL); // Y - Camera
	ReadProcessMemory(hFF, zCamAddr, &playerCamPos_t.z, 4, NULL); // Z - Camera
}

int main( )
{
	std::atomic<bool> run(true);
	std::thread cinThread( ReadCin, std::ref(run));

	HWND hWnd = FindWindowA(0, "FINAL FANTASY XIV");
	DWORD ffPid = 0;
	GetWindowThreadProcessId( hWnd, &ffPid );
	HWND hOurWindow = GetForegroundWindow();
	if( hWnd == 0 ){
		std::cout<< "Could Not Find ffxiv Window" << std::endl;
		std::cin.get();
		return 0;
	} 
	std::cout<< "Found Final Fantasy (PID: " << ffPid << ")" <<std::endl;
	
	MODULEENTRY32 ffModule;
	ffModule = GetModule( ffPid, "ffxiv_dx11.exe" ); 
	if( ffModule.th32ModuleID == 0 ) {
		ffModule = GetModule( ffPid, "ffxiv.exe" ); // try 32 bit module 
		if( ffModule.th32ModuleID == 0 ){
			std::cout << "Couldn't find ffxiv Module" << std::endl;
			std::cin.get();
			return -1;
		}
		std::cout<< "DirectX 9 detected (x32)" << std::endl;
		x64 = false;
	}
	std::cout << "DirectX 11 detected (x64)" << std::endl;
	x64 = true; 

	std::cout << "ffxiv Module Found: " << std::hex << (unsigned int)ffModule.modBaseAddr << std::endl;


	hFF = OpenProcess( PROCESS_ALL_ACCESS, false, ffPid );
	if ( !hFF ){
		std::cout<< "Cannot Access Process Memory"<<std::endl;
		std::cin.get();
		return 0;
	} 
	
	SetupAddress(ffModule);

	while( run.load() ) {
		if(  GetAsyncKeyState(VK_SPACE)  &&  bCommandMode == false && hOurWindow == GetForegroundWindow()  ){
			bCommandMode = true;
			std::cout<< "\n>";
		} else if ( GetAsyncKeyState(VK_ESCAPE) && bCommandMode == true && hOurWindow == GetForegroundWindow() ){
			bCommandMode = false;
			std::cout<< "\b \b";
		}
		if( !bCommandMode ){
			
			UpdatePlayerPos();
			if ( bShowPos ){
				std::cout<< std::fixed << std::setprecision(3) << "*X: " << playerPos_t.x << "  Y: " << playerPos_t.y << "  Z: " << playerPos_t.z << "  Head: " << playerPos_t.h;
				std::cout<< "\r" << std::flush;
			}
#pragma region NOCLIP_INPUT
			if ( moveHacks_t.status.bNoClip || moveHacks_t.status.bGhostClip ) {
				if (GetAsyncKeyState(VK_NUMPAD4)){ // Left
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x - fStepScale;
					noClipBuffer_t.y = playerPos_t.y;
					noClipBuffer_t.z = playerPos_t.z;
					mNoClip.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD6)){ // Right
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x + fStepScale;
					noClipBuffer_t.y = playerPos_t.y;
					noClipBuffer_t.z = playerPos_t.z;
					mNoClip.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD8)){ // Forward
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x;
					noClipBuffer_t.y = playerPos_t.y;
					noClipBuffer_t.z = playerPos_t.z - fStepScale;
					mNoClip.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD5)){ // Back
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x;
					noClipBuffer_t.y = playerPos_t.y;
					noClipBuffer_t.z = playerPos_t.z + fStepScale;
					mNoClip.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD1)){ // Up
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x;
					noClipBuffer_t.y = playerPos_t.y + fStepScale;
					noClipBuffer_t.z = playerPos_t.z;
					mNoClip.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD0)){ // Down
					mNoClip.lock();
					noClipBuffer_t.x = playerPos_t.x;
					noClipBuffer_t.y = playerPos_t.y - fStepScale;
					noClipBuffer_t.z = playerPos_t.z;
					mNoClip.unlock();
				}
			}
#pragma endregion

#pragma region FLY_INPUT
			if ( moveHacks_t.status.bFly || moveHacks_t.status.bGhostFly ) {
				if (GetAsyncKeyState(VK_NUMPAD4)){ // Left
					mFly.lock();
						flyBuffer_t.x = playerPos_t.x + ( sin( playerPos_t.h + PI_HALF ) * fStepScale );
						flyBuffer_t.y = playerPos_t.y;
						flyBuffer_t.z = playerPos_t.z + ( cos( playerPos_t.h + PI_HALF ) * fStepScale );
					mFly.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD6)){ // Right
					mFly.lock();
						flyBuffer_t.x = playerPos_t.x + ( sin( playerPos_t.h - PI_HALF ) * fStepScale );
						flyBuffer_t.y = playerPos_t.y;
						flyBuffer_t.z = playerPos_t.z + ( cos( playerPos_t.h - PI_HALF ) * fStepScale );
					mFly.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD8)){ // Forward
					mFly.lock();
					flyBuffer_t.x = playerPos_t.x + ( sin(playerPos_t.h) * fStepScale);
					flyBuffer_t.y = playerPos_t.y;
					flyBuffer_t.z = playerPos_t.z + ( cos(playerPos_t.h) * fStepScale);
					mFly.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD5)){ // Back
					mFly.lock();
					flyBuffer_t.x = playerPos_t.x - ( sin( playerPos_t.h ) * fStepScale );
					flyBuffer_t.y = playerPos_t.y;
					flyBuffer_t.z = playerPos_t.z - ( cos( playerPos_t.h ) * fStepScale );
					mFly.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD1)){ // Up
					mFly.lock();
					flyBuffer_t.x = playerPos_t.x;
					flyBuffer_t.y = playerPos_t.y + fStepScale;
					flyBuffer_t.z = playerPos_t.z;
					mFly.unlock();
				} else if (GetAsyncKeyState(VK_NUMPAD0)){ // Down
					mFly.lock();
					flyBuffer_t.x = playerPos_t.x;
					flyBuffer_t.y = playerPos_t.y - fStepScale;
					flyBuffer_t.z = playerPos_t.z;
					mFly.unlock();
				}
			}
#pragma endregion
		}
		Sleep(75);
	}

	run.store(false);
	cinThread.join();

	std::cin.get();
	return 0;
}