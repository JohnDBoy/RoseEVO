/**
 * \ingroup SHO_GS
 * \file	CObjAVT.h
 * \brief	CObjCHAR 클래스를 상속 받는 avatar 케릭터 처리
 */
#ifndef	__COBJAVT_H
#define	__COBJAVT_H
#include "CObjCHAR.h"
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \class	CObjAVT
 * \author	wookSang.Jo
 * \brief	사용자 케릭터 데이터 관리및 처리하는 클래스
 *			기본 케릭터 클래스 CObjCHAR, 사용자 기본데이타 클래스 CUserDATA를 상속받음
 */

static short s_nEquipIdxToPartNO [] = {
	-1,						// 0
		BODY_PART_FACE_ITEM,	// EQUIP_IDX_FACE_ITEM = 1,
		BODY_PART_HELMET,		// EQUIP_IDX_HELMET,
		BODY_PART_ARMOR,		// EQUIP_IDX_ARMOR,
		BODY_PART_KNAPSACK,		// EQUIP_IDX_KNAPSACK,
		BODY_PART_GAUNTLET,		// EQUIP_IDX_GAUNTLET,
		BODY_PART_BOOTS,		// EQUIP_IDX_BOOTS,	// 5
		BODY_PART_WEAPON_R,		// EQUIP_IDX_WEAPON_R,
		BODY_PART_WEAPON_L,		// EQUIP_IDX_WEAPON_L,	
		-1,						// EQUIP_IDX_NECKLACE,
		-1,						// EQUIP_IDX_RING,
} ;

class CObjAVT : public CObjCHAR, public CUserDATA
{
public :
	// virtual function inherit from CGameOBJ -----------------------------------------------------
	t_ObjTAG	Get_TYPE ()			{	return OBJ_AVATAR;			}
    char	   *Get_NAME ()			{	return m_Name.Get();		}
	bool		Make_gsv_ADD_OBJECT( classPACKET *pCPacket );

public :	
	// virtual function inherit from CUserDATA ----------------------------------------------------

	short	GetCur_ATK_SPD ()									{	return Get_nAttackSPEED();				}
	float	GetCur_MOVE_SPEED ()								{	return Get_MoveSPEED();					}
	BYTE	GetCur_MOVE_MODE ()									{	return Get_MoveMODE();					}
	BYTE	GetCur_RIDE_MODE ()									{	return m_btRideMODE;					}

	short	GetCur_PartITEM(short nPartIdx)						{	return GetPartITEM(nPartIdx);			}
//	void	SetCur_PartITEM	(short nPartIdx, short nItemNo)		{	this->SetPartITEM(nPartIdx, nItemNo);	}
	void	SetCur_PartITEM	(short nPartIdx, tagITEM &sITEM)	{	this->SetPartITEM(nPartIdx, sITEM);		}

	int		GetCur_R_WEAPON ()									{	return this->Get_R_WEAPON();			}
	int		GetCur_L_WEAPON ()									{	return this->Get_L_WEAPON();			}
	int		GetCur_PET_BODY ()									{	return this->Get_PET_BODY ();			}

	DWORD	GetCur_IngStatusFLAG()								{	return this->GetIngDurationStateFLAG();	}

	int		GetCur_AbilityValue (WORD nType)					{	return Get_AbilityValue(nType);			}
	int		GetCur_MaxHP ()										{	return this->Get_MaxHP();				}
	int		GetCur_MaxMP ()										{	return this->Get_MaxMP();				}

	int		GetCur_ATK()										{	return Get_ATK();			}
	int		GetCur_DEF()										{	return Get_DEF();			}
	int		GetCur_RES()										{	return Get_RES();			}
	int		GetCur_HIT()										{	return Get_HIT();			}
	int		GetCur_AVOID()										{	return Get_AVOID();			}
	int		GetCur_CRITICAL()									{	return Get_CRITICAL();		}


//-------------------------------------
// 2006.11.20/김대성/추가 - 계산공식 수정
#ifdef __BUFF_CALC_
	float	GetCur_MOVE_SPEED2 ()								{	return Get_MoveSPEED2();			}	// 이동력
	short	GetCur_ATK_SPD2 ()									{	return Get_nAttackSPEED2();			}	// 공격속도

