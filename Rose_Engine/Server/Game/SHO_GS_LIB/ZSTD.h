/*---------------------------------------------------------------------------
Make:		2006.03.23
Name:		ZSTD.h
Author:     ����ö.����

Desc:		
---------------------------------------------------------------------------*/



//---------------------------------------------------------------------------

#ifndef __ZSTD_H__
#define __ZSTD_H__

// ����׿� ���� : ZLog �� ZTRACE
#define WATCH_DATA
//#define GS_USER_CPP
//#define OBJ_CHAR_CPP
//#define OBJ_NPC_CPP
//#define REGEN_AREA_CPP

//---------------------------------------------------------------------------

#include <windows.h>

//---------------------------------------------------------------------------

#ifndef FLT_MAX // float.h
#define FLT_MAX         3.402823466e+38F        /* max value */
#endif

const float		INFINITY	= FLT_MAX;
const float		EPSILON		= 0.001f;

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

//---------------------------------------------------------------------------

#include "ZTrace.h"
#include "ZString.h"
#include "ZList.h"
#include "ZStringList.h"
#include "ZIniFile.h"
#include "ZFile.h"
#include "ZLog.h"
#include "ZCriticalSection.h"
#include "ZMultiThreadSync.h"

//---------------------------------------------------------------------------

enum QL_ZQUEST_LIST
{
	QLZ_QUEST_IDX,
	QLZ_QUEST_NO,
	QLZ_QUEST_NAME,
	QLZ_MAX
};

// ����Ʈ ����Ʈ ����
extern ZStringList g_zQuestList[QLZ_MAX];

// (20070315:����ö) : ����� �α׿�
extern ZLog g_WatchData;

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
