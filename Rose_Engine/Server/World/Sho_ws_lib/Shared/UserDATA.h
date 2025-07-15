/**
 * \ingroup SHO_GS
 * \file	CUserData.h
 * \brief	���� ������� �ɸ����� ������ ����, ����
 */
#ifndef	__CUSERDATA_H
#define	__CUSERDATA_H
#include "CQuest.h"
#include "CInventory.h"
#include "CHotICON.h"
#include "MileageInv.h"
#include "Calculation.h"
#include <crtdbg.h>
#include "classLOG.h"
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#ifdef	__BORLANDC__
#include <crtdbg.h>
#endif

#pragma pack (push, 1)
//-------------------------------------------------------------------------------------------------
/**
 * \ingroup SHO_GS_LIB
 * \class	tagBasicINFO
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� �⺻ ���� ����Ÿ ���� ����ü
 */
struct tagBasicINFO {
	// NAME
	// SEX
	char	m_cBirthStone;		///< ź����
	char	m_cFaceIDX;			///< �� ���
	char	m_cHairIDX;			///< �Ӹ� ���
	short	m_nClass;			///< ���� ( 1~30 )
	char	m_cUnion;			///< �Ҽ� ���� ( 1~20 )
	char	m_cRank;			///< ��� ( 1~15 )
	char	m_cFame;			///< �� ( 0 ~ 99 )
	void Init (char cBirthStone, char cFaceIDX, char cHairIDX)
	{
		m_cBirthStone = cBirthStone;		
		m_cFaceIDX    = cFaceIDX;			
		m_cHairIDX    = cHairIDX;			

		m_nClass = 0;
		m_cUnion = 0;
		m_cRank  = 0;
		m_cFame  = 0;
	}
} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	tagBasicAbility
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� �⺻ �ɷ�ġ ����Ÿ ���� ����ü
 */
struct tagBasicAbility {
	union {
		struct {
			short	m_nSTR;			///< �ٷ� 1~200
			short	m_nDEX;			///< ��ø 1~200
			short	m_nINT;			///< ���� 1~200
			short	m_nCON;			///< concentration ���߷� 1~200
			short	m_nCHARM;		///< �ŷ� 1~200
			short	m_nSENSE;		///< ���� 1~200
		} ;
		short		m_nBasicA[ BA_MAX ];
	} ;
	void Init () 
	{
		m_nSTR = 10;
		m_nDEX = 10;
		m_nINT = 10;
		m_nCON = 10;
		m_nCHARM = 10;
		m_nSENSE = 10;
	}
} ;


#define	MAX_BASIC_ABLITY		500		/// 250 �⺻ �ɷ�ġ �ִ�ġ�� 300���� ����
//-------------------------------------
/*
#define	MAX_LEVEL				200		/// 200 �ִ� ������ 250���� ����
*/
// 2006.05.12/��뼺/���� 
#define	MAX_LEVEL				260		// ����
//-------------------------------------
#define	MAX_EXPERIENCE			0x7fffffff

#define	MAX_STAMINA				5000
//#define	MAX_STAMINA				20000
//#define	GREEN_STAMINA			10000	// �̻��̸� ���λ�
//#define	YELLOW_STAMINA			3000	// �̻��̸� �����
//#define	RED_STAMINA				0		// �̻��̸� ������


#define	MAX_MAINTAIN_STATUS		40		// 8 * 50 = 400
struct tagMaintainSTATUS {
	DWORD	m_dwExpiredSEC;
	short	m_nValue;
	short	m_nDummy;
} ;
//struct tagMaintainDATA {
//	tagMaintainSTATUS	m_STATUS[ MAX_MAINTAIN_STATUS ];
//
//	bool UpdateSTATUS (DWORD dwCurAbsSEC, int iAbilityType, int i10Min, short nValue)
//	{
//		short nIndex = iAbilityType - (int)AT_MAINTAIN_ABILITY;
//		if ( nIndex >= 0 && nIndex < MAX_MAINTAIN_STATUS ) {
//			// ����ð� + �Է°� * 10��
//			if ( this->m_STATUS[ nIndex ].m_dwExpiredSEC > dwCurAbsSEC ) {
//				// �ð� �߰�...
//				this->m_STATUS[ nIndex ].m_dwExpiredSEC += ( i10Min * 10 * 60 );
//			} else {
//				// �ð� ����...
//				this->m_STATUS[ nIndex ].m_dwExpiredSEC = dwCurAbsSEC + ( i10Min * 10 * 60 );
//			}
//			this->m_STATUS[ nIndex ].m_nValue = nValue;
//			return true;
//		}
//		return false;
//	}
//	bool IsBankFREE (DWORD dwCurAbsSEC)
//	{
//		return ( this->m_STATUS[ AT_BANK_FREE -AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC );
//	}
//	bool IsBankAddON (DWORD dwCurAbsSEC)
//	{
//		return ( this->m_STATUS[ AT_BANK_ADDON-AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC );
//	}
//	short GetStoreSKIN (DWORD dwCurAbsSEC)
//	{
//		if ( this->m_STATUS[ AT_STORE_SKIN-AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC )
//			return this->m_STATUS[ AT_STORE_SKIN-AT_MAINTAIN_ABILITY ].m_nValue;
//		return 0;
//	}
//} ;

/**
 * \ingroup SHO_GS_LIB
 * \class	tagGrowAbility_old
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� ���� �ɷ�ġ ����Ÿ ���� ����ü
 */
struct tagGrowAbility_old {
	union {
		struct {
			short	m_nHP;				///< 0~2000
			short	m_nMP;				///< 0~1000

			long	m_lEXP;				///< ����ġ 1~100000
			short	m_nLevel;			///< 0~250
			short	m_nBonusPoint;		///< 1~999
			short	m_nSkillPoint;		///< 1~
			BYTE	m_btBodySIZE;		///< ����ũ��
			BYTE	m_btHeadSIZE;		///< �Ӹ�ũ��
			long	m_lPenalEXP;		///< �߰� ����ġ...

			short	m_nFameG;			///< ��������	: ����Ʈ�� ���� ������	: 2004.5.27 �߰�
			short	m_nFameB;			///< �������� : ����Ʈ�� ���� ������	: 2004.5.27 �߰�
			short	m_nJoHapPOINT[ MAX_UNION_COUNT ];	// ���� ����Ʈ		: 2004.5.27 �߰�

			//int	m_iGuildNO;			///< ��� ��ȣ	: 2004.5.27 �߰�
			//short	m_nGuildCNTRB;		///< ��� �⿩��	: 2004.5.27 �߰�
			//BYTE	m_btGuildPOS;		///< ��� ����	: 2004.5.27 �߰�
			int		m_iNULL1;
			short	m_nNULL2;
			BYTE	m_btNULL3;

			short	m_nPKFlag;			// 2004. 6. 17 �߰�..
			short	m_nSTAMINA;			// 2004. 7. 23 �߰�..
		/*
			char	m_cChaos;			// ����
			short	m_nBattle_LV;		// ��Ʋ����
			short	m_nPK_LV;			//
		*/
//#ifdef	__INC_PLATINUM
			tagMaintainSTATUS	m_STATUS[ MAX_MAINTAIN_STATUS ];
//#endif
		} ;
	};
};

/**
 * \ingroup SHO_GS_LIB
 * \class	tagGrowAbility
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� ���� �ɷ�ġ ����Ÿ ���� ����ü
 */
struct tagGrowAbility {
	union {
		struct {
			short	m_nHP;				///< 0~2000
			short	m_nMP;				///< 0~1000

			long	m_lEXP;				///< ����ġ 1~100000
			short	m_nLevel;			///< 0~250
			short	m_nBonusPoint;		///< 1~999
			short	m_nSkillPoint;		///< 1~
			BYTE	m_btBodySIZE;		///< ����ũ��
			BYTE	m_btHeadSIZE;		///< �Ӹ�ũ��
			long	m_lPenalEXP;		///< �߰� ����ġ...

			short	m_nFameG;			///< ��������	: ����Ʈ�� ���� ������	: 2004.5.27 �߰�
			short	m_nFameB;			///< �������� : ����Ʈ�� ���� ������	: 2004.5.27 �߰�
			short	m_nJoHapPOINT[ MAX_UNION_COUNT ];	// ���� ����Ʈ		: 2004.5.27 �߰�

			//int	m_iGuildNO;			///< ��� ��ȣ	: 2004.5.27 �߰�
			//short	m_nGuildCNTRB;		///< ��� �⿩��	: 2004.5.27 �߰�
			//BYTE	m_btGuildPOS;		///< ��� ����	: 2004.5.27 �߰�
			int		m_iNULL1;
			short	m_nNULL2;
			BYTE	m_btNULL3;