	int		GetCur_ATK2()										{	return Get_ATK2();			}	// 공격력
	int		GetCur_DEF2()										{	return Get_DEF2();			}	// 방어력
	int		GetCur_RES2()										{	return Get_RES2();			}	// 항마력
	int		GetCur_HIT2()										{	return Get_HIT2();			}	// 명중력
	int		GetCur_AVOID2()										{	return Get_AVOID2();			}	// 회피력
	int		GetCur_CRITICAL2()									{	return Get_CRITICAL2();		}	// 크리티컬
#endif
//-------------------------------------

	int	    GetCur_FUEL ();
	void    SubCur_FUEL (short nValue);

	short	GetCur_RACE ()										{	return m_nCharRACE;			}
	void    SetCur_SEX( char cValue );

	void	SetCur_TeamNO (int iTeamNO)
			{	
				//	길드 전을 위한 팀 설정 변경.
#ifdef __CLAN_WAR_SET
				if(m_iTeamNO_Back == 0) 
					m_iTeamNO = iTeamNO;
				else
					m_iTeamNO = m_iTeamNO_Back;
#else
				m_iTeamNO = iTeamNO;
#endif
			}
			
#ifdef __CLAN_WAR_SET
	void	SetCur_TeamNO_Back(int iTeamNO)							{ m_iTeamNO_Back = iTeamNO;}
#endif

	void	SetCur_UNION (char cValue);							//	{	if ( cValue >= 0 && cValue < MAX_UNION_COUNT )	this->m_BasicINFO.m_cUnion=cValue;	}
	void	SetCur_JoHapPOINT( BYTE btUnionIDX, short nValue );	//	{	if ( btUnionIDX >=1 ) this->m_GrowAbility.m_nJoHapPOINT[ btUnionIDX-1 ] = nValue;	}

	void	AddCur_EXP (int iExp)								{	this->Add_EXP( iExp );		}
	void	UpdateCur_Ability()									{	this->UpdateAbility();		}

public :	
	// virtual function inherit from CObjAI -------------------------------------------------------
	CAI_OBJ*Get_CALLER()										{	return this;				}
	bool	Is_ALLIED( CAI_OBJ *pDestOBJ );

	bool	SkillChk_ACTIVE ( short nSkillIDX )					
	{	
		if ( SKILL_TYPE_03 == SKILL_TYPE( nSkillIDX ) ) {
			if ( !this->Chk_ShotITEM( 1 ) )
				return false;
		}
		return Skill_ActionCondition( nSkillIDX );	
	}
	void	SkillUse_ABILITY( short nSkillIDX )
	{	
		if ( SKILL_TYPE_03 == SKILL_TYPE( nSkillIDX ) ) {
			this->Dec_ShotITEM( 1 );
		}
		Skill_UseAbilityValue( nSkillIDX );

	}
	void	 Recv_ITEM( short nItemNO, short iDupCnt)
	{
		tagITEM sItem;

		sItem.Init( nItemNO, iDupCnt );
		if ( sItem.GetTYPE() )
			this->Add_ItemNSend( sItem );
	}

	int		Get_CON()				{	return this->GetCur_CON ();		}
	int		GetAdd_RecoverHP()		{	return this->m_btRecoverHP;		}
	int		GetAdd_RecoverMP()		{	return this->m_btRecoverMP;		}

	void    Check_PerFRAME(DWORD dwPassTIME);

	short	Get_WeightRATE()		{	return this->m_btWeightRate;	}

	int		GetANI_Stop()			{	return ( m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_STOP1	)	: AVT_ANI_STOP1;	}
	int		GetANI_Move()			
	{	
		if ( m_btRideMODE ) {
			return ( m_nPatAniROW + PAT_ANI_MOVE );
		}
		
		return ( m_bRunMODE ) ? AVT_ANI_RUN : AVT_ANI_WALK;
	}
	int		GetANI_Attack()			{	return (  m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_ATTACK	)	: AVT_ANI_ATTACK;	}
	int		GetANI_Die ()			{	return (  m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_DIE		)	: AVT_ANI_DIE;		}
	int		GetANI_Hit ()			{	return (  m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_STOP1	)	: AVT_ANI_HIT;		}

