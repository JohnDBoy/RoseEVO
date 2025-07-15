/**
 * \ingroup SHO_GS
 * \file	GS_ThreadZONE.h
 * \brief	CZoneFILEŬ������ ��ӹ�����, ���� ���� ��ϵ� ��ü ����/ó��
 */
#ifndef ThreadZONEH
#define ThreadZONEH
//---------------------------------------------------------------------------
// #include <Classes.hpp>
// #include <SyncObjs.hpp>
#include "classTHREAD.h"
#include "classSYNCOBJ.h"
#include "cdxHPC.h"
#include "OBJECT.h"
#include "ZoneSECTOR.h"
#include "ZoneFILE.h"
#include "CEconomy.h"
//---------------------------------------------------------------------------
#include "LIB_gsMAIN.h"
//---------------------------------------------------------------------------

struct tagZoneTRIGGER {
	t_HASHKEY	m_TriggerHASH;
	short		m_nTeamNO;
} ;

struct tagCalledMOB {
	DWORD	m_dwActionTIME;
	float	m_fCenterX;
	float	m_fCenterY;
	int		m_iRange;
	int		m_iCharNO;
	int		m_iCount;
	int		m_iTeamNO;
} ;


/**
 * \ingroup SHO_GS_LIB
 * \class	CZoneTHREAD
 * \author	wookSang.Jo
 * \brief	��(zone) ó�� �ϴ� ������ Ŭ����
 *			������ Ŭ������ ������ ����Ÿ�� �����ϰ� �ִ� CZoneFILEŬ������ ��ӹ���
 */
class CZoneTHREAD : public CZoneFILE
{
private  :
    classEVENT *m_pEvent;
    CDXHPC		m_Timer;
	DWORD		m_dwCurrentTIME;
	DWORD		m_dwTimeGetTIME;
	DWORD		m_dwCurAbsSEC;

	bool		m_bEnableREGEN;
	classDLLIST< CRegenPOINT* >	m_RegenLIST;

	CCriticalSection			m_csZoneMobCALLED;
	classSLLIST< tagCalledMOB >	m_MobCALLED;

	CCriticalSection			m_csZoneObjWAIT;
	classDLLIST< CGameOBJ* >	m_ObjWAIT;			// ���� �������� ����ϴ� ��ü...

    classDLLIST< CGameOBJ* >    m_ObjLIST;			// ���� �� �ִ� ��ü...

	CCriticalSection			m_csPacketLIST;
	classSLLIST< classPACKET* >	m_ToSendPacketLIST;
	classSLLIST< classPACKET* >	m_SendingPacketLIST;

	long	m_lMaxUSER;
	long	m_lUserCNT;
    bool	m_bEndExecuteFunc;
	DWORD	m_dwGlobalFLAGS;

	// Find Object var
	int		m_iFindSecIDX;
	int		m_iFindDistance;
	tPOINTF	m_FindPOS;
    POINTS  m_FindSECTOR;
    classDLLNODE< CGameOBJ* > *m_pFindObjNODE;
	CGameOBJ	*m_pObjFINDER;

	void	DeleteZoneOBJ ();

	CCriticalSection				m_csTriggerLIST;
	classSLLIST< tagZoneTRIGGER >	m_TriggerLIST;
	void	Proc_ZoneTRIGGER ();

protected:
	CZoneTHREAD *GetZonePTR ()						{	return	this;	}

	bool Add_RegenPOINT (FILE *fp, float fPosX, float fPosY);
	bool Load_Economy (FILE *fp);

    void Execute();
	
public:
	CEconomy	m_Economy;
	void	   *m_pListITEM;			

	//	���� ����ġ ó�� �κ�,
#ifdef	__ZONE_EXP_SETUP
	int		m_Exp_Lv;		// ����ġ ����.
	int		Get_Exp_LV();	// ����ġ ����
#endif

    CZoneTHREAD(bool CreateSuspended);
    ~CZoneTHREAD();

	int		Get_UserCNT ()		{	return m_lUserCNT;	}
	void	Inc_UserCNT ();
	void	Dec_UserCNT ();
/*
	// (20070312:����ö) : CLIB_GameSRV�� �μ� ���Ӽ��� ã�� �� �����Ͽ� �ϴ� cpp�� ����
	{	
		::InterlockedIncrement( &m_lUserCNT );
		if ( m_lUserCNT > m_lMaxUSER ) {
			m_lMaxUSER = m_lUserCNT;
			CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 2, m_lUserCNT );
		}
		CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 1, m_lUserCNT );
	}

	void	Dec_UserCNT ()		
	{	
		::InterlockedDecrement( &m_lUserCNT );
		CLIB_GameSRV::ExeAPI()->SetListItemINT( m_pListITEM, 1, m_lUserCNT );
	}
*/

    bool	Add_OBJECT( CGameOBJ *pObj );
	bool	Add_DIRECT( CGameOBJ *pObj );
    void	Sub_DIRECT( CGameOBJ *pObj, bool bSubFromREGEN=true );

	CZoneSECTOR *GetSECTOR (int iSectorX, int iSectorY);
	bool	UpdateSECTOR( CObjCHAR *pCHAR );