			short	m_nPKFlag;			// 2004. 6. 17 �߰�..
			short	m_nSTAMINA;			// 2004. 7. 23 �߰�..
		/*
			char	m_cChaos;			// ����
			short	m_nBattle_LV;		// ��Ʋ����
			short	m_nPK_LV;			//
		*/
//#ifdef	__INC_PLATINUM
			tagMaintainSTATUS	m_STATUS[ MAX_MAINTAIN_STATUS ];
//#endif
#ifdef __KCHS_BATTLECART__
#pragma message( "~~~~~~~~~~~~~~~~~~~ ��Ʋ īƮ HP, ��Ÿ�� �߰� ~~~~~~~~~~~" )
			short	m_nPatHP;
			DWORD	m_dwPatCoolTIME;
#else
#pragma message( "~~~~~~~~~~~~~~~~~~~ ��Ʋ īƮ ���� ~~~~~~~~~~~" )
#endif
		} ;
		BYTE	m_pDATA[ 1 ];
	} ;





	void Init () 
	{
		::ZeroMemory( m_pDATA, sizeof(tagGrowAbility) );
		m_nHP = 50;
		m_nMP = 40;

		m_nLevel = 1;
		m_lEXP = 0;
		m_btBodySIZE = 100;		// ����ũ��
		m_btHeadSIZE = 100;		// �Ӹ�ũ��
		m_nSTAMINA = MAX_STAMINA;

#ifndef	__SERVER
		m_nBonusPoint = 0;
		m_nSkillPoint = 0;
		m_lPenalEXP = 0;
		m_nFameG = m_nFameB = 0;
		m_iGuildNO = m_nGuildCNTRB = m_btGuildPOS = 0;
		::ZeroMemory( m_nUnionPOINT, sizeof(m_nUnionPOINT) );
		m_nPKFlag = 0;
		m_nPatHP = 0;
		m_dwPatCoolTIME = 0;
#endif
	}



//#ifdef	__INC_PLATINUM
	bool UpdateSTATUS (DWORD dwCurAbsSEC, int iAbilityType, int i10Min, short nValue)
	{
		short nIndex = iAbilityType - (int)AT_MAINTAIN_ABILITY;
		if ( nIndex >= 0 && nIndex < MAX_MAINTAIN_STATUS ) {
			// ����ð� + �Է°� * 10��
			if ( this->m_STATUS[ nIndex ].m_dwExpiredSEC > dwCurAbsSEC ) {
				// �ð� �߰�...
				this->m_STATUS[ nIndex ].m_dwExpiredSEC += ( i10Min * 10 * 60 );
			} else {
				// �ð� ����...
				this->m_STATUS[ nIndex ].m_dwExpiredSEC = dwCurAbsSEC + ( i10Min * 10 * 60 );
			}
			this->m_STATUS[ nIndex ].m_nValue = nValue;
			return true;
		}
		return false;
	}
	bool IsBankFREE (DWORD dwCurAbsSEC)
	{
		return ( this->m_STATUS[ AT_BANK_FREE -AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC );
	}
	bool IsBankAddON (DWORD dwCurAbsSEC)
	{
		return ( this->m_STATUS[ AT_BANK_ADDON-AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC );
	}
	short GetStoreSKIN (DWORD dwCurAbsSEC)
	{
		if ( this->m_STATUS[ AT_STORE_SKIN-AT_MAINTAIN_ABILITY ].m_dwExpiredSEC > dwCurAbsSEC )
			return this->m_STATUS[ AT_STORE_SKIN-AT_MAINTAIN_ABILITY ].m_nValue;
		return 0;
	}
//#endif
} ;


/// �ƹ�Ÿ ������ų - �������� 120���� ���� ��ȹ���� ����� 90��
#ifdef	__MILEAGE_SKILL_USED
	#define	MAX_LEARNED_SKILL_CNT			362	// ũ������.
#else 
	#ifdef  __KRRE_NEW_AVATAR_DB
		#define	MAX_LEARNED_SKILL_CNT			162
	#else
		#define	MAX_LEARNED_SKILL_CNT			120
	#endif
#endif

#define	MAX_LEARNED_SKILL_PAGE			4
#define	MAX_LEARNED_SKILL_PER_PAGE		( MAX_LEARNED_SKILL_CNT/MAX_LEARNED_SKILL_PAGE )


//	2006�� 7�� 6�� ����� ���ϸ��� ��ų �߰� ����
//	�迵ȯ : �߰�. ��ų ���� ����
#define MAX_JOB_SKILL 60
#define MAX_PATS_SKILL 30
#define MAX_UNIQUE_SKILL 30
#define MAX_COMMON_SKILL 42

#ifdef	__MILEAGE_SKILL_USED
	#define MAX_SKILL_TAB 4       //������ų-0,������ų-1,����ũ��ų-2,���ϸ�����ų - 3
	#define MAX_MILEAGE_SKILL 200		// ����
	#define MAX_SKILL_INV_CNT (MAX_JOB_SKILL+MAX_PATS_SKILL+MAX_UNIQUE_SKILL + MAX_MILEAGE_SKILL)
#else
	#define MAX_SKILL_TAB 3      //������ų-0,������ų-1,����ũ��ų-2
	#define MAX_SKILL_INV_CNT (MAX_JOB_SKILL+MAX_PATS_SKILL+MAX_UNIQUE_SKILL)
#endif

#define BEGIN_JOB_SKILL_INDEX			0
#define BEGIN_PATS_SKILL_INDEX			MAX_JOB_SKILL
#define BEGIN_UNIQUE_SKILL_INDEX		MAX_JOB_SKILL+MAX_PATS_SKILL
#define BEGIN_MILEAGE_SKILL_INDEX		MAX_JOB_SKILL+MAX_PATS_SKILL+MAX_UNIQUE_SKILL

/**
 * \ingroup SHO_GS_LIB
 * \class	tagSkillAbility
 * \author	wookSang.Jo
 * \brief	����� �ɸ��Ͱ� ���ų� ��� ������ ��ų�ɷ�ġ ����Ÿ ���� ����ü
 */


//	2006�� 7�� 6�� ����� ���ϸ��� ��ų �߰� ����
//	�迵ȯ : ���ϸ��� �迭 �߰�
#ifdef	__MILEAGE_SKILL_USED
	struct SkillInv
	{
		short m_nJobSkill[MAX_JOB_SKILL];           //���� ��ų..
		short m_nPatsSKill[MAX_PATS_SKILL];         //���� ��ų
		short m_nUniqueSkill[MAX_UNIQUE_SKILL];		//����ũ ��ų
		short m_nMileageSkill[MAX_MILEAGE_SKILL];	//���ϸ��� ��ų : �迵ȯ �߰�.
		//short m_nCommonSkill[MAX_COMMON_SKILL]; //�Ϲݽ�ų..
	};
#else
	struct SkillInv
	{
		short m_nJobSkill[MAX_JOB_SKILL];           //���� ��ų..
		short m_nPatsSKill[MAX_PATS_SKILL];         //���� ��ų
		short m_nUniqueSkill[MAX_UNIQUE_SKILL];		//����ũ ��ų
		//short m_nCommonSkill[MAX_COMMON_SKILL]; //�Ϲݽ�ų..
	};
#endif
	
struct tagSkillAbility 
{
	union
	{
		short	m_nSkillINDEX[ MAX_LEARNED_SKILL_CNT ];
		short	m_nPageIndex [ MAX_LEARNED_SKILL_PAGE ][ MAX_LEARNED_SKILL_PER_PAGE ];	// ����, ���/����, �нú� ��ų ���� 10������ ����.
		SkillInv m_nSkillInv;
	} ;
	void Init ()
	{
		::ZeroMemory( m_nSkillINDEX,	sizeof(short) * MAX_LEARNED_SKILL_CNT );

#ifdef __KRRE_NEW_AVATAR_DB


		return;
#endif

		m_nSkillINDEX[ 0 ] = 11;	// �ɱ�
		m_nSkillINDEX[ 1 ] = 12;	// �ݱ�
		m_nSkillINDEX[ 2 ] = 16;	// �Ϲݰ���
		m_nSkillINDEX[ 3 ] = 20;	// �ŷ���û

		m_nSkillINDEX[  4 ] = 49;	// ȭ��
		m_nSkillINDEX[  5 ] = 50;	// �ڼ�
		m_nSkillINDEX[  6 ] = 48;	// ����
		m_nSkillINDEX[  7 ] = 47;	// ������
		m_nSkillINDEX[  8 ] = 46;	// ����
		m_nSkillINDEX[  9 ] = 43;	// �λ�
		m_nSkillINDEX[ 10 ] = 42;	// �λ�
		m_nSkillINDEX[ 11 ] = 41;	// �λ�
		m_nSkillINDEX[ 12 ] = 181;	// �λ�
		m_nSkillINDEX[ 13 ] = 22;	// ���� Ÿ����
	}
} ;


/// ����Ʈ ������ ...

// #define	__APPLY_EXTAND_QUEST_VAR

#define	QUEST_PER_PLAYER			10			// ���δ� �ִ� ��� ���� ����Ʈ��...

#define	QUEST_EPISODE_VAR_CNT		5			// ���Ǽҵ� ���� ���� ����
#define	QUEST_JOB_VAR_CNT			3			// ������ ���� ���� ����
#define	QUEST_PLANET_VAR_CNT		7			// �༺��...
#define	QUEST_UNION_VAR_CNT			10			// ���պ�...

// Ŭ������ ���� ����Ʈ ���� ���� ���� ó��
//	�迵ȯ �߰�.
#ifdef __CLAN_WAR_SET
	#define	QUEST_CLANWAR_VAR_CNT		10			// Ŭ���� ���� ���� ����...
#endif

#ifdef	__APPLY_EXTAND_QUEST_VAR
	#define	QUEST_ETC_VAR_CNT			53
	#define	QUEST_SWITCH_CNT			(512+512)	// ��ü ����Ʈ ���� ����ġ��..
#else
	#define	QUEST_SWITCH_CNT			(512    )	// ��ü ����Ʈ ���� ����ġ��..
#endif
#define	QUEST_SWITCH_GROUP_CNT	  ( QUEST_SWITCH_CNT/sizeof(DWORD) )

// #define	MAX_SQUEST_SWITCH	2048
/**
 * \ingroup SHO_GS_LIB
 * \class	tagQuestData
 * \author	wookSang.Jo
 * \brief	����� �ɸ��Ͱ� �������̰ų� �Ϸ�� ����Ʈ�� ����Ÿ ���� ����ü
 */

//	Ŭ������ ���� ����Ʈ ���� ���� �߰�
//	�迵ȯ �߰�.
#ifdef __CLAN_WAR_SET
struct tagQuestData {
private:
	enum {	BIT_SHIFT=3, WHICH_BIT=0x07 };
	BYTE GetBit (int iIndex)
	{
		return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & g_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;
	}
	void SetBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= g_btSwitchBitMask[ iIndex & WHICH_BIT ];
	}
	void ClearBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(g_btSwitchBitMask[ iIndex & WHICH_BIT ]);
	}
	BYTE FlipBit (int iIndex)
	{
		if ( this->GetBit( iIndex ) )
			this->ClearBit( iIndex );
		else
			this->SetBit ( iIndex );

		return this->GetBit( iIndex );
	}

public :
	short		m_nEpisodeVAR[ QUEST_EPISODE_VAR_CNT	];		// 2 * 5
	short		m_nJobVAR	 [ QUEST_JOB_VAR_CNT		];		// 2 * 3
	short		m_nPlanetVAR [ QUEST_PLANET_VAR_CNT		];		// 2 * 7
	short		m_nUnionVAR  [ QUEST_UNION_VAR_CNT		];		// 2 * 10
	CQUEST		m_QUEST		 [ QUEST_PER_PLAYER			];		// 660 : Ŭ������Ʈ �߰�
	union {
		DWORD	m_dwSWITCHES [ QUEST_SWITCH_CNT/32		];		
		BYTE	m_btSWITCHES [ QUEST_SWITCH_CNT/8		];		// 1024 / 8 = 128
	} ;

#ifdef	__APPLY_EXTAND_QUEST_VAR
	short		m_nEtcVAR	 [ QUEST_ETC_VAR_CNT		];		// 2 * 50
#endif