	// 카트는 스킬을 직접 하지 못한다 :: 스킬 사용시 탑승 아바타가 모션을 하는것으로 가정 !!!
	int		GetANI_Casting ()		
	{	
		if ( IsTAIWAN() ) {
			return SKILL_ANI_CASTING( Get_ActiveSKILL() );
		}

		return (  m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_CASTING	)	: SKILL_ANI_CASTING( Get_ActiveSKILL() );		
	}
	int		GetANI_Skill ()	
	{	
		if ( IsTAIWAN() ) {
			return SKILL_ANI_ACTION_TYPE( Get_ActiveSKILL() );
		}

		return (  m_btRideMODE == RIDE_MODE_DRIVE ) ? ( m_nPatAniROW + PAT_ANI_SKILL	)	: SKILL_ANI_ACTION_TYPE( Get_ActiveSKILL() );	
	}

	int		GetANI_Sitting()		{	return AVT_ANI_SITTING;								}
	int		GetANI_Standing()		{	return AVT_ANI_STANDUP;								}
	int		GetANI_Sit()			{	return AVT_ANI_SIT;									}

	int		Get_R_WEAPON ()			{	return this->m_sRWeaponIDX.m_nItemNo;				}
	int		Get_L_WEAPON ()			{	return this->m_sLWeaponIDX.m_nItemNo;				}
	int		Get_PET_BODY ()			{	return this->m_sFrameIDX.m_nItemNo;					}

	int		Def_AttackRange()		{	return 70;						}

	bool	SetCMD_TOGGLE (BYTE btTYPE, bool bForce=false);

	short	Get_nAttackSPEED ()		{	return ( m_btRideMODE ) ?   GetOri_ATKSPEED() : CObjCHAR::Get_nAttackSPEED();	}
	float	Get_MoveSPEED ()		{	return ( m_btRideMODE ) ?	GetOri_RunSPEED() : CObjCHAR::Get_MoveSPEED();	}

//-------------------------------------
// 2006.11.20/김대성/추가 - 계산공식 수정
#ifdef __BUFF_CALC_
	short	Get_nAttackSPEED2 ()		{	return ( m_btRideMODE ) ?   GetOri_ATKSPEED() : CObjCHAR::Get_nAttackSPEED2();	}
	float	Get_MoveSPEED2 ()		{	return ( m_btRideMODE ) ?	GetOri_RunSPEED() : CObjCHAR::Get_MoveSPEED2();	}
#endif
//-------------------------------------

	// virtual function inherit from CObjCHAR -----------------------------------------------------
	int		Get_AttackRange (short nSkillIDX);
    bool	IsFemale ()				{	return (m_nCharRACE&0x01);	}
    short	IsMagicDAMAGE()			{	return WEAPON_IS_MAGIC_DAMAGE( Get_R_WEAPON() );	}

	tagMOTION  *Get_MOTION (short nActionIdx=AVT_ANI_STOP1);

	short	Get_WEIGHT ()			{	return	GetCur_WEIGHT ();		}
//    short	Get_ItemQUALITY (short nEquipIDX);
    int		Get_LEVEL ()			{	return	GetCur_LEVEL ();		}

	int		Get_EXP ()				{	return  GetCur_EXP();			}
	int		Get_JOB ()				{	return	GetCur_JOB();			}
	__int64	Get_MONEY()				{	return	GetCur_MONEY();			}

	int		Get_INT ()				{	return	GetCur_INT ();			}
    int		Get_CHARM ()			{	return	GetCur_CHARM ();		}	// 매력
    int		Get_SENSE ()			{	return	GetCur_SENSE ();		}	// 감각

	BYTE	Get_MoveMODE ()			{	return  ( this->m_btRideMODE ) ? m_btRideMODE : m_bRunMODE;	}
	BYTE	Get_RideMODE ()			{	return	this->m_btRideMODE;		}
#ifdef __KCHS_BATTLECART__
	bool	CanDoPatSkill( short nSkillIDX );
	bool	CanUsePAT()				{	return (GetCur_PatHP() > 0);	}
#endif

    int		Get_GiveEXP ()           {	return 1;						}
	int		Get_AbilityValue (WORD wType);

//-------------------------------------
// 2006.11.20/김대성/추가 - 계산공식 수정
#ifdef __BUFF_CALC_
	int		Get_AbilityValue2 (WORD wType);
#endif
//-------------------------------------

