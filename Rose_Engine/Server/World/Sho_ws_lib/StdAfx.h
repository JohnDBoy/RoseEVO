// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)
#define AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_



#define	_WIN32_WINNT 0x0500



#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// TODO: reference additional headers your program requires here
#include <windows.h>
#include <crtdbg.h>
#include <assert.h>

//#define	__SHO_WS   //�Ӽ� ��ó���⿡�� ������....
//	���ϸ��� ��ų ���� ����
//	�迵ȯ �ۼ�
#define __MILEAGE_SKILL_USED	// ���ϸ��� ��ų : DB, Userdata, ��Ŷ ��� ���� ����

#define __CLAN_INFO_ADD			// Ŭ�� ���� �߰� : ��ŷ ���� ���� ó��.

#define __CLAN_WAR_SET			// Ŭ���� �߰� ����

// (20061222:����ö) : Ŭ���� ���� ����Ʈ �߰�
#define __CLAN_REWARD_POINT


// 2007.01.10/��뼺/�߰�	
#define __ITEM_TIME_LIMIT		// �Ⱓ�� ������



#include "LIB_Util.h"
#include "classHASH.h"
#include "classTIME.h"
#include "classLOG.h"
#include "classSTR.h"
#include "classSTB.h"
#include "classSYNCOBJ.h"
#include "CRandom.h"
#include "CDataPOOL.h"
#include "DLLIST.h"
#include "CDLList.h"
#include "CSLList.h"
#include "CVector.h"





#include "ZSTD.h"
#define __COMMUNITY_1_1_CHAT



////////////////////////////////////////////////////////////////////////////////////////////////
////////  �� ��    **DEFINE**    ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
//  ************************** �߿� ���⼭�� �ϳ��� define�� �����ؾ� �Ѵ�...****************************

//#define __ENGLAND
//#define __JAPAN				// �Ϻ� ���� (������)
//#define __EUROPE				// ������ ������ ��������....
//#define __KR_RENEWAL			// �ѱ� ���������
//#define __KOREA				// �ѱ������� �������� ����. ���̻� ���� �Ұ� (2006.04.11) => �����Ϸ� (2006.05.16)
//#define __PHILIPPINE			// �ʸ��� ����
//#define __NORTHUSA			// �̱� ����


//#define __KOREA_EVO				// �ѱ������� �������� ����. ���̻� ���� �Ұ� (2006.04.11) => �����Ϸ� (2006.05.16)
//#define __PHILIPPINE_EVO		// �ʸ��� �������
#define __NORTHUSA_EVO		// �̱� �������
//#define __JAPAN_EVO				// �Ϻ� �������
/////////////////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------
// 2006.05.17/��뼺/�߰�
#define __PACKET_HEADER_VERSION		0x0000 // ��Ŷ������ �������� ��Ŷ����� pPacketHeader->m_wReserved ���� Ư�������� üũ


#pragma COMPILE_TIME_MSG("&&&&&&&&&&&&&&&WS57005&&&&&&&&&&&&&&&&&&&")
//-------------------------------------
#define __MAX_ITEM_999_EXT //����			// 2006.07.06/��뼺/�߰� - ������ 999 ���� Ȯ��


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM				// �츮���� �����Ѵ�.		=> ??? NET_Prototype.h �� ���ǵǾ� �־ ��� ������ �� ����ȴ�.
#define __PASS_FREE_PLATINUM_CHAR   // �츮 ���� ���� �̺�Ʈ�� ����Ƽ�� ĳ���������Ҽ��ְ� �Ѵ�..
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __KOREA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __KOREA     &&&&&&&&&&&&&&&&&&&&&&&")

#define __INC_PLATINUM				// �츮���� �����Ѵ�.		=> ??? NET_Prototype.h �� ���ǵǾ� �־ ��� ������ �� ����ȴ�.
#define __KR_RENEWAL
#endif


#ifdef __JAPAN
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN     &&&&&&&&&&&&&&&&&&&&&&&")

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __JAPAN_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __JAPAN_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					//�븸 �ʱ� ������ ����
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __PHILIPPINE_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __PHILIPPINE_EVO     &&&&&&&&&&&&&&&&&&&&&&&")

#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __ENGLAND
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __ENGLAND     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// �븸 �ʱ� ������ ����
#define __MAX_CHAR_3			3		// �ִ� �ɸ��ͼ��� 5 ->3 ���� ����..
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __EUROPE
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(��������)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(��������)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __EUROPE(��������)     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// �븸 �ʱ� ������ ����
#define __MAX_CHAR_3			3		// �ִ� �ɸ��ͼ��� 5 ->3 ���� ����..
#define __KCHS_BATTLECART__	
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(�̱�)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(�̱�)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA(�̱�)     &&&&&&&&&&&&&&&&&&&&&&&")

#define __TAW_RENEWAL					// �븸 �ʱ� ������ ����
#define __MAX_CHAR_3			3		// �ִ� �ɸ��ͼ��� 5 ->3 ���� ����..
#define __KCHS_BATTLECART__
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __NORTHUSA_EVO
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(�̱� �������)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(�̱� �������)     &&&&&&&&&&&&&&&&&&&&&&&")
#pragma  COMPILE_TIME_MSG("&&&&&&&&&&&&&&&&  __NORTHUSA_EVO(�̱� �������)     &&&&&&&&&&&&&&&&&&&&&&&")
 
#define __MAX_CHAR_3			3		// �ִ� �ɸ��ͼ��� 5 ->3 ���� ����..
#define __KR_RENEWAL
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef  __KR_RENEWAL					//�ѱ� ����������� �븸 �ʱ� ������������� �۾��Ѵ�..

#define __TAW_RENEWAL					//�븸 �ʱ� ������ ����
#define __KCHS_BATTLECART__				//��ƲīƮ
#define __KRRE_START_ZONEPOS			//�ѱ� ������ �۾� AVATAR STB �� ������ ��ġ�� �����.. �ϵ��ڵ����� ����..

///////////////////////////  ���⼭ ���ʹ� ��� ������ ����� ����.///////////////////////////////////////////////////////////
 //(�κ�Ȯ��,���ϸ��� ������ �߰�,��ų�κ� ��������,��Ÿ���߰�)
#define __KRRE_NEW_AVATAR_DB			//�ѱ������� �۾��� ������ ����� ���ο� ���
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif



#include "Net_Prototype.h"
#include "ioDataPOOL.h"			//	Net_Prototype.h ���� �ڿ� !!!

#include "IOCPSocketSERVER.h"
#include "CClientSOCKET.h"
#include "CSocketWND.h"

typedef		classSTB	CGameSTB;	






//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC125CC8_4AFD_4EA7_A465_E7BFBABBD26E__INCLUDED_)