	short		m_nClanWarVAR  [QUEST_CLANWAR_VAR_CNT];	// 2*10 = 20 BYTE 

	void Init ()
	{
		::ZeroMemory( m_nEpisodeVAR,	sizeof(short) * QUEST_EPISODE_VAR_CNT	);
		::ZeroMemory( m_nJobVAR,		sizeof(short) * QUEST_JOB_VAR_CNT		);
		::ZeroMemory( m_nPlanetVAR,		sizeof(short) * QUEST_PLANET_VAR_CNT	);
		::ZeroMemory( m_nUnionVAR,		sizeof(short) * QUEST_UNION_VAR_CNT		);
#ifdef	__APPLY_EXTAND_QUEST_VAR
		::ZeroMemory( m_nEtcVAR,		sizeof(short) * QUEST_ETC_VAR_CNT		);
#endif
		this->ClearAllSwitchs ();

		//	Ŭ������ �ʱ�ȭ
		::ZeroMemory( m_nClanWarVAR, sizeof(short) * QUEST_CLANWAR_VAR_CNT);

		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			m_QUEST[ nI ].Init ();
		}
	}
	void CheckExpiredTIME ()
	{
		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			if ( this->m_QUEST[ nI ].GetID() ) {
				this->m_QUEST[ nI ].CheckExpiredTIME ();
			}
		}
	}

	//-------------------------------------------------------------------------------------------------
	void ClearAllSwitchs ()
	{
		::ZeroMemory (m_dwSWITCHES, sizeof(m_dwSWITCHES));
	}
	void Set_SWITCH (int iSwitchNO, int iValue)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return;

		if ( iValue )
			this->SetBit( iSwitchNO );
		else
			this->ClearBit( iSwitchNO );
	}
	int  Get_SWITCH (int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->GetBit( iSwitchNO );
	}
	int  Flip_SWITCH(int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->FlipBit( iSwitchNO );
	}
};
#else

struct tagQuestData {
private:
	enum {	BIT_SHIFT=3, WHICH_BIT=0x07 };
	BYTE GetBit (int iIndex)
	{
		return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & g_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;
	}
	void SetBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= g_btSwitchBitMask[ iIndex & WHICH_BIT ];
	}
	void ClearBit (int iIndex)
	{
		m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(g_btSwitchBitMask[ iIndex & WHICH_BIT ]);
	}
	BYTE FlipBit (int iIndex)
	{
		if ( this->GetBit( iIndex ) )
			this->ClearBit( iIndex );
		else
			this->SetBit ( iIndex );

		return this->GetBit( iIndex );
	}

public :
	short		m_nEpisodeVAR[ QUEST_EPISODE_VAR_CNT	];		// 2 * 5
	short		m_nJobVAR	 [ QUEST_JOB_VAR_CNT		];		// 2 * 3
	short		m_nPlanetVAR [ QUEST_PLANET_VAR_CNT		];		// 2 * 7
	short		m_nUnionVAR  [ QUEST_UNION_VAR_CNT		];		// 2 * 10
	CQUEST		m_QUEST		 [ QUEST_PER_PLAYER			];		// 660 : Ŭ������Ʈ �߰�
	union {
		DWORD	m_dwSWITCHES [ QUEST_SWITCH_CNT/32		];		
		BYTE	m_btSWITCHES [ QUEST_SWITCH_CNT/8		];		// 1024 / 8 = 128
	} ;

#ifdef	__APPLY_EXTAND_QUEST_VAR
	short		m_nEtcVAR	 [ QUEST_ETC_VAR_CNT		];		// 2 * 50
#endif

	void Init ()
	{
		::ZeroMemory( m_nEpisodeVAR,	sizeof(short) * QUEST_EPISODE_VAR_CNT	);
		::ZeroMemory( m_nJobVAR,		sizeof(short) * QUEST_JOB_VAR_CNT		);
		::ZeroMemory( m_nPlanetVAR,		sizeof(short) * QUEST_PLANET_VAR_CNT	);
		::ZeroMemory( m_nUnionVAR,		sizeof(short) * QUEST_UNION_VAR_CNT		);
#ifdef	__APPLY_EXTAND_QUEST_VAR
		::ZeroMemory( m_nEtcVAR,		sizeof(short) * QUEST_ETC_VAR_CNT		);
#endif
		this->ClearAllSwitchs ();

		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			m_QUEST[ nI ].Init ();
		}
	}
	void CheckExpiredTIME ()
	{
		for (short nI=0; nI<QUEST_PER_PLAYER; nI++) {
			if ( this->m_QUEST[ nI ].GetID() ) {
				this->m_QUEST[ nI ].CheckExpiredTIME ();
			}
		}
	}

	//-------------------------------------------------------------------------------------------------
	void ClearAllSwitchs ()
	{
		::ZeroMemory (m_dwSWITCHES, sizeof(m_dwSWITCHES));
	}
	void Set_SWITCH (int iSwitchNO, int iValue)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return;

		if ( iValue )
			this->SetBit( iSwitchNO );
		else
			this->ClearBit( iSwitchNO );
	}
	int  Get_SWITCH (int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->GetBit( iSwitchNO );
	}
	int  Flip_SWITCH(int iSwitchNO)
	{
		if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_CNT )
			return -1;

		return this->FlipBit( iSwitchNO );
	}
} ;
#endif