	int		Get_HP ()				{	return	GetCur_HP ();				}	// 생명력
	int		Get_MP ()				{	return	GetCur_MP ();				}

	void	Set_HP (int iHP)		{	SetCur_HP( iHP );					}
	void	Set_MP (int iMP)		{	SetCur_MP( iMP );					}

	int		Add_HP (int iValue)		{	return  AddCur_HP(iValue);			}
	int		Add_MP (int iValue)		{	return  AddCur_MP(iValue);			}

	int		Sub_HP (int iValue)		{	return  SubCur_HP(iValue);			}
	int		Sub_MP (int iValue)		{	return  SubCur_MP(iValue);			}

	void	Add_STAMINA(short nV)	{	AddCur_STAMINA( nV );				}
	void	Add_MONEY(short nV)		{	Add_CurMONEY( nV );					}

	void	Clear_SummonCNT()		{	this->m_nSummonCNT = 0;				}
	void	Add_SummonCNT(short nV)	{	this->m_nSummonCNT += nV;			}
	void	Sub_SummonCNT(short nV)	{	this->m_nSummonCNT -= nV;			}
	short	Get_SummonCNT()			{	return this->m_nSummonCNT;			}
	short	Max_SummonCNT()			{	return this->GetMax_SummonCNT();	}

	int		GetCur_SummonCNT()		{	return this->m_nSummonCNT;			}

	short	GetOri_WalkSPEED()		{	return WALK_CmPerSec;				}
	short	GetOri_RunSPEED()		{	return m_nRunSPEED;					}
	short	GetOri_ATKSPEED()		{	return this->m_nAtkAniSPEED;		}

	int		GetOri_ATK ()			{	return this->GetDef_ATK();			}
	int		GetOri_DEF ()			{	return this->GetDef_DEF();			}
	int		GetOri_RES ()			{	return this->GetDef_RES();			}
	int		GetOri_HIT ()			{	return this->GetDef_HIT();			}
	int		GetOri_AVOID ()			{	return this->GetDef_AVOID();		}
	int		GetOri_CRITICAL()		{	return this->GetDef_CRITICAL();		}

	int		GetOri_MaxHP()			{	return  GetDef_MaxHP();				}
	int		GetOri_MaxMP()			{	return  GetDef_MaxMP();				}

	int		Save_Damage( int iAttackerIDX, int iDamage);

	void	Dec_WeaponLife();

//-------------------------------------
/*
	void	Dec_EngineLife();
*/
// 2006.06.26/김대성/수정 - // 공격시 무기에 의한 연료감소
	void	Dec_EngineLife(BOOL bWeapon=FALSE);
//-------------------------------------

	void	Stamp_AttackTIME ();


	short	Get_PatHP_MODE()		{	return m_nPatHP_Mode;	}
	void	Set_PatHP_MODE(short nV){	m_nPatHP_Mode = nV;		}

//	int     Proc ()					{	return CObjCHAR::Proc ()			}

protected:
	// CObjAVT ------------------------------------------------------------------------------------
	short	m_nRunSPEED;
	short	m_nAtkAniSPEED;

	DWORD	m_dwGoddnessTIME;
	DWORD	m_dwPatTIME;

	DWORD	m_dwAttackTIME;
	DWORD	m_dwRecoverTIME;
	BYTE	m_btWeightRate;
	enum {
		RIDE_MODE_DRIVE = MOVE_MODE_DRIVE,
		RIDE_MODE_GUEST = MOVE_MODE_RIDEON,
	} ;

	short	m_nSummonCNT;
	BYTE	m_btSummonCMD;

	short	m_nPatAniROW;
	short	m_nPatAniCOL;

public :
	union {
		tagPartITEM	m_PartITEM[ MAX_BODY_PART ];
		struct {
			// 순서는 t_CharPART에 따라서...
			tagPartITEM m_sFaceIDX;			//short m_nFaceIDX;
			tagPartITEM m_sHairIDX;			//short m_nHairIDX;
			tagPartITEM m_sHelmetIDX;		//short m_nHelmetIDX;
			
			tagPartITEM m_sArmorIDX;		//short m_nArmorIDX;
			tagPartITEM m_sGauntletIDX;		//short m_nGauntletIDX;
			tagPartITEM m_sBootsIDX;		//short m_nBootsIDX;
			
