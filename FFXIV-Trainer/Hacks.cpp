#include "Hacks.h"


void MaintainFlight()
{
	while( moveHacks_t.status.bFly ) {
		mFly.lock();
		if( flyBuffer_t.x != 0 || flyBuffer_t.y != 0 || flyBuffer_t.z != 0 ) {

			flyCurrent_t.x = flyBuffer_t.x;
			flyCurrent_t.y = flyBuffer_t.y;
			flyCurrent_t.z = flyBuffer_t.z;


			WriteProcessMemory( hFF, xAddr, &flyBuffer_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &flyBuffer_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &flyBuffer_t.z, 4, NULL );

			flyBuffer_t.x = 0;
			flyBuffer_t.y = 0;
			flyBuffer_t.z = 0;
		} else {
			WriteProcessMemory( hFF, xAddr, &flyCurrent_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &flyCurrent_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &flyCurrent_t.z, 4, NULL );
		}
		mFly.unlock();
	}
}
void MaintainGhostClip()
{
	while( moveHacks_t.status.bGhostClip ) {
		mNoClip.lock();
		if( noClipBuffer_t.x != 0 || noClipBuffer_t.y != 0 || noClipBuffer_t.z != 0 ) { /* Is there a Change to process? Noted by noClipBuffer */

			noClipCurrant_t.x = noClipBuffer_t.x;
			noClipCurrant_t.y = noClipBuffer_t.y;
			noClipCurrant_t.z = noClipBuffer_t.z;


			WriteProcessMemory( hFF, xAddr, &noClipBuffer_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &noClipBuffer_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &noClipBuffer_t.z, 4, NULL );

			noClipBuffer_t.x = 0;
			noClipBuffer_t.y = 0;
			noClipBuffer_t.z = 0;
		} else { /* Else, just hover. */
			WriteProcessMemory( hFF, xAddr, &noClipCurrant_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &noClipCurrant_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &noClipCurrant_t.z, 4, NULL );
		}
		mNoClip.unlock();
		Sleep( 25 );
	}
}

void MaintainNoClip()
{
	while( moveHacks_t.status.bNoClip ) {
		mNoClip.lock();
		if( noClipBuffer_t.x != 0 || noClipBuffer_t.y != 0 || noClipBuffer_t.z != 0 ) { /* Is there a Change to process? Noted by noClipBuffer */

			noClipCurrant_t.x = noClipBuffer_t.x;
			noClipCurrant_t.y = noClipBuffer_t.y;
			noClipCurrant_t.z = noClipBuffer_t.z;


			WriteProcessMemory( hFF, xAddr, &noClipBuffer_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &noClipBuffer_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &noClipBuffer_t.z, 4, NULL );

			WriteProcessMemory( hFF, xCamAddr, &noClipBuffer_t.x, 4, NULL );
			WriteProcessMemory( hFF, yCamAddr, &noClipBuffer_t.y, 4, NULL );
			WriteProcessMemory( hFF, zCamAddr, &noClipBuffer_t.z, 4, NULL );

			noClipBuffer_t.x = 0;
			noClipBuffer_t.y = 0;
			noClipBuffer_t.z = 0;
		} else { /* Else, just hover. */
			WriteProcessMemory( hFF, xAddr, &noClipCurrant_t.x, 4, NULL );
			WriteProcessMemory( hFF, yAddr, &noClipCurrant_t.y, 4, NULL );
			WriteProcessMemory( hFF, zAddr, &noClipCurrant_t.z, 4, NULL );
		}
		mNoClip.unlock();
		Sleep( 25 );
	}
}

void MaintainNoFalling() /* No Mutex */
{
	float yMaintain = playerPos_t.y; // Our Altitude to Maintain
	float yCurrent; // Current altitude Read-in from program.
	while( moveHacks_t.status.bNoFall ) {
		ReadProcessMemory( hFF, yAddr, &yCurrent, 4, NULL ); // Read Y( Altitude ) Value In
		if( yCurrent > yMaintain + 0.15 ) { // if the player jumped or reacher higher altitude ( with a little margin for error )
			yMaintain = playerPos_t.y;
		}
		if( yCurrent != yMaintain ) {
			WriteProcessMemory( hFF, yAddr, &yMaintain, 4, NULL ); // Update our height
			WriteProcessMemory( hFF, yCamAddr, &yMaintain, 4, NULL ); // Update our height for the camera
		}
	}
}
void SetupAddress()
{
	if( x64 ) {
		xBase = { playerPosAddr, 0xB0 };
		yBase = { playerPosAddr, 0xB4 };
		zBase = { playerPosAddr, 0xB8 };
		xCamBase = { playerPosAddr, 0xFC, 0x40 };
		yCamBase = { playerPosAddr, 0xFC, 0x44 };
		zCamBase = { playerPosAddr, 0xFC, 0x48 };
		hBase = { playerPosAddr, 0xC0 };
	} else {
		xBase = { playerPosAddr, 0xA0 };
		yBase = { playerPosAddr, 0xA4 };
		zBase = { playerPosAddr, 0xA8 };
		xCamBase = { playerPosAddr, 0xEC, 0x30 };
		yCamBase = { playerPosAddr, 0xEC, 0x34 };
		zCamBase = { playerPosAddr, 0xEC, 0x38 };
		hBase = { playerPosAddr, 0xB0 };
	}
	xAddr = TraceBasePointer( xBase );
	yAddr = TraceBasePointer( yBase );
	zAddr = TraceBasePointer( zBase );
	hAddr = TraceBasePointer( hBase );
	xCamAddr = TraceBasePointer( xCamBase );
	yCamAddr = TraceBasePointer( yCamBase );
	zCamAddr = TraceBasePointer( zCamBase );
}
LPVOID TraceBasePointer( std::vector<unsigned long long> Base )
{
	unsigned long long lBaseAddr = Base[0]; // First Element in Base Vector is Base Addr
	for( unsigned int i = 1; i < Base.size(); i++ ) { // Rest are Offsets
		if( x64 ) {
			ReadProcessMemory( hFF, ( LPVOID )lBaseAddr, &lBaseAddr, 8, NULL );
		} else {
			ReadProcessMemory( hFF, ( LPVOID )lBaseAddr, &lBaseAddr, 4, NULL );
		}
		lBaseAddr += Base[i];
	}
	return ( LPVOID )lBaseAddr;
}
void UpdatePlayerPos()
{
	ReadProcessMemory( hFF, xAddr, &playerPos_t.x, 4, NULL ); // Read X Value In
	ReadProcessMemory( hFF, yAddr, &playerPos_t.y, 4, NULL ); // Read Y( Altitude ) Value In
	ReadProcessMemory( hFF, zAddr, &playerPos_t.z, 4, NULL ); // Read Z Value In
	ReadProcessMemory( hFF, hAddr, &playerPos_t.h, 4, NULL ); // Read Radians In

	ReadProcessMemory( hFF, xCamAddr, &playerCamPos_t.x, 4, NULL ); // X - Camera
	ReadProcessMemory( hFF, yCamAddr, &playerCamPos_t.y, 4, NULL ); // X - Camera
	ReadProcessMemory( hFF, zCamAddr, &playerCamPos_t.z, 4, NULL ); // X - Camera
}