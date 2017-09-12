#pragma once

#define MAX_VS_BONE 28
#define REFF_NONE    0
#define REFF_ALPHA   1
#define REFF_SFX     2

// file version
#define		VER_BONE			4
#define		VER_MESH			20
#define		VER_MOTION			10

#define		COL_WHITE		D3DCOLOR_ARGB(255, 255, 255, 255)
#define		COL_RED			D3DCOLOR_ARGB(255, 255,   0,   0)
#define		COL_GREEN		D3DCOLOR_ARGB(255,   0, 255,   0)
#define		COL_BLUE		D3DCOLOR_ARGB(255,   0,   0, 255)
#define		COL_GRAY		D3DCOLOR_ARGB(255, 128, 128, 128)
#define		COL_DARKGRAY	D3DCOLOR_ARGB(255,  96,  96,  96)
#define		COL_LIGHTGRAY	D3DCOLOR_ARGB(255, 192, 192, 192)

#define		BS_NONE			0
#define		BS_MODEL		1
#define		BS_OBJECT		2
#define		BS_MTRLBLK		3


#define MAX_MDL_EVENT		8

#define		MA_HIT			(0x00000001)		// Å¸Á¡
#define		MA_SOUND		(0x00000002)		// ¼Ò¸®
#define		MA_QUAKE		(0x00000004)		// ÁöÁø.


#define		MAX_GROUP		3

#define D3DFVF_SKINVERTEX_BUMP (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SKINVERTEX (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX_BUMP (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SHADOWMASKVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

#define		MAX_MATERIAL		2048

#include <vector>
#include "DirectX/d3dx9.h"
#include <windows.h>
#include <iostream>
#include "CMotion.h"

using namespace std;



struct SHADOWMASK_VERTEX
{
	float x, y, z, rhw;
	DWORD color;
};

// Mesh Type



// Vertex Buffer Type
//enum VBTYPE
//{
//	VBT_ERROR = -1,
//	VBT_NORMAL,
//	VBT_BUMP,
//};


struct	INDEX_BUFFER
{
	int	nIdx;
	D3DXVECTOR3		normal;
};

struct SKINVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

struct SKINVERTEX_BUMP
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
	D3DXVECTOR3 tangent;
	FLOAT       tu, tv;		// The texture coordinates
};


struct NORMALVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};


struct NORMALVERTEX_BUMP
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
	D3DXVECTOR3 tangent;
	FLOAT       tu, tv;		// The texture coordinates
};
inline BOOL IsEmpty( LPCTSTR str )
{
	return (str[0] == '\0') ? TRUE : FALSE;
}