			tagPartITEM m_sGoggleIDX;		//short m_nGoggleIDX;
			tagPartITEM m_sKnapsackIDX;		//short m_nKnapsackIDX;
			
			tagPartITEM m_sRWeaponIDX;		//short m_nRWeaponIDX;
			tagPartITEM m_sLWeaponIDX;		//short m_nLWeaponIDX;
		} ;
	} ;
	union {
		tagPartITEM m_RideITEM[ MAX_RIDING_PART ];
		struct {
			tagPartITEM m_sFrameIDX;		//short m_nFrameIDX;	// RIDE_PART_BODY = 0,
			tagPartITEM m_sEngineIDX;		//short m_nEngineIDX;	// RIDE_PART_ENGINE,
			tagPartITEM m_sLegIDX;			//short m_nLegIDX;	// RIDE_PART_LEG,
#ifndef __KCHS_BATTLECART__
			tagPartITEM m_sArmIDX;			//short m_nArmIDX;	// RIDE_PART_ARMS = 3,
#else
			tagPartITEM m_sAbilIDX;			//short m_nArmIDX;	// RIDE_PART_ARMS = 3, // kchs
			tagPartITEM	m_sArmIDX;			// ABIL이 기존 풀린 아이템 때문에 Arms앞에
#endif
		} ;
	} ;

	// CLAN interface ......
	tag_MY_CLAN		m_CLAN;
	CStrVAR			m_ClanNAME;
	void	ClanINIT(DWORD dwClanID=0, int iClanPOS=0)
	{
		m_CLAN.m_btClanLEVEL = 1;
		m_CLAN.m_dwClanID	 = dwClanID;
		m_CLAN.m_dwClanMARK  = 0;
		m_CLAN.m_iClanSCORE  = 0;
		m_CLAN.m_iClanRATE   = 0;
		m_CLAN.m_biClanMONEY = 0;
		m_CLAN.m_nMemberCNT  = 0;
		m_CLAN.m_iClanRATE	 = 0;

		m_CLAN.m_btClanPOS   = (BYTE)iClanPOS;
		m_CLAN.m_iClanCONT   = 0;

		m_ClanNAME.Set( NULL );

#ifdef	MAX_CLAN_SKILL_SLOT
		::ZeroMemory( m_CLAN.m_ClanBIN.m_pDATA, sizeof( tagClanBIN ) );
#endif
	}
	char*	GetClanNAME()				{	return m_ClanNAME.Get();					}
	void	SetClanNAME(char*szName)	{	m_ClanNAME.Set( szName );					}

	DWORD	GetClanID()					{	return m_CLAN.m_dwClanID;					}
	void	SetClanID(DWORD dwClanID)	{	m_CLAN.m_dwClanID = dwClanID;				}

	DWORD	GetClanMARK()				{	return m_CLAN.m_dwClanMARK;					}
	void	SetClanMARK(DWORD dwMark)	{	m_CLAN.m_dwClanMARK = dwMark;				}

	BYTE	GetClanPOS()				{	return m_CLAN.m_btClanPOS;					}
	void	SetClanPOS(int iPos)		{	m_CLAN.m_btClanPOS = (BYTE)iPos;			}

	BYTE	GetClanLEVEL()				{	return m_CLAN.m_btClanLEVEL;				}
	void	SetClanLEVEL(short nLev)	{	m_CLAN.m_btClanLEVEL=(BYTE)nLev;			}

	int		GetClanSCORE()				{	return m_CLAN.m_iClanSCORE;					}
	void	SetClanSCORE(int iScore)	{	m_CLAN.m_iClanSCORE = iScore;				}

	int		GetClanCONTRIBUTE()			{	return m_CLAN.m_iClanCONT;					}
	void	SetClanCONTRIBUTE(int iCon)	{	m_CLAN.m_iClanCONT = iCon;					}

	__int64 GetClanMONEY ()				{	return m_CLAN.m_biClanMONEY;				}
	void	SetClanMONEY (__int64 biM)	{	m_CLAN.m_biClanMONEY = biM;					}

	short	GetClanUserCNT ()			{	return m_CLAN.m_nMemberCNT;					}
	void	SetClanUserCNT (short nC)	{	m_CLAN.m_nMemberCNT = nC;					}