// sizeof(tagITME) = 6+8 = 14
// 14 * 40 * 4 = 2240
//#ifdef	__INC_PLATINUM
	#define	BANKSLOT_JPN_DEFAULT	40
	#define	BANKSLOT_JPN_EXTRA		120

	#define	BANKSLOT_DEFAULT		90
	#define	BANKSLOT_ADDON			30
	#define	BANKSLOT_PLATINUM		40
//#else
//	#define	BANKSLOT_DEFAULT		90
//	#define	BANKSLOT_ADDON			0
//	#define	BANKSLOT_PLATINUM		0
//#endif
#define	BANKSLOT_TOTAL			( BANKSLOT_DEFAULT+BANKSLOT_ADDON+BANKSLOT_PLATINUM )

#define	BANKSLOT_PLATINUM_0		( BANKSLOT_DEFAULT+BANKSLOT_ADDON )
/**
 * \ingroup SHO_GS_LIB
 * \class	tagSkillAbility
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� â�� ����Ÿ ���� ����ü
 */
struct tagBankData {
	tagITEM		m_ItemLIST[	BANKSLOT_TOTAL ];
	__int64		m_i64ZULY;

	void Init ()
	{
		::ZeroMemory( m_ItemLIST, sizeof(m_ItemLIST) );
		m_i64ZULY = 0;
	}

	short Get_EmptySlot (short nStartSlotNO, short nEndSlotNO);
	short Add_ITEM(tagITEM &sITEM, short nStartSlotNO, short nEndSlotNO);
	short Add_ITEM(short nSlotNO, tagITEM &sITEM);
	short Sub_ITEM(short nSlotNO, tagITEM &sITEM);
	void  Set_ITEM(short nSlotNO, tagITEM &sITEM)
	{
		m_ItemLIST[ nSlotNO ] = sITEM;
	}
} ;


#define	MAX_WISH_ITEMS			30				// ���� ����ϴ� �� ������ �ִ� ����
/**
 * \ingroup SHO_GS_LIB
 * \class	tagWishLIST
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� ���� ��� ��� ����Ÿ ���� ����ü
 */
struct tagWishLIST {
	tagBaseITEM	m_WishITEM[ MAX_WISH_ITEMS ];

	void Init ()
	{
		::ZeroMemory( m_WishITEM,	sizeof(m_WishITEM) );
	}
	bool Set(BYTE btSlotNO, tagBaseITEM &sITEM)
	{
		if ( btSlotNO >= MAX_WISH_ITEMS )
			return false;

		this->m_WishITEM[ btSlotNO ] = sITEM;
		return true;
	}
} ;

//-------------------------------------------------------------------------------------------------
#pragma pack (pop)


/**
 * \ingroup SHO_GS_LIB
 * \class	tagBattleAbility
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� ���� �ɷ�ġ ������ ���� ����ü - ��� ������� �ʰ� �ǽð� ����
 */
struct tagBattleAbility {
	short	m_nMaxHP;
	short	m_nMaxMP;
	short	m_nATT;			///< ���ݷ� 1~200
	short	m_nDEF;			///< ���� 1~200
	short	m_nMAG;			///< ������ 1~200
	short	m_nHIT;			///< ���߷� 1~500
	short	m_nRES;			///< �׸��� 1~200
	short	m_nAVOID;		///< ȸ�Ƿ� 1~999
	short	m_nSPD;			///< �̵��� 1~50
	short	m_nMaxWEIGHT;	///< weight �����ۼ�����.
	short	m_nWEIGHT;		///< ���� �����ѷ�

//	int		m_iDefQuality;	///< ��ü �� ǰ��
	int		m_iDefDurabity;	///< ��ü �� ��������
	int		m_iDefGrade;	///< ��ü �� �����

	int		m_iCritical;	///< ũ��Ƽ�� ��ġ
	float	m_fRateUseMP;	///< MP ���� ������ ���Ǿ��� MP �Ҹ�� ����.
#ifdef __KCHS_BATTLECART__
	short	m_nPatMaxHP;
#endif
	short	m_nImmunity;	///< �鿪��

} ;


//-------------------------------------------------------------------------------------------------



/**
 * \ingroup SHO_GS_LIB
 * \class	CUserDATA
 * \author	wookSang.Jo
 * \brief	����� �ɸ����� �⺻ ������ ���� ����ü
 */
class CUserDATA 
{
public:
	tagBasicINFO		m_BasicINFO;					///< �⺻ ���� ������...	
	tagBasicAbility		m_BasicAbility;					///< �⺻ �ɷ�ġ ������...
	tagGrowAbility		m_GrowAbility;					///< ���� �ɷ�ġ ������ ...
	tagSkillAbility		m_Skills;						///< �ƹ�Ÿ ������ų
	tagQuestData		m_Quests;						///< quest
	CInventory			m_Inventory;					///< inventory
	CHotICONS			m_HotICONS;						///< ��������� ��� ����...
	CMileageInv         m_MileageInv;                   //  <���ϸ��� �κ�....
	CPatsEquipInv     m_PatsEquip;						//���������κ�..


	tagBankData		m_Bank;							///< ���� ����Ÿ.
	tagWishLIST			m_WishLIST;						///< ���� �������� ���� ���Խ� ����� ������� �����۸��.

	tagBattleAbility	m_Battle;						///< ���� �ɷ�ġ ������ - ���Ǿ��� ����

	int					m_iAddValue	  [ AT_MAX ];		///< ���� �����ۿ� ���� ������ ��ġ : ���Ǿ���.
	short				m_nPassiveRate[ AT_MAX ];		///< �нú� ��ų�� ���� ������ ����

	BYTE				m_btRecoverHP;					///< == m_PassiveSkill[ PST_RECOVER_HP ] + this->m_iAddValue[ AT_RECOVER_HP ]
	BYTE				m_btRecoverMP;					///< == m_PassiveSkill[ PST_RECOVER_MP ] + this->m_iAddValue[ AT_RECOVER_MP ]

	int					m_iDropRATE;					///< ��� Ȯ��

	short				m_PassiveAbilityFromValue[ BA_MAX ];	///< �⺻ �ɷ�ġ :: ��ġ �нú� ��ų�� ���� ������ ��
	short				m_PassiveAbilityFromRate [ BA_MAX ];	///< �⺻ �ɷ�ġ :: ���� �нú� ��ų�� ���� ������ ��
	short				m_nPassiveAttackSpeed;

