/*---------------------------------------------------------------------------
Make:		2006.03.23
Name:		ZSTD.h
Author:     남병철.레조

Desc:		
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZSTD_H__
#define __ZSTD_H__

// 디버그용 정의 : ZLog 및 ZTRACE
#define WATCH_DATA
//#define GS_USER_CPP
//#define OBJ_CHAR_CPP
//#define OBJ_NPC_CPP
//#define REGEN_AREA_CPP

//---------------------------------------------------------------------------

#include <windows.h>

//---------------------------------------------------------------------------

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//---------------------------------------------------------------------------
// ZTRACE
// ZString
// ZList
// [ZString] <- ZFile ZLowFile
// [ZString ZList] <- ZStringList
// [ZFile] <- ZBMP ZLog
//---------------------------------------------------------------------------

#include "ZList.h"
#include "ZString.h"
#include "ZStringList.h"

//---------------------------------------------------------------------------

//extern ZLog g_WatchData;
//extern ZLog g_GSUser;
//extern ZLog g_ObjCHAR;
//extern ZLog g_ObjNPC;
//extern ZLog g_RegenAREA;

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
