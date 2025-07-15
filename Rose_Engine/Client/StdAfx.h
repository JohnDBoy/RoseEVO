// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef	__SERVER
#include <VCL.h>
#else
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define	_WIN32_WINNT			0x0400

// Windows Header Files:
#include <windows.h>
#include <winUser.h>
#include <mmsystem.h> 
// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>	
#include <crtdbg.h>
#include <string>
#include <set>
#include <d3d9.h>
#include <D3DX9.h>


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// ��ó���� �߰��ϸ� �ּ� �����ּ���.
// ��ó���� ���� �����ϸ� ������ �� ���ּ���.
// 06. 01. 12 - _NoRecover ������ ���缭 ������. �� ������ �ܰ躰�� ���� ���.
//////////////////////////////////////////////////////////////////////////
// ��ó���� ���� ���⿡.//////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG	//������� ��쿡�� �׻� �ְ�. ������ �ϰ�쿡�� �ʿ��� ��츸 �ִ´�.
#define __TEST
#endif

//#define __RELEASE_TEST	//������ �������� �׽�Ʈ������ ����ϱ� ���� �߰��ߴ�.

//#define __TEST
//#define __WORK

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// ���� �����ϴ� �ѱ����� 

#ifdef __TEST

#define _DE							// �׽�Ʈ ������ �ߺ� ���� �����ϰ� �Ѵ�.
//#define __USE_WITHIN_COMPANY_TEST	// use inner test << if not commented the packet will be 604

#define __MSG_USE_RETURN			// �޼��� �ڽ� ���� ���.
#define __AUTOLOGIN					//�ڵ��α���.
#define __SET_PREDEFINED_LANGUAGE	//���� ��ó����� ��� ����


// �̻�� �κ�.
//#define __CHGLISTBOX


#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///���� ��ó����s///

#define __USEJEMBREAK
#define _GBC					// ��ƲīƮ
#define _NoRecover				//  ���� : HP / MP�� ��� ���� �Ͼ�� �������� 1�ʸ��� �ѹ��� �����ֱ�� �ߴ�. �׷��� Ŭ��ȸ���� ��� X
#define _NEWUI					// ȫ�� : ������ �Ǵ� �������̽�.
#define __CLEAR_SKILL			// ��ų �ʱ�ȭ
#define __CLAN_WAR_SET			// Ŭ���� 
#define __ITEM_MAX				// ������ Ȯ��
#define __KRRE_NEW_AVATAR_DB	//
#define __MILEAGESKILL			// ���ϸ��� ��ų
#define _NEWBREAK				// ����
#define _NEWINTERFACEKJH		// 
#define _NEWSTATUS				//
#define __NEWUPDATEPARTYBUFF
#define	__PRIVATECHAT2			// Ŀ�´�Ƽ ��� �߰�
#define __BATTLECART2			// īƮ ��źȭ
#define __CAL_BUF2				// ���� ���� ����.
#define __DIGITEFFECT8			// ȫ�� : ������ �ִ� ǥ�� 4�ڸ����� 8�ڸ��� ����.
#define __ENCRYPT2				// ���� : ��ũ�� ��ȣȭ ��ſ� xor ���!
#define __ARCFOUR				// ȫ�� : ��Ŷ ��ȣȭ �۾�
//#define __ARCFOURLOG
#define __EXPERIENCE_ITEM		// ����ġ ������ �߰�.

#define __ITEM_TIME_LIMMIT		//�Ⱓ�� ������ �߰�. 20070111
#define __KEYBOARD_DIALOG		// ��ȭâ Ű���� ���. ���� 3:54 2007-02-26

#define __NEWUPGRADESYSTEM			// ���� ����.
//Ŭ���� 1�� S
#define __CLAN_INFO_ADD			//
#define __NPC_COLLISION			//
#define __CLAN_WAR_BOSSDLG		// Ŭ���� ������ â �߰�.
#define __EDIT_CLANDLG			// 
//Ŭ���� 1�� E

#define __CLAN_STORE				//Ŭ�� ����Ʈ ����.
#define __EDIT_CLANDLG_CLANWAR		//Ŭ��â ���� �߿� ���� ����Ʈ ����.
#define __NEW_PAT_TYPES			// New pat types (2010-03-21)


#define __BUILD_REGION_NAME   "ROSE Online Evolution"
#define _PACKETRESERVED		 0x0000
#define _WARNING
#define _DE
#define _USA
#define __NoNProtect
#define LAUNCHER_BYPASS_STRING "@TRIGGER_SOFT@"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




// TODO: reference additional headers your program requires here
//-------------------------------------------------------------------------------------------------

typedef	LPDIRECT3DDEVICE9			LPD3DDEVICE;
typedef	IDirect3DTexture9			ID3DTexture;
typedef	LPDIRECT3DTEXTURE9			LPD3DTEXTURE;
typedef	D3DMATERIAL9				D3DMATERIAL;
typedef	LPDIRECT3DVERTEXBUFFER9		LPD3DVERTEXBUFFER;

#define	ZZ_MEM_OFF	// not use zz_mem

#include "zz_interface.h"

// #pragma warning(default : 4786)
#pragma warning( disable : 4651 ) 

#include "util\CVector.h"
#include "common\DataTYPE.h"

#include "util\CD3DUtil.h"
#include "util\classSTR.h"
#include "util\classLOG.h"
#include "util\classLUA.h"
#include "util\classHASH.h"
#include "util\classUTIL.h"
#include "util\SLLIST.h"
#include "util\DLLIST.h"
#include "util\CFileLIST.h"

#include "util\CGameSTR.h"
#include "util\CGameSTB.h"

#include "CApplication.h"

#endif
//-------------------------------------------------------------------------------------------------

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif


#ifndef	GET_WHEEL_DELTA_WPARAM
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))
#endif

//-------------------------------------------------------------------------------------------------