	short				m_nPatHP_Mode;
private:
	inline int	GetPassiveSkillValue(short nAbilityTYPE)
	{
		return this->m_iAddValue[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillValue(short nAbilityTYPE, short nValue)
	{
		this->m_iAddValue[ nAbilityTYPE ] += nValue;
		g_LOG.OutputString(LOG_DEBUG,"VALUE TYPE %d ADD:%d VALUE:%d\n",nAbilityTYPE,nValue,this->m_iAddValue[ nAbilityTYPE ]);
	}
	inline short GetPassiveSkillRate(short nAbilityTYPE)
	{
		return this->m_nPassiveRate[ nAbilityTYPE ];
	}
	inline void AddPassiveSkillRate(short nAbilityTYPE, short nRate)
	{
		this->m_nPassiveRate[ nAbilityTYPE ] += nRate;
		g_LOG.OutputString(LOG_DEBUG,"RATE TYPE %d ADD:%d RATE:%d\n",nAbilityTYPE,nRate,this->m_nPassiveRate[ nAbilityTYPE ]);

	}

	void	Cal_AddAbility (tagITEM sITEM, short nItemTYPE);

	void	Cal_AddPatAbility (tagITEM &sITEM, short nItemTYPE);
	void	Cal_AddAbility ();

	void	Cal_RecoverHP ();
	void	Cal_RecoverMP ();

	int		Cal_MaxHP ();
	int		Cal_MaxMP ();

	int		Cal_HIT ();
	int		Cal_HIT_KRNEW ();
	int		Cal_DEFENCE();
	int		Cal_DEFENCE_KRNEW();

	int		Cal_RESIST ();
	int		Cal_RESIST_KRNEW ();
	int		Cal_MaxWEIGHT ();
	int		Cal_AvoidRATE ();
	int		Cal_CRITICAL ();
	int		Cal_IMMUNITY ();


	short	GetPassiveSkillAttackPower( int iCurAP, short nRightWeaponItemNo );
protected :
	short	GetPassiveSkillAttackSpeed( float fCurSpeed, short nRightWeaponItemNo );

protected :
	float	Cal_RunSPEED ();
	void	Cal_BattleAbility ();
	int		Cal_ATTACK ();
	int		Cal_ATTACK_KRNEW ();

public :
	short GetCur_HP ()				{	return	this->m_GrowAbility.m_nHP;			}	/// �����
	short GetCur_MP ()				{	return	this->m_GrowAbility.m_nMP;			}
	virtual short GetCur_WEIGHT ()			{	return	this->m_Battle.m_nWEIGHT;			}

	int	  GetCur_LEVEL ()			{	return	this->m_GrowAbility.m_nLevel;		}

	int	  GetDef_ATK ()				{	return	this->m_Battle.m_nATT;				}	/// ���ݷ�
	int	  GetDef_DEF ()				{	return	this->m_Battle.m_nDEF;				}	/// ����
	int	  GetDef_RES ()				{	return	this->m_Battle.m_nRES;				}	/// �׸���
	int	  GetDef_HIT ()				{	return	this->m_Battle.m_nHIT;				}
	int	  GetDef_AVOID ()			{	return	this->m_Battle.m_nAVOID;			}	/// ȸ�Ƿ�
	int	  GetDef_CRITICAL()			{	return	this->m_Battle.m_iCritical;			}
#ifdef	__APPLY_2ND_JOB
	int	  GetDef_IMMUNITY()			{	return	this->m_Battle.m_nImmunity;			}	/// �鿪��
#endif

	int	  GetCur_BIRTH()			{	return  this->m_BasicINFO.m_cBirthStone;	}	/// ź����
	int	  GetCur_RANK()				{	return	this->m_BasicINFO.m_cRank;			}	/// ���
	int	  GetCur_JOHAP ()			{	return	this->m_BasicINFO.m_cUnion;			}	/// �Ҽ�
	int	  GetCur_FAME ()			{	return	this->m_BasicINFO.m_cFame;			}	/// ��
	int	  GetCur_JOB ()				{	return	this->m_BasicINFO.m_nClass;			}	/// ����

	int	  GetCur_EXP ()				{	return	this->m_GrowAbility.m_lEXP;			}
	int	  GetCur_BonusPOINT ()		{	return	this->m_GrowAbility.m_nBonusPoint;	}
	int	  GetCur_SkillPOINT ()		{	return	this->m_GrowAbility.m_nSkillPoint;	}
	BYTE  GetCur_HeadSIZE()			{	return	this->m_GrowAbility.m_btHeadSIZE;	}
	BYTE  GetCur_BodySIZE()			{	return	this->m_GrowAbility.m_btBodySIZE;	}

	/// �⺻ �ɷ�ġ...
	int	  GetDef_STR ()				{	return	this->m_BasicAbility.m_nSTR;		}	/// �ٷ�
	int	  GetDef_DEX ()				{	return	this->m_BasicAbility.m_nDEX;		}	/// ��ø
	int	  GetDef_INT ()				{	return	this->m_BasicAbility.m_nINT;		}	/// ����
	int	  GetDef_CON ()				{	return	this->m_BasicAbility.m_nCON;		}	/// ����
	int	  GetDef_CHARM ()			{	return	this->m_BasicAbility.m_nCHARM;		}	/// �ŷ�
	int	  GetDef_SENSE ()			{	return	this->m_BasicAbility.m_nSENSE;		}	/// ����

	void  SetDef_STR(short nValue)	{	this->m_BasicAbility.m_nSTR=nValue;			}
	void  SetDef_DEX(short nValue)	{	this->m_BasicAbility.m_nDEX=nValue;			}
	void  SetDef_INT(short nValue)	{	this->m_BasicAbility.m_nINT=nValue;			}
	void  SetDef_CON(short nValue)	{	this->m_BasicAbility.m_nCON=nValue;			}
	void  SetDef_CHARM(short nValue){	this->m_BasicAbility.m_nCHARM=nValue;		}
	void  SetDef_SENSE(short nValue){	this->m_BasicAbility.m_nSENSE=nValue;		}

	int	  GetCur_STR ()				{	return	( GetDef_STR()   + m_iAddValue[ AT_STR		] + m_PassiveAbilityFromRate[ AT_STR  -AT_STR ] );	}	// �ٷ�
	int	  GetCur_DEX ()				{	return	( GetDef_DEX()   + m_iAddValue[ AT_DEX		] + m_PassiveAbilityFromRate[ AT_DEX  -AT_STR ] );	}	// ��ø
	int	  GetCur_INT ()				{	return	( GetDef_INT()   + m_iAddValue[ AT_INT		] + m_PassiveAbilityFromRate[ AT_INT  -AT_STR ] );	}	// ����
	int	  GetCur_CON ()				{	return	( GetDef_CON()   + m_iAddValue[ AT_CON		] + m_PassiveAbilityFromRate[ AT_CON  -AT_STR ] );	}	// ����
	int	  GetCur_CHARM ()			{	return	( GetDef_CHARM() + m_iAddValue[ AT_CHARM	] + m_PassiveAbilityFromRate[ AT_CHARM-AT_STR ] );	}	// �ŷ�
	int	  GetCur_SENSE ()			{	return	( GetDef_SENSE() + m_iAddValue[ AT_SENSE	] + m_PassiveAbilityFromRate[ AT_SENSE-AT_STR ] );	}	// ����

	int	  GetCur_SaveMP ()	// MP ���� ����
	{
		//return  ( GetPassiveSkillValue( AT_PSV_SAVE_MP ) + m_iAddValue[ AT_SAVE_MP ] + (short)(m_iAddValue[ AT_SAVE_MP ]*GetPassiveSkillRate( AT_PSV_SAVE_MP )/100.f) );	
		return  GetPassiveSkillRate( AT_PSV_SAVE_MP )+GetPassiveSkillRate(AT_SAVE_MP);
	}
	//int GetCur_DropRATE()			{	return  ( GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] );	}	// ��� Ȯ��
	int	  GetCur_DropRATE()			{	return  m_iDropRATE;	}	// ��� Ȯ��
	void  Cal_DropRATE ()	// ��� Ȯ��		
	{
		m_iDropRATE = GetPassiveSkillValue( AT_PSV_DROP_RATE ) + m_iAddValue[ AT_DROP_RATE ] + (short)(m_iAddValue[ AT_DROP_RATE ]*GetPassiveSkillRate( AT_PSV_DROP_RATE )/100.f);	
	}

	int	  GetMax_SummonCNT()		{	return	( 50+GetPassiveSkillValue( AT_PSV_SUMMON_MOB_CNT ) );						}	// �ִ� ��ȯ ���� �� ����

	int	  GetCur_JoHapPOINT( char btUnionIDX )					{	return btUnionIDX>=1 ?this->m_GrowAbility.m_nJoHapPOINT[ btUnionIDX-1 ] : 0;		}
	virtual void  SetCur_JoHapPOINT( BYTE btUnionIDX, short nValue ) = 0;//	{	if ( btUnionIDX >=1 ) this->m_GrowAbility.m_nJoHapPOINT[ btUnionIDX-1 ] = nValue;	}
	void  AddCur_JoHapPOINT( BYTE btUnionIDX, short nValue )	
	{	
		if ( btUnionIDX >=1 ) {
			short nResult = this->m_GrowAbility.m_nJoHapPOINT[ btUnionIDX-1 ] + nValue;
			this->SetCur_JoHapPOINT( btUnionIDX, nResult );
		}
	}
	void  SubCur_JoHapPOINT( BYTE btUnionIDX, short nValue )	
	{	
		if ( btUnionIDX >=1 ) {
			short nResult = this->m_GrowAbility.m_nJoHapPOINT[ btUnionIDX-1 ] - nValue;
			this->SetCur_JoHapPOINT( btUnionIDX, nResult );
		}
	}

	int	  GetCur_FameB ()			{	return	this->m_GrowAbility.m_nFameB;		}
	int	  GetCur_FameG ()			{	return	this->m_GrowAbility.m_nFameG;		}

	//int	  GetCur_GuildNO()			{	return	this->m_GrowAbility.m_iGuildNO;		}
	//int	  GetCur_GuildPOS()			{	return	this->m_GrowAbility.m_btGuildPOS;	}
	//int	  GetCur_GuildCONTRIBUTION(){	return	this->m_GrowAbility.m_nGuildCNTRB;	}
	//virtual void AddClanCONTRIBUTE(int /*iCon*/)=0;
	virtual void AddClanSCORE(int/*iValue*/)=0;

	short GetDef_MaxHP ()			{	return	this->m_Battle.m_nMaxHP;			}
	short GetDef_MaxMP ()			{	return	this->m_Battle.m_nMaxMP;			}

	short GetCur_MaxWEIGHT ()		{	return	this->m_Battle.m_nMaxWEIGHT;		}
	int	  GetCur_MAG ()				{	return	this->m_Battle.m_nMAG;				}	// ������

//	int	  GetTot_DEF_QUALITY()		{	return	this->m_Battle.m_iDefQuality;		}
	int	  GetTot_DEF_DURABITY()		{	return	this->m_Battle.m_iDefDurabity;		}
	int	  GetTot_DEF_GRADE()		{	return	this->m_Battle.m_iDefGrade;			}

	float GetCur_RateUseMP()		{	return	this->m_Battle.m_fRateUseMP;		}

	void	SetCur_MONEY (__int64 iMoney)	{	this->m_Inventory.m_i64Money=iMoney;	}
	__int64 GetCur_MONEY ()					{	return	this->m_Inventory.m_i64Money;	}	// ��

	void	Add_CurMONEY (int iMoney)		
	{	if(iMoney<0) 
		return; 
		this->m_Inventory.m_i64Money+=iMoney;	
	}

	void	Sub_CurMONEY (int iMoney)		
	{	
		if(this->m_Inventory.m_i64Money< iMoney)
		{
			this->m_Inventory.m_i64Money =0; 
			return;
		}
		this->m_Inventory.m_i64Money-=iMoney;
	}

	void	Add_CurMONEY (__int64  iMoney)		
	{	if(iMoney<0) 
	return; 
	this->m_Inventory.m_i64Money+=iMoney;	
	}

	void	Sub_CurMONEY (__int64  iMoney)		
	{	
		if(this->m_Inventory.m_i64Money< iMoney)
		{
			this->m_Inventory.m_i64Money =0; 
			return;
		}
		this->m_Inventory.m_i64Money-=iMoney;
	}

	void  SetCur_HP (short nValue);// �����
	void  SetCur_MP (short nValue)	{	this->m_GrowAbility.m_nMP=nValue;				}

	int   AddCur_HP (short nValue)	
	{	
		m_GrowAbility.m_nHP+=nValue; 
		if ( m_GrowAbility.m_nHP > GetCur_MaxHP() )
			m_GrowAbility.m_nHP = GetCur_MaxHP();
		return m_GrowAbility.m_nHP;
	}
	int	  AddCur_MP (short nValue)	
	{	
		m_GrowAbility.m_nMP+=nValue;
		if ( m_GrowAbility.m_nMP > GetCur_MaxMP() )
			m_GrowAbility.m_nMP = GetCur_MaxMP();
		return m_GrowAbility.m_nMP;
	}
	int   SubCur_HP (short nValue)
	{	
		m_GrowAbility.m_nHP-=nValue;
		if ( m_GrowAbility.m_nHP < 0 )
			m_GrowAbility.m_nHP = 0;
		return m_GrowAbility.m_nHP;
	}
	int	  SubCur_MP (short nValue)
	{	
		m_GrowAbility.m_nMP-=nValue;
		if ( m_GrowAbility.m_nMP < 0 )
			m_GrowAbility.m_nMP = 0;
		return m_GrowAbility.m_nMP;
	}

	void  SetCur_FAME (char cValue)			{	this->m_BasicINFO.m_cFame=cValue;													}
	virtual void SetCur_UNION (char cValue) = 0;//		{	if ( cValue >= 0 && cValue < MAX_UNION_COUNT )	this->m_BasicINFO.m_cUnion=cValue;	}
	void  SetCur_RANK (char cValue)			{	this->m_BasicINFO.m_cRank=cValue;			}

	char  GetCur_FACE ()					{	return this->m_BasicINFO.m_cFaceIDX;		}
	void  SetCur_FACE (char cValue)			{	this->m_BasicINFO.m_cFaceIDX=cValue;		}

	char  GetCur_HAIR ()					{	return this->m_BasicINFO.m_cHairIDX;		}
	void  SetCur_HAIR (char cValue)			{	this->m_BasicINFO.m_cHairIDX=cValue;		}

	virtual void  SetCur_JOB (short nValue)	{	this->m_BasicINFO.m_nClass=nValue;			}

	void  SetCur_LEVEL(WORD wValue)			{	this->m_GrowAbility.m_nLevel = ( wValue>MAX_LEVEL) ? MAX_LEVEL : wValue;	}
	void  SetCur_EXP (long lValue)			{	this->m_GrowAbility.m_lEXP=lValue;			}
	void  SetCur_BonusPOINT(short nValue)	{	this->m_GrowAbility.m_nBonusPoint=nValue;	}
	void  SetCur_SkillPOINT(short nValue)	{	this->m_GrowAbility.m_nSkillPoint=nValue;	}


//	tagMaintainDATA		m_MaintainDATA;

	tagITEM	Get_EquipITEM (WORD wEquipIDX)							{	return m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}
	tagITEM*Get_EquipItemPTR(WORD wEquipIDX)						{	return &m_Inventory.m_ItemEQUIP[ wEquipIDX ];			}

	tagITEM Get_InventoryITEM (WORD wTotalListIndex)				{	return m_Inventory.m_ItemLIST[ wTotalListIndex ];		}
	tagITEM Get_InventoryITEM (t_InvTYPE InvTYPE, WORD wPageIndex)	{	return m_Inventory.m_ItemPAGE[ InvTYPE ][ wPageIndex ];	}

	MileageItem* Get_M_EquipITEMPTR(WORD wEquipIDX)             { return &m_MileageInv.m_ItemEQUIP[wEquipIDX];}

#ifdef __SERVER
	short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	short	Add_ITEM (tagITEM &sITEM)								
	{
		return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					
	}

	short Add_ITEM(MileageItem &sITEM)
	{
		return m_MileageInv.AppendITEM(sITEM, m_Battle.m_nWEIGHT );		
	}

	short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}
#else
	virtual short	Add_CatchITEM (short nListRealNO, tagITEM &sITEM);//		{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT );	}
	virtual short	Add_ITEM (tagITEM &sITEM)								{	return m_Inventory.AppendITEM( sITEM, m_Battle.m_nWEIGHT );					}
	virtual short	Add_ITEM(short nListRealNO, tagITEM &sITEM);//				{	return m_Inventory.AppendITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);		}
	virtual void	Sub_ITEM(short nListRealNO, tagITEM &sITEM);//				{	m_Inventory.SubtractITEM( nListRealNO, sITEM, m_Battle.m_nWEIGHT);			}
	virtual void    Set_ITEM(short nListRealNO, tagITEM& sITEM);
	virtual void	ClearITEM(WORD wListRealNO)									
	{	
///		m_Battle.m_nWEIGHT -= m_Inventory.GetWEIGHT (wListRealNO);
		m_Inventory.DeleteITEM(wListRealNO); 
	}

#endif
	/// ���� ��ų�� ���� ������ ���� ������ �Լ�...
	virtual void  SetCur_SEX( char cValue ) = 0;
	virtual short GetCur_RACE() = 0;            /// ����,,,, gsv_SELECT_CHAR.m_btCharRACE �� �����ϸ��
	virtual short GetCur_ATK_SPD () = 0;
	virtual float GetCur_MOVE_SPEED () = 0;
	virtual int	  GetCur_ATK() = 0;
	virtual int	  GetCur_DEF() = 0;
	virtual int	  GetCur_RES() = 0;
	virtual int	  GetCur_HIT() = 0;
	virtual int	  GetCur_AVOID() = 0;
	virtual int	  GetCur_CRITICAL() = 0;

	virtual int	  GetCur_MaxHP () = 0;			/// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
	virtual int	  GetCur_MaxMP () = 0;	        /// ��ų�� ���� ������ ���� ������� �������� ������ �Լ�
	virtual void  UpdateCur_Ability() = 0;
	virtual BYTE  GetCur_MOVE_MODE ()			{	return 0;	}

	virtual int	  GetCur_FUEL () = 0;
	virtual void  SubCur_FUEL (short nValue) = 0;

	virtual void  SetCur_TeamNO(int /*nValue*/) = 0;			
#ifdef __CLAN_WAR_SET
	//	Ŭ���� �߰� ����.
	virtual void  SetCur_TeamNO_Back(int) = 0;
#endif

	void	SetCur_PK_FLAG (short nValue)		{	this->m_GrowAbility.m_nPKFlag = nValue;		}
	short	GetCur_PK_FLAG ()					{	return this->m_GrowAbility.m_nPKFlag;		}

	short	GetCur_STAMINA ()					{	return this->m_GrowAbility.m_nSTAMINA;		}
	void	SetCur_STAMINA (short nValue)		
	{	
		this->m_GrowAbility.m_nSTAMINA = nValue;
		if ( this->m_GrowAbility.m_nSTAMINA > MAX_STAMINA )
			this->m_GrowAbility.m_nSTAMINA = MAX_STAMINA;
		else
		if ( this->m_GrowAbility.m_nSTAMINA < 0 )
			this->m_GrowAbility.m_nSTAMINA = 0;
	}
	void	AddCur_STAMINA (short nValue)		
	{	
		this->m_GrowAbility.m_nSTAMINA += nValue;
		if ( this->m_GrowAbility.m_nSTAMINA > MAX_STAMINA )
			this->m_GrowAbility.m_nSTAMINA = MAX_STAMINA;
		else
		if ( this->m_GrowAbility.m_nSTAMINA < 0 )
			this->m_GrowAbility.m_nSTAMINA = 0;
	}

	virtual void  AddCur_EXP ( int /*iValue*/ )	{	/* nop */	}	// ȣ��ɰ�� �������� ���۵�.
	void	AddCur_BonusPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint+=nValue;	}
	void	AddCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint+=nValue;	}

	void	SubCur_BounsPOINT (short nValue)	{	this->m_GrowAbility.m_nBonusPoint-=nValue;	}
	void	SubCur_SkillPOINT (short nValue)	{	this->m_GrowAbility.m_nSkillPoint-=nValue;	}
		
	bool	Set_AbilityValue (WORD nType, int iValue);
	void	Add_AbilityValue (WORD nType, int iValue);

	virtual int	GetCur_SummonCNT() = 0;                         /// ���� ���� ��ȯ�Ǿ� �ִ� NPC����(������, �� ������ 0���� ������)
