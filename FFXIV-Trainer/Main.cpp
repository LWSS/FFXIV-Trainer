#include <iostream>
#include <Windows.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <math.h>

#include "Memory.hpp"
#include "HackThreads.hpp"


const static float PI = 3.141592f;
const static float PI_HALF = 1.570796;
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
		} else if ( buffer == "noclip" ){
			if( moveHacks_t.status.bNoClip ){
				std::cout<<"Noclip[OFF] - "<<std::endl;
				moveHacks_t.status.bNoClip = false;
				noClipThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"Noclip[ON] - "<< fStepScale <<std::endl;
					noClipCurrant_t.x = playerPos_t.x; // Set Initial Values for Position Locking.
					noClipCurrant_t.y = playerPos_t.y;
					noClipCurrant_t.z = playerPos_t.z;
					moveHacks_t.status.bNoClip = true;
					noClipThread = std::thread(MaintainNoClip);
				} else {
					std::cout<< "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if (buffer == "ghostclip") { /* No clip, don't set player's position until space is pressed in-game. */
			if (moveHacks_t.status.bGhostClip) {
				std::cout << "GhostClip[OFF] - " << std::endl;
				moveHacks_t.status.bGhostClip = false;
				ghostClipThread.join();
			}
			else {
				if( !IsMoveHack() ) {
					std::cout << "GhostClip[ON] - " << fStepScale << std::endl;
					noClipCurrant_t.x = playerPos_t.x; // Set Initial Values for Position Locking.
					noClipCurrant_t.y = playerPos_t.y;
					noClipCurrant_t.z = playerPos_t.z;
					moveHacks_t.status.bGhostClip = true;
					ghostClipThread = std::thread(MaintainGhostClip);
				} else {
					std::cout << "You Must turn off Other Movement Hacks first" << std::endl;
				}
			}
		} else if ( buffer == "step" ){
			std::cin >> fStepScale;
			std::cout<< "Step => " << fStepScale << std::endl;
		} else if ( buffer == "nofall"){
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
		} else if( buffer == "fly" ){
			if( moveHacks_t.status.bFly ){
				std::cout<<"Fly[OFF] - "<<std::endl;
				moveHacks_t.status.bFly = false;
				flyThread.join();
			} else {
				if( !IsMoveHack() ){
					std::cout<<"Fly[ON] - " << std::endl;
					flyCurrent_t.x = playerPos_t.x; // Set Initial Values for Position Locking.
					flyCurrent_t.y = playerPos_t.y;
					flyCurrent_t.z = playerPos_t.z;
					moveHacks_t.status.bFly = true;
					flyThread = std::thread(MaintainFlight);
				} else {
					std::cout<< "You Must turn off Other Movement Hacks first" << std::endl;
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
	ReadProcessMemory(hFF, yCamAddr, &playerCamPos_t.y, 4, NULL); // X - Camera
	ReadProcessMemory(hFF, zCamAddr, &playerCamPos_t.z, 4, NULL); // X - Camera
}

int main( )
{
	
	const int directx = MessageBox( NULL, "Is DirectX 11 on?", "DirectX", MB_YESNO ); // TODO detect directx11 automatically 

	switch( directx ) {
	case IDYES:
		x64 = true;
		break;
	case IDNO:
		x64 = false;
		break;
	}
	
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
	DWORD PID;
	GetWindowThreadProcessId( hWnd, &PID );
	hFF = OpenProcess( PROCESS_ALL_ACCESS, false, PID );
		
	if ( !hFF ){
		std::cout<< "Cannot Access Process Memory"<<std::endl;
		std::cin.get();
		return 0;
	} 
	SetupAddress();

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
				std::cout<< "*X: " << playerPos_t.x << "  Y: " << playerPos_t.y << "  Z: " << playerPos_t.z << "  Head: " << playerPos_t.h;
				std::cout<< "\r" << std::flush;
			}
#pragma region noClip
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

#pragma region fly
			if ( moveHacks_t.status.bFly ) {
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