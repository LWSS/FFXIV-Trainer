#pragma once
#ifndef HACKTHREADS_H
#define HACKTHREADS_H

#include <mutex>
#include <thread>
#include <Windows.h>
#include <vector>

static HANDLE hFF;

// Actual => FFXIV
// ~~ 4/1/16
// X     =     X        -- 12C8F0D0
// Y     =     Altitude -- 12C8F0D4
// Z     =     Y        -- 12C8F0D8
// 4/4/16 --- { 0x01C21D1C, 0x4, 0xA0 };
// 4/8/16 --- { 0x01FD1D14, 0x4, 0xA0 };
// 4/9/16 --- { 0x01A60840, 0xA0 };
// 4/15/16 -- { 0x022F0840, 0xA0 };
// 4/20/16 -- { 0x01180840, 0xA0 }; date estimated
// 8/26/16 -- { 0x01E145F0, 0xA0 }; 
// 8/26/16 -- Camera Movement -- 0x154840D0
// 8/26/16 -- Heading -- 0x01E161E0
// 8/30/16 -- { 0x01CC55C0, 0xA0 };
// 8/30/16 --  { 0x01C445F0, 0xA0 };
// 9/1/16 -- { 0x00FF45F0, 0xA0 };
// 9/2/16 -- { 0x01F945F0, 0xA0 };
// 11/12/16 -- { 0x01D645F0, 0xA0 };
// 1/12/17 -- { 0x013B5E40, 0xA0 };

// static const unsigned long long playerPosAddr = 0x7FF725A9EB68L;
const unsigned long long playerPosAddr = 0x01313F30;
static bool x64;

std::vector<unsigned long long> xBase;
std::vector<unsigned long long> yBase;
std::vector<unsigned long long> zBase;
std::vector<unsigned long long> xCamBase;
std::vector<unsigned long long> yCamBase;
std::vector<unsigned long long> zCamBase;
std::vector<unsigned long long> hBase; // ;/0xB0 }; // Heading in Radians

static LPVOID xAddr;
static LPVOID yAddr;
static LPVOID zAddr;
static LPVOID hAddr;
static LPVOID xCamAddr;
static LPVOID yCamAddr;
static LPVOID zCamAddr;

LPVOID TraceBasePointer( std::vector<unsigned long long> Base );

void SetupAddress();



std::mutex mNoClip, mFly;
std::thread noClipThread, noFallThread, flyThread, ghostClipThread;

union
{
	struct
	{
		bool bNoClip, bGhostClip, bNoFall, bFly; // boolean default = FALSE
	} status;
	bool iterate[4];
} moveHacks_t;

struct vec3
{
	float x, y, z, h; // x, y, z, heading in rads
} playerPos_t, playerCamPos_t, noClipBuffer_t = { 0, 0, 0 }, noClipCurrant_t, flyBuffer_t = { 0, 0, 0 }, flyCurrent_t = { 0, 0, 0 };

void MaintainNoClip();
void MaintainGhostClip();
void MaintainNoFalling();
void MaintainFlight();

void UpdatePlayerPos(); /* Called in loop to Read in Player Coords */


#endif