#ifdef	__SERVER
	virtual int GetCur_AbilityValue (WORD nType) = 0;
//			int GetOri_AbilityValue (WORD nType);
#else
	int		GetCur_AbilityValue (WORD nType)						{	return Get_AbilityValue(nType);		}
	int		Get_AbilityValue (WORD nType);
	void	RefreshLookUpTable()									{   m_Inventory.MakeItemIndexList();	}
	virtual void  UpdateModelData() = 0{ *(int*)0 = 10; }		/// �� ������ ����..
#endif

	bool	Use_ITEM (WORD wUseItemNO);
	bool	Set_EquipITEM (short nEquipIDX, tagITEM &sITEM);
	bool	Check_PatEquipCondition(tagITEM &sITEM, short nEquipIdx );
	bool	Check_EquipCondition (tagITEM &sITEM, short nEquipIdx);
	bool    Check_EquipCondition (MileageItem &sITEM, short nEquipIdx);  //���ϸ��� ����������..
	bool	Check_JobCollection( short nClassStbIDX );
	
	virtual void  SetCur_PartITEM (short nPartIdx, tagITEM &sItem) = 0;
	virtual short GetCur_PartITEM(short nPartIdx) = 0;

public :
	int	Get_NeedEXP (int iLevel)
	{
		return ( m_GrowAbility.m_lPenalEXP + CCal::Get_NeedRawEXP(iLevel) );
	}


	short Get_NeedPoint2AbilityUP (short BasicAbilityType)
	{
		_ASSERT( BasicAbilityType >= 0 && BasicAbilityType < 6 );

		return (short)( m_BasicAbility.m_nBasicA[ BasicAbilityType ] * 0.2 );
	}

	/// skill stb�� �÷����� ����ϴ°�����...
	short Get_NeedPoint2SkillUP (short nSkillSLOT);

	//  -------------------------------------------------------------------------------------------
	/// ��ų ���� ���� �Լ�...
	virtual int		GetCur_R_WEAPON () = 0;
	virtual int		GetCur_L_WEAPON () = 0;
	virtual int		GetCur_PET_BODY () = 0;
	virtual DWORD	GetCur_IngStatusFLAG() = 0;

	/// ��ų�� �� ���� ã��
	short Skill_FindEmptySlot( short nSkillIDX );

	/// ������ ��ų�� ������ ��´�.
	short Skill_FindLearnedLevel( short nSkillIDX );

	/// ������ ��ų�� ������ ã�´�.
	short Skill_FindLearnedSlot ( short nSkillIDX );

	short Skill_FindLearnableSlot ( short nSkillIDX );

	/// �ɷ�ġ ��� �Լ��� ������ ��ų���� ����ϴ� �ɷ�ġ�� �Ҽ��� �Ұ� ������
	/// �ӵ��� ���̱� ���ؼ� ��ų�� ����ϴ� �ɷ�ġ�� ������ �Լ�...
	int   Skill_GetAbilityValue( short nAbilityType );

	/// ��ų���� �Ҹ�Ǵ� ��ġ ����...
	int   Skill_ToUseAbilityVALUE(short nSKillIDX, short nPropertyIDX );
	bool  Skill_UseAbilityValue( short nSkillIDX );


	bool Skill_CheckJOB( short nSkillIDX );				/// ����, ������� ���� üũ 
	bool Skill_CheckLearnedSKILL( short nSkillIDX );	/// ����, ������� �̹� ������ ��ų üũ 
	bool Skill_CheckNeedABILITY( short nSkillIDX );		/// ����, ������� ����� �ɷ�ġ üũ..

	/// ��ų ó�� ���� ���� üũ �Լ�
	BYTE  Skill_LearnCondition( short nSkillIDX );
	BYTE Skill_LearnCondition(short nSkillIDX,short& nSlot);

	/// ��ų ������ ���� üũ �Լ�
	BYTE  Skill_LevelUpCondition( short nCurLevelSkillIDX, short nNextLevelSkillIDX );

	/// ��ų ��� ���� üũ �Լ�
	bool  Skill_ActionCondition( short nSkillIDX );

	/// ��ų ���� :: �����̳� �������� ȣ���ؾ� �нú� ��ų �����..
	BYTE  Skill_LEARN( short nSkillSLOT, short nSkillIDX, bool bSubPOINT=true );

	/// ������ ��ų�� �����Ѵ�.
	bool  Skill_DELETE( short nSkillSLOT, short nSkillIDX );

	/// �нú� ��ų ���� �Լ�.
	void  InitPassiveSkill ();				/// ������ ���� ��ų ����Ÿ ������ �ѹ� ȣ���ؾ���...
