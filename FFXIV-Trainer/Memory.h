#pragma once
#ifndef MEMORY_H
#define MEMORY_H

union
{
	struct
	{
		bool bNoClip, bGhostClip, bNoFall, bFly; // boolean default = FALSE
	} status;
	bool iterate[4];
} moveHacks_t;

HANDLE hFF;

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

static const std::vector<int> xBase = { 0x01E25E40, 0xA0 };
static const std::vector<int> yBase = { 0x01E25E40, 0xA4 };
static const std::vector<int> zBase = { 0x01E25E40, 0xA8 };
static const std::vector<int> xCamBase = { 0x01E25E40, 0xEC, 0x30 };
static const std::vector<int> yCamBase = { 0x01E25E40, 0xEC, 0x34 };
static const std::vector<int> zCamBase = { 0x01E25E40, 0xEC, 0x38 };
static const std::vector<int> hBase = { 0x01E25E40, 0xB0 }; // ;/0xB0 }; // Heading in Radians
LPVOID xAddr;
LPVOID yAddr;
LPVOID zAddr;
LPVOID hAddr;
LPVOID xCamAddr;
LPVOID yCamAddr;
LPVOID zCamAddr;

struct vec3 {
	float x, y, z, h;
} playerPos_t, playerCamPos_t, noClipBuffer_t = { 0, 0, 0 }, noClipCurrant_t, flyBuffer_t = { 0, 0, 0 }, flyCurrent_t = { 0, 0, 0 };

LPVOID TraceBasePointer(std::vector<int> Base)
{
	int iBaseAddr = Base[0]; // First Element in Base Vector is Base Addr
	for (unsigned int i = 1; i < Base.size(); i++) { // Rest are Offsets
		ReadProcessMemory(hFF, (LPVOID)iBaseAddr, &iBaseAddr, 4, NULL);
		iBaseAddr += Base[i];
	}
	return (LPVOID)iBaseAddr;
}

void SetupAddress()
{
	xAddr = TraceBasePointer(xBase);
	yAddr = TraceBasePointer(yBase);
	zAddr = TraceBasePointer(zBase);
	hAddr = TraceBasePointer(hBase);
	xCamAddr = TraceBasePointer(xCamBase);
	yCamAddr = TraceBasePointer(yCamBase);
	zCamAddr = TraceBasePointer(zCamBase);
}



#endif