	int		GetClanRATE()				{	return m_CLAN.m_iClanRATE;					}
	void	SetClanRATE(int iR)			{	m_CLAN.m_iClanRATE = iR;					}

	bool	Is_ClanMASTER(void)			{	return this->GetClanPOS() >= GPOS_MASTER;	}

	int				m_iAppliedPenaltyEXP;
	BYTE			m_btRideMODE;
	int				m_iLinkedCartObjIDX;	// 존이 동시에 바꿈
	int				m_iLinkedCartUsrIDX;	// 접속 종료시까지 바뀌지 않음

	#define			RIDE_ATTR_NORMAL		0x01
	#define			RIDE_ATTR_CART			0x02
	#define			RIDE_ATTR_CASTLE		0x04
	#define			RIDE_ATTR_CART_SUB		0x08

	BYTE			m_btRideATTR;		// 0x1: Walk / Run, 0x2: Carts adjustment, 0x4: Castle adjustment, 0x8: cart ride

    CStrVAR			m_Name;

	CObjAVT ();
	virtual ~CObjAVT ();

	void    Set_NAME (char *szName)						{	m_Name.Set(szName);							}
	void	Set_LEVEL(int iValue)						{	SetCur_LEVEL (iValue);						}

	tagITEM	Get_EquipITEM (short nEquipIDX)			    {	return m_Inventory.m_ItemEQUIP[ nEquipIDX ];}
    
	short	GetPartITEM (short nPartIdx)                {	return m_PartITEM[ nPartIdx ].m_nItemNo;	}
//  void    SetPartITEM (short nPartIdx, short nItemNo) {   m_nPartItemIDX[ nPartIdx ] = nItemNo;		}

	short	SetPartITEM (short nEquipInvIDX);
    void    SetPartITEM (short nPartIdx, tagITEM&sITEM) 
	{ 
		if(!m_MileageInv.m_ItemEQUIP[nPartIdx].GetItemNO())
		{
			this->m_PartITEM[ nPartIdx ].m_nItemNo	 = sITEM.GetItemNO();
			this->m_PartITEM[ nPartIdx ].m_nGEM_OP	 = sITEM.GetGemNO();
			this->m_PartITEM[ nPartIdx ].m_cGrade	 = sITEM.GetGrade();
			this->m_PartITEM[ nPartIdx ].m_bHasSocket= sITEM.m_bHasSocket;
		}
	}

	void SetPartITEM(short nPartIdx, MileageItem sITEM)
	{
		if(!sITEM.GetItemNO())
		{
			this->m_PartITEM[ nPartIdx ].m_nItemNo	 = m_Inventory.m_ItemEQUIP[nPartIdx].GetItemNO();
			this->m_PartITEM[ nPartIdx ].m_nGEM_OP	 = m_Inventory.m_ItemEQUIP[nPartIdx].GetGemNO();
			this->m_PartITEM[ nPartIdx ].m_cGrade	 = m_Inventory.m_ItemEQUIP[nPartIdx].GetGrade();
			this->m_PartITEM[ nPartIdx ].m_bHasSocket= m_Inventory.m_ItemEQUIP[nPartIdx].m_bHasSocket;
		}
		else
		{
			this->m_PartITEM[ nPartIdx ].m_nItemNo	 = sITEM.GetItemNO();
			this->m_PartITEM[ nPartIdx ].m_nGEM_OP	 = sITEM.GetGemNO();
			this->m_PartITEM[ nPartIdx ].m_cGrade	 = sITEM.GetGrade();
			this->m_PartITEM[ nPartIdx ].m_bHasSocket= sITEM.m_bHasSocket;
		}
	}

	void	SetRideITEM (short nRideInvIDX);