//	short GetPassiveSkillAttackSpeed()		{	return m_nPassiveAttackSpeed + m_iAddValue[ AT_ATK_SPD ];	}

	short GetBuySkillVALUE()				
	{
#ifdef __KRRE_NEW_AVATAR_DB
		return GetPassiveSkillRate( AT_PSV_BUYING  );	
#endif
		return GetPassiveSkillValue( AT_PSV_BUYING  );	
	}

	short GetSellSkillVALUE()				
	{
#ifdef __KRRE_NEW_AVATAR_DB
		return GetPassiveSkillRate( AT_PSV_SELLING  );	
#endif
		return GetPassiveSkillValue( AT_PSV_SELLING );	
	}

	//  -------------------------------------------------------------------------------------------
	/// ����Ʈ ���� ���� �Լ�...
#ifdef	__SERVER
	virtual bool Quest_IsPartyLEADER() = 0;						    /// ��Ƽ ¯�̳� ?
	virtual int	 Quest_GetPartyLEVEL() = 0;						    /// ���� ��Ƽ ����
	virtual int	 Quest_GetZoneNO() = 0;							    /// ���� �� ��ȣ
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0;	/// ������ġ���� �Ÿ�
	virtual int	 Quest_PartyMemberCNT() = 0;                        /// ��Ƽ �ο���
#else
	virtual bool Quest_IsPartyLEADER() = 0							{	*(int*)0 = 10; return false;		}	/// ��Ƽ ¯�̳� ?
	virtual int	 Quest_GetPartyLEVEL() = 0							{	*(int*)0 = 10; return -1;			}	/// ���� ��Ƽ ����
	virtual int	 Quest_GetZoneNO() = 0								{	*(int*)0 = 10; return 0;			}	/// ���� �� ��ȣ
	virtual int	 Quest_DistanceFrom(int iX, int iY, int iZ) = 0		{	*(int*)0 = 10; return 999999999;	}	/// ������ġ���� �Ÿ�
	virtual int	 Quest_PartyMemberCNT() = 0							{	*(int*)0 = 10; return 0;			}	/// ��Ƽ �ο���
#endif

	////	����Ʈ ���� ���� ���
	DWORD	Quest_Get_Number();
	BYTE    Quest_GetRegistered( int iQuestIDX );
	short	Quest_Append( int iQuestIDX );
#ifdef	__CLAN_WAR_SET
	short	Quest_Append_9( int iQuestIDX );	// 9�� ����Ʈ �߰�.
