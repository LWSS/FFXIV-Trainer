#pragma once
#ifndef HACKTHREADS_H
#define HACKTHREADS_H

std::mutex mNoClip, mFly;
std::thread noClipThread, ghostClipThread, noFallThread, flyThread, ghostFlyThread;


void MaintainNoClip()
{
	while (moveHacks_t.status.bNoClip) {
		mNoClip.lock();
		if (noClipBuffer_t.x != 0 || noClipBuffer_t.y != 0 || noClipBuffer_t.z != 0) { /* Is there a Change to process? Noted by noClipBuffer */

			noClipCurrant_t.x = noClipBuffer_t.x;
			noClipCurrant_t.y = noClipBuffer_t.y;
			noClipCurrant_t.z = noClipBuffer_t.z;


			WriteProcessMemory(hFF, xAddr, &noClipBuffer_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &noClipBuffer_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &noClipBuffer_t.z, 4, NULL);

			WriteProcessMemory(hFF, xCamAddr, &noClipBuffer_t.x, 4, NULL);
			WriteProcessMemory(hFF, yCamAddr, &noClipBuffer_t.y, 4, NULL);
			WriteProcessMemory(hFF, zCamAddr, &noClipBuffer_t.z, 4, NULL);

			noClipBuffer_t.x = 0;
			noClipBuffer_t.y = 0;
			noClipBuffer_t.z = 0;
		}
		else { /* Else, just hover. */
			WriteProcessMemory(hFF, xAddr, &noClipCurrant_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &noClipCurrant_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &noClipCurrant_t.z, 4, NULL);
		}
		mNoClip.unlock();
		Sleep(25);
	}
}

void MaintainGhostClip()
{
	while (moveHacks_t.status.bGhostClip) {
		mNoClip.lock();
		if (noClipBuffer_t.x != 0 || noClipBuffer_t.y != 0 || noClipBuffer_t.z != 0) { /* Is there a Change to process? Noted by noClipBuffer */

			noClipCurrant_t.x = noClipBuffer_t.x;
			noClipCurrant_t.y = noClipBuffer_t.y;
			noClipCurrant_t.z = noClipBuffer_t.z;


			WriteProcessMemory(hFF, xAddr, &noClipBuffer_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &noClipBuffer_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &noClipBuffer_t.z, 4, NULL);

			noClipBuffer_t.x = 0;
			noClipBuffer_t.y = 0;
			noClipBuffer_t.z = 0;
		}
		else { /* Else, just hover. */
			WriteProcessMemory(hFF, xAddr, &noClipCurrant_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &noClipCurrant_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &noClipCurrant_t.z, 4, NULL);
		}
		mNoClip.unlock();
		Sleep(25);
	}
}


void MaintainNoFalling() /* No Mutex */
{
	float yMaintain = playerPos_t.y; // Our Altitude to Maintain
	float yCurrent; // Current altitude Read-in from program.
	while (moveHacks_t.status.bNoFall) {
		ReadProcessMemory(hFF, yAddr, &yCurrent, 4, NULL); // Read Y( Altitude ) Value In
		if (yCurrent > yMaintain + 0.15) { // if the player jumped or reacher higher altitude ( with a little margin for error )
			yMaintain = playerPos_t.y;
		}
		WriteProcessMemory(hFF, yAddr, &yMaintain, 4, NULL); // Update our height
		WriteProcessMemory(hFF, yCamAddr, &yMaintain, 4, NULL); // Update our height for the camera
		Sleep(25);
	}
} 

void MaintainFlight()
{
	while (moveHacks_t.status.bFly) {
		mFly.lock();
		if (flyBuffer_t.x != 0 || flyBuffer_t.y != 0 || flyBuffer_t.z != 0) {

			flyCurrent_t.x = flyBuffer_t.x;
			flyCurrent_t.y = flyBuffer_t.y;
			flyCurrent_t.z = flyBuffer_t.z;


			WriteProcessMemory(hFF, xAddr, &flyBuffer_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &flyBuffer_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &flyBuffer_t.z, 4, NULL);

			WriteProcessMemory(hFF, xCamAddr, &flyBuffer_t.x, 4, NULL);
			WriteProcessMemory(hFF, yCamAddr, &flyBuffer_t.y, 4, NULL);
			WriteProcessMemory(hFF, zCamAddr, &flyBuffer_t.z, 4, NULL);

			flyBuffer_t.x = 0;
			flyBuffer_t.y = 0;
			flyBuffer_t.z = 0;
		}
		else {
			WriteProcessMemory(hFF, xAddr, &flyCurrent_t.x, 4, NULL);
			WriteProcessMemory(hFF, yAddr, &flyCurrent_t.y, 4, NULL);
			WriteProcessMemory(hFF, zAddr, &flyCurrent_t.z, 4, NULL);
		}
		mFly.unlock();
	}
}

void MaintainGhostFlight()
{
	while( moveHacks_t.status.bGhostFly ) {
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

#endif