	short	GetPsv_ATKSPEED ()							{	return this->m_nPassiveAttackSpeed;			}
	void	UpdateAbility()								
	{   
		if ( this->GetCur_MOVE_MODE() == MOVE_MODE_DRIVE||this->GetCur_MOVE_MODE() == MOVE_MODE_RIDEON )
		{
			//2인승카트일경우.. 운전수랑 캐스트랑. 같은 애니를 취한다..
			/*
			m_nPatAniROW = PAT_RELATIVE_MOTION_POS(	m_nRideItemIDX[ RIDE_PART_BODY ] );	// 몸체에서 모션 줄 번호
			m_nPatAniCOL = PAT_RELATIVE_MOTION_POS( m_nRideItemIDX[ RIDE_PART_ARMS ] );	// 무기에서 모션 열 번호
			*/
			m_nPatAniROW = PAT_RELATIVE_MOTION_POS(	m_RideITEM[ RIDE_PART_BODY ].m_nItemNo );	// 몸체에서 모션 줄 번호
			m_nPatAniCOL = PAT_RELATIVE_MOTION_POS( m_RideITEM[ RIDE_PART_ARMS ].m_nItemNo );	// 무기에서 모션 열 번호
		}

		Cal_BattleAbility();
		Update_SPEED();	
	}
	void	Update_SPEED ();
	void	Resurrection (short nSkillIDX);

	classDLLNODE< CObjAVT* >	*m_pTargetNODE;
	virtual char*	Get_StoreTITLE ()=0;
	virtual bool	Send_gsv_HP_REPLY (int iObjectIDX, int iHP)=0;
	virtual bool	Send_gsv_SET_ITEM_LIFE (short nInvIDX, short nLife)=0;
	virtual bool	Send_gsv_SPEED_CHANGED (bool bUpdateSpeed)=0;
	virtual bool	Send_gsv_SETEXP (WORD wFromObjIDX) = 0;

			void	Set_TargetIDX (int iTargetIndex, bool bCheckHP=true);

	void Set_PenalEXP (BYTE btAddPercent)
	{
		if ( this->GetCur_LEVEL() >= 10 ) {
			// 10렙 이상이면 페널티 적용..
			int iNeedEXP  = CCal::Get_NeedRawEXP( m_GrowAbility.m_nLevel );
			int iPenalEXP = (int) ( iNeedEXP * btAddPercent / 100.f );

			m_iAppliedPenaltyEXP = iPenalEXP;
			if ( m_GrowAbility.m_lEXP >= iPenalEXP ) {
				m_GrowAbility.m_lEXP -= iPenalEXP;
			} else {
				iPenalEXP -= m_GrowAbility.m_lEXP;
				m_GrowAbility.m_lPenalEXP += iPenalEXP;
				m_GrowAbility.m_lEXP = 0;
				if ( m_GrowAbility.m_lPenalEXP > DIV02(iNeedEXP) ) {
					m_iAppliedPenaltyEXP -= ( DIV02(iNeedEXP) - m_GrowAbility.m_lPenalEXP );
					m_GrowAbility.m_lPenalEXP = DIV02(iNeedEXP);
				}
			}
		}
	}
	void Cancel_PenalEXP (BYTE btPercent)
	{
		if ( this->GetCur_LEVEL() >= 10 && m_iAppliedPenaltyEXP > 0 ) {
			// 10렙 이상이면 적용했던 페널티 해제...
			m_iAppliedPenaltyEXP = (int)( m_iAppliedPenaltyEXP * btPercent / 100.f );
			if ( m_iAppliedPenaltyEXP ) {
				m_GrowAbility.m_lEXP += m_iAppliedPenaltyEXP;
			}
		}
		m_iAppliedPenaltyEXP = 0;
	}

	bool Send_gsv_GODDNESS_MODE( BYTE btOnOff );
	bool Add_Goddess (void);
	bool Del_Goddess (void);

	void Cal_AruaAtkSPD (void);
	void Cal_AruaRunSPD (void);

	void Cal_AruaMaxHP (void);
	void Cal_AruaMaxMP (void);

	void Cal_AruaATTACK (void);
	void Cal_AruaHIT (void);
	void Cal_AruaAVOID (void);

	void Cal_AruaCRITICAL (void);
	void Cal_AruaRES( void ); // kchs

#ifdef __KCHS_BATTLECART__ 
	bool Send_gsv_PATSTATE_CHAGE( BYTE btOnOff, DWORD dwCoolTIME );
#endif

	bool Is_CartDriver()	{	return ( RIDE_MODE_DRIVE == this->GetCur_RIDE_MODE() && this->m_iLinkedCartObjIDX );	}
	bool Is_CartGuest ()	{	return ( RIDE_MODE_GUEST == this->GetCur_RIDE_MODE() );	}
	void UpdateCartGuest ();
} ;

//-------------------------------------------------------------------------------------------------
#endif