#endif
	bool	Quest_Append( BYTE btSlot, int iQuestIDX );
	bool	Quest_Delete( int iQuestIDX );
	bool	Quest_Delete( BYTE btSLOT, int iQuestID );
	tagITEM*Quest_FindEquipITEM(WORD wEquipIDX)		{	return ( wEquipIDX < MAX_EQUIP_IDX ) ? &m_Inventory.m_ItemEQUIP[ wEquipIDX ] : NULL;		}
	tagITEM*Quest_FindITEM( tagITEM &sFindITEM )	
	{
		short nFindSLOT = m_Inventory.FindITEM( sFindITEM );
		return ( nFindSLOT >= 0 ) ? &m_Inventory.m_ItemLIST[ nFindSLOT ] : NULL;
	}
	bool	Quest_SubITEM( tagITEM &sSubITEM );

	short Quest_GetEpisodeVAR( WORD wVarIDX )		{	return ( wVarIDX < QUEST_EPISODE_VAR_CNT	) ? m_Quests.m_nEpisodeVAR	[ wVarIDX ] : 0;	}
	short Quest_GetJobVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_JOB_VAR_CNT		) ? m_Quests.m_nJobVAR		[ wVarIDX ] : 0;	}
	short Quest_GetUnionVAR( WORD wVarIDX )			{	return ( wVarIDX < QUEST_UNION_VAR_CNT		) ? m_Quests.m_nUnionVAR	[ wVarIDX ] : 0;	}
	short Quest_GetPlanetVAR( WORD wVarIDX )		
	{	
		if ( wVarIDX < QUEST_PLANET_VAR_CNT	) 
			return m_Quests.m_nPlanetVAR[ wVarIDX ];
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wVarIDX < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			return m_Quests.m_nEtcVAR[ wVarIDX-QUEST_PLANET_VAR_CNT ];
		}
#endif
		return 0;
	}

//	Ŭ���� ����Ʈ ���� ���
#ifdef __CLAN_WAR_SET	
	//	������ ���� ���� ��´�.
	short Quest_GetClanWarVAR( WORD wVarIDX ) {return ( wVarIDX < QUEST_CLANWAR_VAR_CNT) ? m_Quests.m_nClanWarVAR[wVarIDX] : 0;}
#endif

	void Quest_SetEpisodeVAR( WORD wI, short nV )	{	if ( wI < QUEST_EPISODE_VAR_CNT )	m_Quests.m_nEpisodeVAR[ wI ] = nV;	}
	void Quest_SetJobVAR	( WORD wI, short nV )	{	if ( wI < QUEST_JOB_VAR_CNT )		m_Quests.m_nJobVAR[ wI ] = nV;		}
	void Quest_SetUnionVAR	( WORD wI, short nV )	{	if ( wI < QUEST_UNION_VAR_CNT )	m_Quests.m_nUnionVAR[ wI ] = nV;	}
	void Quest_SetPlanetVAR	( WORD wI, short nV )	
	{	
		if ( wI < QUEST_PLANET_VAR_CNT ) 
			m_Quests.m_nPlanetVAR[ wI ] = nV;
#ifdef	__APPLY_EXTAND_QUEST_VAR
		else
		if ( wI < QUEST_PLANET_VAR_CNT+QUEST_ETC_VAR_CNT ) {
			m_Quests.m_nEtcVAR[ wI-QUEST_PLANET_VAR_CNT ] = nV;
		}
#endif
	}
 
//	Ŭ���� ����Ʈ ���� ����
#ifdef __CLAN_WAR_SET	
	//	������ ���� ���� �����Ѵ�.
	void Quest_SetClanWarVAR( WORD wI, short nV ) {	if ( wI < QUEST_CLANWAR_VAR_CNT )	m_Quests.m_nClanWarVAR[ wI ] = nV;	}
#endif

	/// ����Ʈ ���� ���� �Լ�
#ifdef	__SERVER
	virtual bool Reward_WARP( int iZoneNO, tPOINTF &PosGOTO) = 0;
#endif

	virtual bool Quest_CHANGE_SPEED()							{	return true;	}

	virtual bool Add_MoneyNSend( int iAmount, WORD wType )		{	return true;	}
	virtual bool Add_ExpNSend( int iExp )						{	return true;	}
	virtual bool Add_ItemNSend( tagITEM &sITEM )				{	return true;	}
	virtual bool Add_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Sub_SkillNSend( short nSkillIDX )				{	return true;	}
	virtual bool Set_AbilityValueNSend( WORD nType, int iValue ){	return true;	}
	virtual void Add_AbilityValueNSend( WORD wType, int iValue ){	/* nop */		}

	bool Reward_InitSKILL (void);
	bool Reward_InitSTATUS (void);
	//	��ų �ʱ�ȭ �Լ� : ��ȫ�� �븮.
	bool Reward_InitSKILLType ( int iInitType = -1, int iSkillSlot = -1 );
	int	 Set_InitSKILLType ( int iBeginSkillIndex, int iSize );

	bool Reward_ITEM	( tagITEM &sITEM, BYTE btRewardToParty, BYTE btQuestSLOT );
	bool Reward_ABILITY	( bool bSetOrAdj, int iType, int iValue, BYTE btRewardToParty );
	bool Reward_CalEXP	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty );
	bool Reward_CalMONEY( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, short nDupCNT );
	bool Reward_CalITEM	( BYTE btEquation, int iBaseValue, BYTE btRewardToParty, int iItemSN, short nItemOP, BYTE btQuestSLOT );

	virtual void Cal_AruaATTACK ()	= 0;
	virtual void Cal_AruaHIT ()		= 0;
	virtual void Cal_AruaAVOID ()	= 0;
	virtual void Cal_AruaCRITICAL ()= 0;
	virtual void Cal_AruaMaxHP ()	= 0;
	virtual void Cal_AruaMaxMP ()	= 0;
	virtual void Cal_AruaRunSPD()	= 0;

	virtual short Get_WeightRATE()	= 0;

#ifdef __KCHS_BATTLECART__

#ifndef	__INC_WORLD
//-------------------------------------
// 2006.05.16/��뼺/���� - īƮ, ĳ����� ��Ÿ�� 30�п��� 30�ʷ� ����
//	#define PAT_COOLTIME	( 30 * 60 * 1000 )
	#define PAT_COOLTIME	( 30 * 1000 )
//-------------------------------------
#else
	#define PAT_COOLTIME	( 30 * 1000 )	// �׽�Ʈ��(���μ���)
#endif

	void Cal_PatMaxHP ();

	int GetCur_PatMaxHP()
	{
		return m_Battle.m_nPatMaxHP;
	}

	int GetCur_PatHP()
	{
		return this->m_GrowAbility.m_nPatHP;
	}

	int AddCur_PatHP(short nValue)
	{
		m_GrowAbility.m_nPatHP += nValue;
		if( m_GrowAbility.m_nPatHP > GetCur_PatMaxHP() )
			m_GrowAbility.m_nPatHP = GetCur_PatMaxHP();
		return m_GrowAbility.m_nPatHP;
	}

	int SubCur_PatHP(short nValue)
	{
		m_GrowAbility.m_nPatHP -= nValue;
		if( m_GrowAbility.m_nPatHP <= 0 ) {
			m_GrowAbility.m_nPatHP = 0;
			this->m_nPatHP_Mode = 1;
		}
		return m_GrowAbility.m_nPatHP;
	}

	int Set_MaxPatHP( short nValue )
	{
		m_Battle.m_nPatMaxHP = nValue;

		return m_Battle.m_nPatMaxHP;
	}

	void SetCur_PatHP(short nValue)
	{
		m_GrowAbility.m_nPatHP = nValue;
		if( m_GrowAbility.m_nPatHP > GetCur_PatMaxHP() )
			m_GrowAbility.m_nPatHP = GetCur_PatMaxHP();
	}

	DWORD GetCur_PatCoolTIME()
	{
		return m_GrowAbility.m_dwPatCoolTIME;
	}

	DWORD SetCur_PatCoolTIME( DWORD dwValue )
	{
		m_GrowAbility.m_dwPatCoolTIME = dwValue;
		if( m_GrowAbility.m_dwPatCoolTIME < 0 || m_GrowAbility.m_dwPatCoolTIME > PAT_COOLTIME )
			m_GrowAbility.m_dwPatCoolTIME = PAT_COOLTIME;
		return m_GrowAbility.m_dwPatCoolTIME;
	}

	DWORD SubCur_PatCoolTIME( DWORD dwValue )
	{
		if( m_GrowAbility.m_dwPatCoolTIME <= 0 )
		{
			m_GrowAbility.m_dwPatCoolTIME = 0;
			return 0;
		}

		// 2005.08.03 : icarus
		if ( dwValue >= m_GrowAbility.m_dwPatCoolTIME ) {
			m_GrowAbility.m_dwPatCoolTIME = 0;
			return 0;
		} else 
			m_GrowAbility.m_dwPatCoolTIME -= dwValue;
/*
		m_GrowAbility.m_dwPatCoolTIME -= dwValue;
		if( m_GrowAbility.m_dwPatCoolTIME < 0 || m_GrowAbility.m_dwPatCoolTIME > PAT_COOLTIME )
		{ // m_GrowAbility.m_dwPatCoolTIME > PAT_COOLTIME �̷��ٶ�� �� 0 - 1 �ߴٴ� ��.
			m_GrowAbility.m_dwPatCoolTIME = 0;
			return 0;
		}
*/
		return m_GrowAbility.m_dwPatCoolTIME;
	}
#endif

	/// 2������ ������ ��µǴ� �ɷ�ġ ����
	void Apply_2ndJob_Ability ( void );
} ;


//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif
