#pragma once

#include <Windows.h>

class CScriptSystem
{
public:

    char padding[0x38];
    __int64* lua_state;

    virtual ~CScriptSystem() = 0;
    virtual char** GetTypeInfo() = 0;
};

class CDuniaEngine
{
public:
    char pad_0x0000[0x14]; //0x0000
    DWORD N000000B3; //0x0014 
    char pad_0x0018[0x88]; //0x0018
    DWORD64 N00000069; //0x00A0 
    char pad_0x00A8[0x1AB]; //0x00A8
    unsigned char IsGamePaused; //0x0253 
    char pad_0x0254[0x2]; //0x0254
    unsigned char N000000AF; //0x0256 
    char pad_0x0257[0x21]; //0x0257
    DWORD GameStateFlags; //0x0278 
    char pad_0x027C[0xC]; //0x027C

}; //Size=0x0288

class CFacility
{
public:
    char pad_0x0000[0x10]; //0x0000
    DWORD64 N00000011; //0x0010 
    DWORD64 N00000010; //0x0018 
    char pad_0x0020[0x60]; //0x0020
    float N00000023; //0x0080 
    char pad_0x0084[0x4]; //0x0084
    float N00000022; //0x0088 
    char pad_0x008C[0x4]; //0x008C
    unsigned char IsGamePaused; //0x0090 
    char pad_0x0091[0x1]; //0x0091
    unsigned char N0000003F; //0x0092 
    char pad_0x0093[0x1D]; //0x0093
    unsigned char N00000030; //0x00B0 
    char pad_0x00B1[0x3]; //0x00B1
    DWORD64 N00000034; //0x00B4 
    char pad_0x00BC[0x4]; //0x00BC
    unsigned char N0000002E; //0x00C0 
    char pad_0x00C1[0x1F]; //0x00C1
    float N0000001F; //0x00E0 
    char pad_0x00E4[0x4]; //0x00E4

}; //Size=0x00E8

// TODO : document better

struct IDXGIFactory;

class D3D_Class
{
public:

    char padding[80];
    IDXGIFactory *factory;
};