	CObjMOB*RegenMOB( float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO );
	bool	RegenCharacter( CObjMOB *pNewMOB, float fCenterX, float fCenterY, int iRange, int iIndex, int iTeamNO);
	CObjMOB*RegenCharacter(float fCenterX, float fCenterY, int iRange, int iIndex, int iCount, int iTeamNO, bool bImmediate );
    void	RegenCharacter( CRegenPOINT *pRegenPOINT, int iIndex, int iCount );
    void	SendPacketToSectors (CGameOBJ *pGameObj, classPACKET *pCPacket);
	void	SendTeamPacketToSectors (CGameOBJ *pGameObj, classPACKET *pCPacket, int iTeamNO);

    bool	AddObjectToSector   (CGameOBJ *pObj, BYTE btUpdateFLAG);
    void	SubObjectFromSector (CGameOBJ *pObj, char cUpdateFLAG);

    bool	Init (char *szBaseDIR, short nZoneNO);
    bool	Free (void);

	inline int Get_ItemBuyPRICE  (short nItemTYPE, short nItemNO, short nBuySkillVALUE)
	{	
		return m_Economy.Get_ItemBuyPRICE( nItemTYPE, nItemNO, nBuySkillVALUE );	
	}
	inline int Get_ItemSellPRICE (tagITEM &sITEM, short nSellSkillVALUE)
	{	
		return m_Economy.Get_ItemSellPRICE( sITEM, nSellSkillVALUE );
	}
	inline int Get_EconomyVAR	 (short nVarIDX)
	{
		return m_Economy.Get_EconomyVAR( nVarIDX );
	}

	DWORD	GetPassTIME ()				{	return	m_Timer.GetPassTIME();			}
	DWORD	GetCurrentTIME ()			{	return	m_dwCurrentTIME;				}
	DWORD	GetTimeGetTIME ()			{	return	m_dwTimeGetTIME;				}
	DWORD	GetCurAbsSEC ()				{	return	m_dwCurAbsSEC;					}
	DWORD	GetEconomyTIME ()			{	return	this->m_Economy.m_dwUpdateTIME;	}
	DWORD	GetGlobalFLAG()				{	return	this->m_dwGlobalFLAGS;			}
	int		GetGameObjCNT()				{	return	m_ObjLIST.GetNodeCount();		}

	void	BuyITEMs( tagITEM &sITEM )					{	m_Economy.BuyITEM( sITEM );					}
	void	SellITEMs( tagITEM *pITEM, int iQuantity )	{	m_Economy.SellITEM( *pITEM, iQuantity );	}

	bool	SendShout (CGameOBJ *pGameOBJ, classPACKET *pCPacket, int iTeamNo=0);
	bool	SendPacketToZONE (t_PACKET *pSendPacket);

	void	Send_EconomyDATA ();
	void	Send_GLOBAL_FLAG ();
	DWORD	Toggle_PK_FLAG ();
	void	Set_PK_FLAG( BYTE btOnOff );
	//	�迵ȯ �߰� �۾�
	////	���� PK �÷��� ���� ��´�,
	BOOL	Get_PK_FLAG();
	////	Ŭ�� �ʵ忡�� ����ϱ� ���� ó��. 	�迵ȯ : �߰�
//#ifdef __CLAN_FIELD_SET
	BOOL	All_User_Warp(DWORD pType);
//#endif

	CObjCHAR* FindFirstCHAR (tPOINTF &PosCENTER, int iDistance, CGameOBJ *pObjFINDER);
	CObjCHAR* FindFirstCHAR (CObjCHAR *pCenterCHAR, int iDistance, CGameOBJ *pObjFINDER);
	CObjCHAR* FindNextCHAR  ();

	void	Kill_AllMOB (classUSER *pUSER);
	//	���� �Ҽӵ� ��� ���͸� ���δ�.	�迵ȯ : �߰�
	void	Kill_AllMOB_Team(classUSER *pUSER,DWORD	p_Team_N);
	//	���� ���� ���� ��� 	�迵ȯ : �߰�
	DWORD	Get_Zone_MOB_Number();
	//	�ش� ���� ��� ������ HP ȸ�� 	�迵ȯ : �߰�
	BOOL	Set_MAX_HP_All_MOB(DWORD p_Team_N);
	//	���� ����ȣ ���� �ο� ���  �迵ȯ : �߰�
	DWORD	Get_Zone_Team_User_No(DWORD p_Team_N);

	bool	Toggle_REGEN ()						{	m_bEnableREGEN = !m_bEnableREGEN;	return m_bEnableREGEN;	}
	void	Set_REGEN ( bool bEnableRegen )		{	m_bEnableREGEN = bEnableRegen;	}
	bool	Get_REGEN( ) const { return m_bEnableREGEN; }

	void	Reset_REGEN ();
	short	GetRegenPointCNT ();
	short   GetRegenPointMOB( short nIndex, classUSER *pUSER );
	CRegenPOINT *GetRegenPOINT(short nIndex);
	short	CallRegenPointMOB( short nIndex );
	short	GetNearstRegenPOINT (tPOINTF &PosCENTER);

	void	Do_QuestTrigger( short nTeamNO, t_HASHKEY HashKEY );
} ;

#include "CRegenAREA.h"

//---------------------------------------------------------------------------
#endif
