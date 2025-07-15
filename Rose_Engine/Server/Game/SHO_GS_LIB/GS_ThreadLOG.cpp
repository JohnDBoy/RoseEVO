
#include "LIB_gsMAIN.h"
//#include "GS_SocketLOG.h"
#include "GS_ThreadZONE.h"
#include "IO_Quest.h"

#include "GS_ThreadSQL.h"
#include "GS_ThreadLOG.h"

static char *SZ_ZULY = "ZULY";

/*
enum e_LogTYPE {
	GSLOG_LOGIN=0,
	GSLOG_LOGOUT,

	GSLOG_DEADBY,		// ����

	GSLOG_PICK,			// �������� �ݱ�
	GSLOG_DROP,			// �������� �ٴڿ� ����
	GSLOG_DELETED,		// �ʵ� �������� �����

	GSLOG_CREATED,		// ������ �����ؼ� ������.
	GSLOG_DESTROYED,	// ������ ���۽� �����ؼ� �Ҹ��.

	GSLOG_SELL,			// �������� �Ǹ�
	GSLOG_USE,			// �������� ����ؼ� �Ҹ��

	GSLOG_GIVE,			// �������� �ǳ���
	GSLOG_RECV,			// �������� ����

	GSLOG_DEPOSIT,		// â�� ����
	GSLOG_WITHDRAW,		// â���� ã��
} ;
*/

//-------------------------------------------------------------------------------------------------
/*
bool GS_CThreadLOG::When_ERROR ( classUSER *pUSER, char *szFile, int iLine, char *szDesc )
{
	m_csSQL.Lock ();
	{
		// �ð�, �ɸ�, ����, ��ġ, IP
		this->m_QuerySTR.Printf("INSERT tblGS_ERROR ( dateREG, txtIP, txtACCOUNT, txtCHAR, txtFILE, intLINE, txtDESC ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,\'%s\' );",
				this->GetCurDateTimeSTR(),
				pUSER->Get_IP(),
				pUSER->Get_ACCOUNT(),
				pUSER->Get_NAME() ? pUSER->Get_NAME() : "????",

				szFile,
				iLine,
				szDesc 
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}
*/
//-------------------------------------------------------------------------------------------------
// @param btTYPE : 0:login, 1:logout, 2:char backup

bool GS_CThreadLOG::When_LogInOrOut (classUSER *pUSER, BYTE btType )
{
	m_csSQL.Lock ();
	{
		if ( 0 == pUSER->m_nZoneNO ) {
			this->m_QuerySTR.Printf( SP_AddLoginLog,				//[AddLoginLog]
					btType,											//	@Login tinyint,
					"not selected",									//	@CharName nvarchar(30),
					CLIB_GameSRV::GetInstance()->GetChannelNO(),	//	@Channel tinyint,
					0,												//	@CharLevel smallint,
					(__int64)0,										//	@Money bigint,
					"lobby",										//	@Location varchar(24),
					(int)0,											//	@LocX int,
					(int)0,											//	@LocY int
					pUSER->Get_IP() );								//  @LoginIP varchar(15)
		} else {
			char *szZoneName = ZONE_NAME( pUSER->m_nZoneNO );		
			this->m_QuerySTR.Printf( SP_AddLoginLog,				//[AddLoginLog]
					btType,											//	@Login tinyint,
					pUSER->Get_NAME(),								//	@CharName nvarchar(30),
					CLIB_GameSRV::GetInstance()->GetChannelNO(),	//	@Channel tinyint,
					pUSER->Get_LEVEL(),								//	@CharLevel smallint,
					pUSER->Get_MONEY(),								//	@Money bigint,
					szZoneName?szZoneName:"null",					//	@Location varchar(24),
					(int)pUSER->m_PosCUR.x,							//	@LocX int,
					(int)pUSER->m_PosCUR.y,							//	@LocY int
					pUSER->Get_IP() );								//  @LoginIP varchar(15)
		}
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}






//-------------------------------------------------------------------------------------------------
/*
bool GS_CThreadLOG::When_CreateCHAR ( classUSER *pSourAVT, char *szCharNAME )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				szCharNAME,
				pSourAVT->Get_IP(),

				"AddCHAR",
				pSourAVT->Get_ACCOUNT()
				);

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}
bool GS_CThreadLOG::When_DeleteCHAR ( classUSER *pSourAVT, char *szCharNAME )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf("INSERT tblGS_LOG ( dateREG, txtSUBJECT, txtSBJIP, txtACTION, txtITEM ) VALUES( \'%s\',\'%s\',\'%s\',\'%s\',\'%s\' );",
				this->GetCurDateTimeSTR(),
				szCharNAME,
				pSourAVT->Get_IP(),

				"DelCHAR",
				pSourAVT->Get_ACCOUNT()
				);
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}
*/

//-------------------------------------------------------------------------------------------------
	#define	CHECK_TRADE_MONEY_AMOUNT	10000
	#define	CHECK_ITEM_PRICE			5000


/*
	@iDupCnt ���� �ŷ� ����
*/
char *GS_CThreadLOG::MakeItemDESC( CStrVAR *pStrVAR, tagITEM *pITEM, int iItmePrice, int iDupCnt, int iMoneyAmout, DWORD dwTradeMoney, int iTradeCnt )
{
	if ( pITEM->IsEnableDupCNT() ) {
		// �Ҹ�/��Ÿ
		if ( dwTradeMoney < CHECK_TRADE_MONEY_AMOUNT && 
			 ITEM_BASE_PRICE( pITEM->GetTYPE(), pITEM->GetItemNO() ) * iDupCnt < iItmePrice ) {
			// �����δ� ��� �α׸� �����ʿ� ����.
			if ( (iTradeCnt && iTradeCnt < iDupCnt) || pITEM->GetQuantity() < iDupCnt )
				return NULL;	// ���� �ŷ� ���� �̴�...
		}

		pStrVAR->Printf("[%d:%d]%s : %dEA, TradeZ: %d", pITEM->GetTYPE(), pITEM->GetItemNO(), ITEM_NAME( pITEM->GetTYPE(), pITEM->GetItemNO() ), iTradeCnt ? iTradeCnt : pITEM->GetQuantity(), dwTradeMoney );
	} else
	if ( ITEM_TYPE_MONEY == pITEM->GetTYPE() ) {
		// ��
		if ( dwTradeMoney < CHECK_TRADE_MONEY_AMOUNT && pITEM->GetMoney() < iMoneyAmout )
			return NULL;

		pStrVAR->Printf("ZULY: %d, TradeZ: %d", pITEM->GetMoney(), dwTradeMoney );
	} else {
		// ���
		// if ( pITEM->IsCreated() ||	// ������ �������̳� ???
		if ( dwTradeMoney >= CHECK_TRADE_MONEY_AMOUNT || ITEM_BASE_PRICE( pITEM->GetTYPE(), pITEM->GetItemNO() ) >= iItmePrice )	// ��� �Ϲ� ���.
		{
			pStrVAR->Printf("TradeZ: %d, [%d:%d]%s, G:%d,D:%d,L:%d, O:%d/%s S:%d:, A:%d, SN:%I64d", 
					dwTradeMoney,
					pITEM->GetTYPE(), pITEM->GetItemNO(), 
					ITEM_NAME( pITEM->GetTYPE(), pITEM->GetItemNO() ), 

					pITEM->GetGrade(), pITEM->GetDurability(), pITEM->GetLife(),

					pITEM->GetGemNO(), GEMITEM_NAME( pITEM->GetGemNO() ),
					pITEM->HasSocket(), pITEM->IsAppraisal(),
					pITEM->m_iSN );
		} else
			return NULL;
	}

	return pStrVAR->Get();
}
char *GS_CThreadLOG::GetItemINFO( tagITEM *pITEM, int iItmePrice, int iDupCnt, int iMoneyAmout, DWORD dwTradeMoney, int iTradeCnt )
{
	return this->MakeItemDESC( &m_ItemSTR, pITEM, iItmePrice, iDupCnt, iMoneyAmout, dwTradeMoney, iTradeCnt );
}



bool GS_CThreadLOG::When_CreateOrDestroyITEM ( classUSER *pSourAVT, tagITEM *pOutItem, tagITEM *pUseItem, short nUseCNT, BYTE btMakeOrBreak, BYTE btSucOrFail )
{
	m_csSQL.Lock ();
	{
	//if ( this->GetItemINFO( pITEM, 1000, 1, 1000 ) ) 
	{
		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		if ( pOutItem ) { 
			m_ItemSTR.Printf("%d:%d", pOutItem->GetTYPE(), pOutItem->GetItemNO() );
			this->m_QuerySTR.Printf( SP_CreateLog,	// [AddCreateLog]
					pSourAVT->m_dwDBID,				// @CharID int,
					pSourAVT->Get_NAME(),			// @CharName nvarchar(30),
					m_ItemSTR.Get(),				// @ItemID varchar(10),
					ITEM_NAME( pOutItem->GetTYPE(), pOutItem->GetItemNO() ),						// @ItemName varchar(50),
					nUseCNT>0 ? ITEM_NAME( pUseItem[0].GetTYPE(), pUseItem[0].GetItemNO() ):"none",	// @Stuff1 varchar(24),
					nUseCNT>1 ? ITEM_NAME( pUseItem[1].GetTYPE(), pUseItem[1].GetItemNO() ):"none",	// @Stuff2 varchar(24),
					nUseCNT>2 ? ITEM_NAME( pUseItem[2].GetTYPE(), pUseItem[2].GetItemNO() ):"none",	// @Stuff3 varchar(24),
					nUseCNT>3 ? ITEM_NAME( pUseItem[3].GetTYPE(), pUseItem[3].GetItemNO() ):"none",	// @Stuff4 varchar(24),
					btMakeOrBreak,					// @Making tinyint,
					btSucOrFail,					// @Success tinyint,
					szZoneName?szZoneName:"null",	// @Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,		// @LocX int,
					(int)pSourAVT->m_PosCUR.y );	// @LocY int
		} else {
			this->m_QuerySTR.Printf( SP_CreateLog,	// [AddCreateLog]
					pSourAVT->m_dwDBID,				// @CharID int,
					pSourAVT->Get_NAME(),			// @CharName nvarchar(30),
					"none",							// @ItemID varchar(10),
					"none",							// @ItemName varchar(50),
					nUseCNT>0 ? ITEM_NAME( pUseItem[0].GetTYPE(), pUseItem[0].GetItemNO() ):"none",	// @Stuff1 varchar(24),
					nUseCNT>1 ? ITEM_NAME( pUseItem[1].GetTYPE(), pUseItem[1].GetItemNO() ):"none",	// @Stuff2 varchar(24),
					nUseCNT>2 ? ITEM_NAME( pUseItem[2].GetTYPE(), pUseItem[2].GetItemNO() ):"none",	// @Stuff3 varchar(24),
					nUseCNT>3 ? ITEM_NAME( pUseItem[3].GetTYPE(), pUseItem[3].GetItemNO() ):"none",	// @Stuff4 varchar(24),
					btMakeOrBreak,					// @Making tinyint,
					btSucOrFail,					// @Success tinyint,
					szZoneName?szZoneName:"null",	// @Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,		// @LocX int,
					(int)pSourAVT->m_PosCUR.y );	// @LocY int
		}

		this->Send_gsv_LOG_SQL();
	}
	}
	m_csSQL.Unlock ();

	return true;
}


//------------------------------------------------------------------------------------------------
bool GS_CThreadLOG::When_DieBY ( CObjCHAR *pKillOBJ, classUSER *pDeadAVT )
{
	m_csSQL.Lock ();
	{

		char *szZoneName = ZONE_NAME( pDeadAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_AddDieLog,	// [AddDieLog]
				pDeadAVT->Get_NAME(),			// @CharName varchar(32),
				pDeadAVT->Get_MONEY(),			// @Money bigint,
				szZoneName?szZoneName:"null",	// @KillPos varchar(24),
				pDeadAVT->Get_LEVEL(),			// @CharLevel smallint,
				pDeadAVT->Get_EXP(),			// @Exp int,
				(int)pDeadAVT->m_PosCUR.x,		// @PosX int,
				(int)pDeadAVT->m_PosCUR.y,		// @PosY int,
				pKillOBJ->Get_NAME() );			// @ObjectName varchar(32)

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// ������
bool GS_CThreadLOG::When_LevelUP ( classUSER *pSourAVT, int iGetEXP  )
{
	// LogString(LOG_NORMAL, ">>> %s LEVEL UP to %d,  CurEXP: %d, GetEXP: %d\n", pSourAVT->Get_NAME(), pSourAVT->Get_LEVEL(), pSourAVT->m_GrowAbility.m_lEXP, iGetEXP);

	m_csSQL.Lock ();
	{

		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_LevelUpLog,	//[AddLevelUpLog]
				pSourAVT->m_dwDBID,				//	@CharID int,
				pSourAVT->Get_NAME(),			//	@CharName nvarchar(30),
				pSourAVT->Get_LEVEL(),			//	@toLevel smallint,
				pSourAVT->GetCur_BonusPOINT(),	//	@BPoint smallint,
				pSourAVT->GetCur_SkillPOINT(),  //	@SPoint smallint,
				szZoneName?szZoneName:"null",   //	@Location varchar(24),
				(int)pSourAVT->m_PosCUR.x,		//	@LocX int,
				(int)pSourAVT->m_PosCUR.y );	//	@LocY int


		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}

//-------------------------------------------------------------------------------------------------
// ��ų...
bool GS_CThreadLOG::When_LearnSKILL ( classUSER *pSourAVT, short nSkillIDX )
{
	m_csSQL.Lock ();
	{

		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_SkillLog,	//[AddSkillLog]
				pSourAVT->m_dwDBID,				//	@CharID int,
				pSourAVT->Get_NAME(),			//	@CharName nvarchar(30),
				nSkillIDX,						//	@SkillID int,
				SKILL_NAME ( nSkillIDX ),		//	@SkillName varchar(24),
				SKILL_LEVEL( nSkillIDX ),		//	@SkillLevel smallint,
				pSourAVT->GetCur_SkillPOINT(),	//	@SPoint smallint,
				szZoneName?szZoneName:"null",	//	@Location varchar(24),
				(int)pSourAVT->m_PosCUR.x,		//	@LocX int,
				(int)pSourAVT->m_PosCUR.y );	//	@LocY int

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------


bool GS_CThreadLOG::When_QuestLOG ( classUSER *pSourAVT, int iQuestIDX, BYTE btLogType )
{
	m_csSQL.Lock ();
	{
		this->m_QuerySTR.Printf( SP_AddQuestLog,	//[AddQuestLog]
				pSourAVT->m_dwDBID,					//	@CharID int,
				pSourAVT->Get_NAME(),				//	@CharName nvarchar(30),
				iQuestIDX,							//	@QuestID int,
				btLogType);							//	@QuestDo tinyint
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


bool GS_CThreadLOG::When_GemmingITEM ( classUSER *pSourAVT,	tagITEM *pEquipITEM, tagITEM *pJewelITEM, BYTE btGemming, BYTE btSuccess )
{
	m_csSQL.Lock ();
	{
		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		m_ItemSTR.Printf("%d:%d", pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() );
		if ( pJewelITEM ) {
			m_DescSTR.Printf("%d", pJewelITEM->GetItemNO() );
			this->m_QuerySTR.Printf( SP_GemmingLog,									//[AddGemmingLog]
					pSourAVT->m_dwDBID,												//	@CharID int,
					pSourAVT->Get_NAME(),											//	@CharName nvarchar(30),
					m_ItemSTR.Get(),												//	@ItemID varchar(10),
					ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ),	//	@ItemName varchar(50),
					m_DescSTR.Get(),												//	@JewelID varchar(10),
					GEMITEM_NAME( pJewelITEM->GetItemNO() ),						//	@JewelName varchar(24),
					btGemming,														//	@Gemming tinyint,
					btSuccess,														//	@Success tinyint,
					szZoneName?szZoneName:"null",									//	@Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,										//	@LocX int,
					(int)pSourAVT->m_PosCUR.y );									//	@LocY int
		} else {
			this->m_QuerySTR.Printf( SP_GemmingLog,									//[AddGemmingLog]
					pSourAVT->m_dwDBID,												//	@CharID int,
					pSourAVT->Get_NAME(),											//	@CharName nvarchar(30),
					m_ItemSTR.Get(),												//	@ItemID varchar(10),
					ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ),	//	@ItemName varchar(50),
					"0",															//	@JewelID varchar(10),
					GEMITEM_NAME( pEquipITEM->GetGemNO() ),							//	@JewelName varchar(24),
					btGemming,														//	@Gemming tinyint,
					btSuccess,														//	@Success tinyint,
					szZoneName?szZoneName:"null",									//	@Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,										//	@LocX int,
					(int)pSourAVT->m_PosCUR.y );									//	@LocY int
		}

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}



bool GS_CThreadLOG::When_UpgradeITEM ( classUSER *pSourAVT, tagITEM *pEquipITEM, BYTE btBeforeGrade, BYTE btResult )
{

	if(!pEquipITEM->GetTYPE())
	{
		g_LOG.OutputString(LOG_NORMAL,"ERROR: When_UpgradeITEM - NAME:%s  ITEMTYPE %d",pSourAVT->Get_NAME(),pEquipITEM->GetTYPE());
		return true;
	}

	m_csSQL.Lock ();
	{

		m_ItemSTR.Printf("%d:%d", pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() );
		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_UpgradeLog,								//[AddUpgradeLog]
				pSourAVT->m_dwDBID,											//	@CharID int,
				pSourAVT->Get_NAME(),										//	@CharName nvarchar(30),
				this->m_ItemSTR.Get(),										//	@ItemID varchar(10),
				ITEM_NAME( pEquipITEM->GetTYPE(), pEquipITEM->GetItemNO() ),//	@ItemName varchar(50),
				btBeforeGrade+1,											//	@UpLevel smallint,
				btResult,													//	@Success tinyint,
				szZoneName?szZoneName:"null",								//	@Location varchar(24),
				(int)pSourAVT->m_PosCUR.x,									//	@LocX int,
				(int)pSourAVT->m_PosCUR.y );								//	@LocY int

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool GS_CThreadLOG::When_ChangeUNION ( classUSER *pSourAVT, int iBeforeUnion, int iNewUnion )
{
		m_csSQL.Lock ();
		{
			int iCurPnt, iAfterPnt;

			short nAction;
			if ( iBeforeUnion ) {
				iCurPnt   = pSourAVT->GetCur_JoHapPOINT( iBeforeUnion );
				if ( iNewUnion ) {
					iAfterPnt = pSourAVT->GetCur_JoHapPOINT( iNewUnion );
					nAction = NEWLOG_UNION_CHANGE;
				} else {
					iAfterPnt = 0;
					nAction = NEWLOG_UNION_QUIT;
				}
			} else 
			if ( iNewUnion ) {
				iCurPnt = 0;
				iAfterPnt = pSourAVT->GetCur_JoHapPOINT( iNewUnion );
				nAction = NEWLOG_UNION_JOIN;
			} else
				return true;

			char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
			this->m_QuerySTR.Printf( SP_AddUnionLog, 		//[AddUnionLog]
					pSourAVT->m_dwDBID,						//@iCharID int,
					pSourAVT->Get_NAME(),					//@szCharName nvarchar(30),
					nAction,								//@nAction smallint,
					iBeforeUnion,							//@nCurUnion smallint,
					iCurPnt,								//@iCurPoint int,
					iNewUnion,								//@nAfterUnion smallint,
					iAfterPnt,								//@iAfterPoint int,
					szZoneName?szZoneName:"null",			//@szLocation varchar(50),
					(int)pSourAVT->m_PosCUR.x,				//@iLocX int,
					(int)pSourAVT->m_PosCUR.y );			//@iLocY int
			this->Send_gsv_LOG_SQL ();
		}
		m_csSQL.Unlock ();
	return true;
}

bool GS_CThreadLOG::When_PointUNION	( classUSER *pSourAVT, int iCurUnion, int iCurPoint, int iNewPoint )
{
		m_csSQL.Lock ();
		{
			// ���� ����Ʈ�� ���ջ������� ������ ���Ž� ITEM log�� �Բ� ���� ����Ʈ �αװ� ���� ���´�...
			short nAction;
			if ( iNewPoint > iCurPoint ) {
				nAction = NEWLOG_UNION_ADD_POINT;
			} else {
				nAction = NEWLOG_UNION_USE_POINT;
			}

			char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
			this->m_QuerySTR.Printf( SP_AddUnionLog, 		//[AddUnionLog]
					pSourAVT->m_dwDBID,						//@iCharID int,
					pSourAVT->Get_NAME(),					//@szCharName nvarchar(30),
					nAction,								//@nAction smallint,
					iCurUnion,								//@nCurUnion smallint,
					iCurPoint,								//@iCurPoint int,
					iCurUnion,								//@nAfterUnion smallint,
					iNewPoint,								//@iAfterPoint int,
					szZoneName?szZoneName:"null",			//@szLocation varchar(50),
					(int)pSourAVT->m_PosCUR.x,				//@iLocX int,
					(int)pSourAVT->m_PosCUR.y );			//@iLocY int
			this->Send_gsv_LOG_SQL ();
		}
		m_csSQL.Unlock ();
	return true;
}


//-------------------------------------------------------------------------------------------------
// ��� �α�.
bool GS_CThreadLOG::When_BackUP ( classUSER *pSourAVT,  char *szBackUpType )
{
	m_csSQL.Lock ();
	{

		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_AddPeriodicCHARLog,			// [AddPeriodicCHARLog]
				pSourAVT->Get_NAME(),							// @CharName varchar(32),
				CLIB_GameSRV::GetInstance()->GetChannelNO(),	// @Channel tinyint,
				pSourAVT->Get_LEVEL(),							// @CharLevel smallint,
				pSourAVT->Get_MONEY(),							// @Money bigint,
				pSourAVT->Get_EXP(),							// @Exp int,
				pSourAVT->GetCur_BonusPOINT(),					// @BPoint smallint,
				pSourAVT->GetCur_SkillPOINT(),					// @SPoint smallint,
				szZoneName?szZoneName:"null",					// @Location varchar(24),
				(int)pSourAVT->m_PosCUR.x,						// @LocX int,
				(int)pSourAVT->m_PosCUR.y );					// @LocY int

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	return true;
}


bool GS_CThreadLOG::When_CheatCODE ( classUSER *pSourAVT, char *szCode )
{
	m_csSQL.Lock ();
	{
//-------------------------------------
/*
		this->m_QuerySTR.Printf( SP_AddCheatLog,			// [AddCheatLog] 
				pSourAVT->Get_ACCOUNT(),					// @Account nvarchar(20),
				pSourAVT->Get_NAME(),						// @CharName nvarchar(30),
				CLIB_GameSRV::GetInstance()->GetChannelNO(),// @ChannelNo tinyint,
				szCode,										// @CheatCode varchar(25),
				"." );										// @DesCharName nvarchar(30)
*/
// 2006.06.16/��뼺/�߰� - �̱��� ��û���� '/' �� �����ϴ� ��� �޽��� �α� �����, 
		char szChat[55];	// ��� �޽����� ���ܾ� �ϱ⶧���� ���̰� Ŀ������ �ִ�.
		strncpy(szChat, szCode, sizeof(szChat)-1);
		szChat[54] = '\0';
		this->m_QuerySTR.Printf( SP_AddCheatLog,			// [AddCheatLog] 
			pSourAVT->Get_ACCOUNT(),					// @Account nvarchar(20),
			pSourAVT->Get_NAME(),						// @CharName nvarchar(30),
			CLIB_GameSRV::GetInstance()->GetChannelNO(),// @ChannelNo tinyint,
			szChat,										// @CheatCode varchar(55),
			"." );										// @DesCharName nvarchar(30)
//-------------------------------------


		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();

	return true;
}


//-------------------------------------------------------------------------------------------------
bool GS_CThreadLOG::When_ItemHACKING ( classUSER *pSourAVT, tagITEM *pHackITEM, char *szLogType )
{
	m_csSQL.Lock ();
	{
	if ( this->GetItemINFO( pHackITEM, 1, 1, 1 ) ) {
		this->m_QuerySTR.Printf( SP_M_OBJLOG,
				pSourAVT->GetCur_MONEY() - pSourAVT->m_i64StartMoney,
				pSourAVT->Get_NAME(),
				pSourAVT->Get_IP(),

				szLogType,	// "ItemHACK", "BankHACK"
				this->GetLocationSTR( pSourAVT->m_nZoneNO, pSourAVT->m_PosCUR ),
				this->m_ItemSTR.Get(),

				pSourAVT->Get_ACCOUNT(),
				pSourAVT->Get_ACCOUNT()
				);
		this->Send_gsv_LOG_SQL();
	}
	}
	m_csSQL.Unlock ();
	return true;
}


//-------------------------------------------------------------------------------------------------
/*
[Success] Column
	 0 : Ŭ�� ���� ����
	 1 : Ŭ�� ���� ����
	 2 : Ŭ�� ���� ����
	 3 : Ŭ�� ��ü
*/
bool GS_CThreadLOG::When_gs_CLAN ( classUSER *pSourAVT, char *szLogType, BYTE btSucType )
{
	m_csSQL.Lock ();
	{

		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_AddClanLog,	// [AddClanLog]
				pSourAVT->Get_NAME(),			// @CharName nvarchar(32),
				pSourAVT->GetClanNAME(),		// @ClanName nvarchar(20),
				0,								// @ClanLevel smallint,
				0,								// @Point int,
				btSucType,						// @Success tinyint,
				szZoneName?szZoneName:"null",   // @Location varchar(24),
				(int)pSourAVT->m_PosCUR.x,		// @LocX int,
				(int)pSourAVT->m_PosCUR.y );	// @LocY int
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	return true;
}

//-------------------------------------------------------------------------------------------------

bool GS_CThreadLOG::Check_ItemLogFilter( tagITEM *pITEM, short nQuantity, __int64 biZuly )
{
	if ( pITEM->IsEnableDupCNT() ) {
		// �Ҹ�/��Ÿ
		if ( biZuly < CHECK_TRADE_MONEY_AMOUNT && 
			 ITEM_BASE_PRICE( pITEM->GetTYPE(), pITEM->GetItemNO() ) * nQuantity < CHECK_TRADE_MONEY_AMOUNT ) {
			return false;	// ���� �ŷ� ���� �̴�...
		}
	} else
	if ( ITEM_TYPE_MONEY == pITEM->GetTYPE() ) {
		// ��
		if ( biZuly < CHECK_TRADE_MONEY_AMOUNT && pITEM->GetMoney() < CHECK_TRADE_MONEY_AMOUNT )
			return false;
	} else {
		// ���
		__int64 biTotPrice = ITEM_BASE_PRICE( pITEM->GetTYPE(), pITEM->GetItemNO() );
		if ( /* pITEM->m_bHasSocket && */ pITEM->GetGemNO() > 300 ) {
			// ���Ͽ� ���� ������ ������...���� ������...
			biTotPrice += GEMITEM_BASE_PRICE( pITEM->GetGemNO() );
		}

		// if ( pITEM->IsCreated() ||	// ������ �������̳� ???
		if ( biZuly >= CHECK_TRADE_MONEY_AMOUNT || biTotPrice >= CHECK_TRADE_MONEY_AMOUNT )	{	// ��� ���.
			;
		} else
			return false;
	}

	return true;
}

// bForce ���� true�� ������ ����ġ ������ �ʰ� �׻� ����
bool GS_CThreadLOG::When_TagItemLOG( int iAction, classUSER *pSourAVT, tagITEM *pItem, short nQuantity, __int64 biTradeZuly, classUSER *pDestAVT, bool bForce)
{
	m_csSQL.Lock ();
	{
		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		if ( pItem ) {
			if ( 0 == nQuantity ) {
				nQuantity = pItem->IsEnableDupCNT() ? pItem->GetQuantity () : 1;
			}

			if ( !bForce && !this->Check_ItemLogFilter( pItem, nQuantity, biTradeZuly ) ) {				
				// ���� ���� ���� �ݾ��̴ϱ� ���� ���԰� ������ return. �� �׷��� ITEM_USE_RESTRICTION���� ����
				if( pItem->GetTYPE() == ITEM_TYPE_MONEY )
				{
					m_csSQL.Unlock ();	
					return true;
				}

				// �α� ���� �̴�...
				/*				
				1ȸ�� �����̳�, 1ȸ�� ��� ������ ����, �ٸ� ���ϸ��� �������� 3��Į��(�������)�� 6
				6�ϰ�� ��ȯ�� �ȵǵ� ����â�� ������ �����ϰ� ���� 6�ϰ�쿡 ���ϸ��� ���������� �Ǵ� */
				// if ( pItem != ���ϸ��� ������ ) 
				if( ITEM_USE_RESTRICTION( pItem->GetTYPE(), pItem->GetItemNO() ) != 6 )
				{
					m_csSQL.Unlock ();	
					return true;
				}
			}

			char *szItemName;
			if ( ITEM_TYPE_MONEY == pItem->GetTYPE() ) {
				szItemName = SZ_ZULY;
			} else {
				szItemName = ITEM_NAME( pItem->GetTYPE(), pItem->GetItemNO() );
			}

			m_ItemSTR.Printf("%d:%d", pItem->GetTYPE(), pItem->GetItemNO() );
			if ( pDestAVT ) {
				this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
						iAction,						// @Action int,
						pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
						pSourAVT->m_dwDBID,				// @SbjCharID int,
						pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
						m_ItemSTR.Get(),				// @ItemID varchar(10),
						szItemName,						// @ItemName varchar(50),
						nQuantity,						// @ItemCount smallint,
						pItem->m_iSN,					// @ItemSN bigint,
						pItem->GetOption (),			// @ItemOpt smallint,
						biTradeZuly,					// @Money bigint,
						szZoneName?szZoneName:"null",	// @Location varchar(24),
						(int)pSourAVT->m_PosCUR.x,		// @LocX int,
						(int)pSourAVT->m_PosCUR.y,		// @LocY int,
						pDestAVT->Get_ACCOUNT(),		// @ObjAccount nvarchar(20),
						pDestAVT->m_dwDBID,				// @ObjCharID int,
						pDestAVT->Get_NAME(),			// @ObjCharName nvarchar(30),
						pSourAVT->Get_IP(),				// @SbjIP varchar(15),
						pDestAVT->Get_IP()				// @ObjIP varchar(15)
					) ;
			} else {
				this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
						iAction,						// @Action int,
						pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
						pSourAVT->m_dwDBID,				// @SbjCharID int,
						pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
						m_ItemSTR.Get(),				// @ItemID varchar(10),
						szItemName,						// @ItemName varchar(50),
						nQuantity,						// @ItemCount smallint,
						pItem->m_iSN,					// @ItemSN bigint,
						pItem->GetOption (),			// @ItemOpt smallint,
						biTradeZuly,					// @Money bigint,
						szZoneName?szZoneName:"null",	// @Location varchar(24),
						(int)pSourAVT->m_PosCUR.x,		// @LocX int,
						(int)pSourAVT->m_PosCUR.y,		// @LocY int,
						NULL,							// @ObjAccount nvarchar(20),
						0,								// @ObjCharID int,
						NULL,							// @ObjCharName nvarchar(30),
						pSourAVT->Get_IP(),				// @SbjIP varchar(15),
						NULL							// @ObjIP varchar(15)
					) ;
			}
		} else {
 
//-------------------------------------
// 2007.01.17/��뼺/�߰� - â���� ������ �ְų� ���� �α׳���� ����� �κ��丮 �ǽð� ����
			if(bForce == false)
			{
//------------------------------------
			if ( biTradeZuly < 10000 ) {
				// �α� ���� �̴�...
				m_csSQL.Unlock ();
				return true;
			}
//-------------------------------------
// 2007.01.17/��뼺/�߰� - â���� ������ �ְų� ���� �α׳���� ����� �κ��丮 �ǽð� ����
			}
//-------------------------------------

			if ( pDestAVT ) {
				this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
						iAction,						// @Action int,
						pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
						pSourAVT->m_dwDBID,				// @SbjCharID int,
						pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
						NULL,							// @ItemID varchar(10),
						NULL,							// @ItemName varchar(50),
						0,								// @ItemCount smallint,
						(__int64)0,						// @ItemSN bigint,
						(int)0,
						biTradeZuly,					// @Money bigint,
						szZoneName?szZoneName:"null",	// @Location varchar(24),
						(int)pSourAVT->m_PosCUR.x,		// @LocX int,
						(int)pSourAVT->m_PosCUR.y,		// @LocY int,
						pDestAVT->Get_ACCOUNT(),		// @ObjAccount nvarchar(20),
						pDestAVT->m_dwDBID,				// @ObjCharID int,
						pDestAVT->Get_NAME(),			// @ObjCharName nvarchar(30),
						pSourAVT->Get_IP(),				// @SbjIP varchar(15),
						pDestAVT->Get_IP()				// @ObjIP varchar(15)
					) ;
			} else {
				this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
						iAction,						// @Action int,
						pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
						pSourAVT->m_dwDBID,				// @SbjCharID int,
						pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
						NULL,							// @ItemID varchar(10),
						NULL,							// @ItemName varchar(50),
						0,								// @ItemCount smallint,
						(__int64)0,						// @ItemSN bigint,
						(int)0,
						biTradeZuly,					// @Money bigint,
						szZoneName?szZoneName:"null",	// @Location varchar(24),
						(int)pSourAVT->m_PosCUR.x,		// @LocX int,
						(int)pSourAVT->m_PosCUR.y,		// @LocY int,
						NULL,							// @ObjAccount nvarchar(20),
						0,								// @ObjCharID int,
						NULL,							// @ObjCharName nvarchar(30),
						pSourAVT->Get_IP(),				// @SbjIP varchar(15),
						NULL							// @ObjIP varchar(15)
					) ;
			}
		}

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	return true;
}

bool GS_CThreadLOG::When_ObjItemLOG( int iAction, classUSER *pSourAVT, CObjITEM *pItemOBJ)
{
	m_csSQL.Lock ();
	{
		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		short nQuantity;
		__int64 biTradeZuly;
		char *szItemName;
		if ( ITEM_TYPE_MONEY == pItemOBJ->m_ITEM.GetTYPE() ) {
			biTradeZuly = pItemOBJ->m_ITEM.GetMoney();
			nQuantity = 0;
			szItemName = SZ_ZULY;
		} else {
			biTradeZuly = 0;
			nQuantity = pItemOBJ->m_ITEM.IsEnableDupCNT() ? pItemOBJ->m_ITEM.GetQuantity () : 1;
			szItemName = ITEM_NAME( pItemOBJ->m_ITEM.GetTYPE(), pItemOBJ->m_ITEM.GetItemNO() );
		}

		if ( !this->Check_ItemLogFilter( &pItemOBJ->m_ITEM, nQuantity, biTradeZuly ) ) {
			// �α� ���� �̴�...
			m_csSQL.Unlock ();
			return true;
		}

		m_ItemSTR.Printf("%d:%d", pItemOBJ->m_ITEM.GetTYPE(), pItemOBJ->m_ITEM.GetItemNO() );
		if ( pItemOBJ->m_bDropperIsUSER ) {
			this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
					iAction,						// @Action int,
					pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
					pSourAVT->m_dwDBID,				// @SbjCharID int,
					pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
					m_ItemSTR.Get(),				// @ItemID varchar(10),
					szItemName,						// @ItemName varchar(50),
					nQuantity,						// @ItemCount smallint,
					pItemOBJ->m_ITEM.m_iSN,			// @ItemSN bigint,
					pItemOBJ->m_ITEM.GetOption (),	
					biTradeZuly,					// @Money bigint,
					szZoneName?szZoneName:"null",	// @Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,		// @LocX int,
					(int)pSourAVT->m_PosCUR.y,		// @LocY int,
					"<unknown>",					// @ObjAccount nvarchar(20),
					0,								// @ObjCharID int,
					pItemOBJ->m_DropperNAME.Get(),	// @ObjCharName nvarchar(30),
					pSourAVT->Get_IP(),				// @SbjIP varchar(15),
					pItemOBJ->m_DropperIP.Get()		// @ObjIP varchar(15)
				) ;
		} else {
			this->m_QuerySTR.Printf( SP_AddItemLog,	// [AddItemLog]
					iAction,						// @Action int,
					pSourAVT->Get_ACCOUNT(),		// @SbjAccount nvarchar(20),
					pSourAVT->m_dwDBID,				// @SbjCharID int,
					pSourAVT->Get_NAME(),			// @SbjCharName nvarchar(30),
					m_ItemSTR.Get(),				// @ItemID varchar(10),
					szItemName,						// @ItemName varchar(50),
					nQuantity,						// @ItemCount smallint,
					pItemOBJ->m_ITEM.m_iSN,			// @ItemSN bigint,
					pItemOBJ->m_ITEM.GetOption (),
					biTradeZuly,					// @Money bigint,
					szZoneName?szZoneName:"null",	// @Location varchar(24),
					(int)pSourAVT->m_PosCUR.x,		// @LocX int,
					(int)pSourAVT->m_PosCUR.y,		// @LocY int,
					NULL,							// @ObjAccount nvarchar(20),
					0,								// @ObjCharID int,
					NULL,							// @ObjCharName nvarchar(30),
					pSourAVT->Get_IP(),				// @SbjIP varchar(15),
					NULL							// @ObjIP varchar(15)
				) ;
		}

		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	return true;
}



bool GS_CThreadLOG::When_DeletedITEM ( CObjITEM *pItemOBJ )
{
	// ����ڰ� ���� �ʵ� ������ �����.
	m_csSQL.Lock ();
	{

		{
			__int64 biTradeZuly;
			short	nQuantity;
			
			if ( ITEM_TYPE_MONEY == pItemOBJ->m_ITEM.GetTYPE() ) {
				biTradeZuly = pItemOBJ->m_ITEM.GetMoney();
				nQuantity = 0;
			} else 
			if ( pItemOBJ->m_ITEM.IsEnableDupCNT() ) {
				biTradeZuly = 0;
				nQuantity = pItemOBJ->m_ITEM.GetQuantity();
			} else {
				biTradeZuly = 0;
				nQuantity = 0;
			}
			if ( !this->Check_ItemLogFilter( &pItemOBJ->m_ITEM, nQuantity, biTradeZuly ) ) {
				// �α� ���� �̴�...
				m_csSQL.Unlock ();
				return true;
			}

			char *szZoneName = ZONE_NAME( pItemOBJ->GetZONE()->Get_ZoneNO() );
			if ( ITEM_TYPE_MONEY != pItemOBJ->m_ITEM.GetTYPE() ) {
				m_ItemSTR.Printf("%d:%d", pItemOBJ->m_ITEM.GetTYPE(), pItemOBJ->m_ITEM.GetItemNO() );
				this->m_QuerySTR.Printf( SP_AddItemLog,			// [AddItemLog]
							LIA_TIMEOUT,						// @Action int,
							pItemOBJ->m_DropperACCOUNT.Get(),	// @SbjAccount nvarchar(20),
							pItemOBJ->m_dwDropperDBID,			// @SbjCharID int,
							pItemOBJ->m_DropperNAME.Get(),		// @SbjCharName nvarchar(30),
							m_ItemSTR.Get(),					// @ItemID varchar(10),
							ITEM_NAME( pItemOBJ->m_ITEM.GetTYPE(), pItemOBJ->m_ITEM.GetItemNO() ),	// @ItemName varchar(50),
							nQuantity,							// @ItemCount smallint,
							pItemOBJ->m_ITEM.m_iSN,				// @ItemSN bigint,
							pItemOBJ->m_ITEM.GetOption (),

							biTradeZuly,						// @Money bigint,
							szZoneName?szZoneName:"null",		// @Location varchar(24),
							(int)pItemOBJ->m_PosCUR.x,			// @LocX int,
							(int)pItemOBJ->m_PosCUR.y,			// @LocY int,
							NULL,								// @ObjAccount nvarchar(20),
							0,									// @ObjCharID int,
							NULL,								// @ObjCharName nvarchar(30),
							pItemOBJ->m_DropperIP.Get(),		// @SbjIP varchar(15),
							NULL );								// @ObjIP varchar(15)
			} else {
				// ���̴�...������ �̸� ����...
				m_ItemSTR.Printf("%d:%d", pItemOBJ->m_ITEM.GetTYPE(), 0 );
				this->m_QuerySTR.Printf( SP_AddItemLog,			// [AddItemLog]
							LIA_TIMEOUT,						// @Action int,
							pItemOBJ->m_DropperACCOUNT.Get(),	// @SbjAccount nvarchar(20),
							pItemOBJ->m_dwDropperDBID,			// @SbjCharID int,
							pItemOBJ->m_DropperNAME.Get(),		// @SbjCharName nvarchar(30),
							m_ItemSTR.Get(),					// @ItemID varchar(10),
							"Zuly",								// @ItemName varchar(50),
							nQuantity,							// @ItemCount smallint,
							pItemOBJ->m_ITEM.m_iSN,				// @ItemSN bigint,
							pItemOBJ->m_ITEM.GetOption (),
							biTradeZuly,						// @Money bigint,
							szZoneName?szZoneName:"null",		// @Location varchar(24),
							(int)pItemOBJ->m_PosCUR.x,			// @LocX int,
							(int)pItemOBJ->m_PosCUR.y,			// @LocY int,
							NULL,								// @ObjAccount nvarchar(20),
							0,									// @ObjCharID int,
							NULL,								// @ObjCharName nvarchar(30),
							pItemOBJ->m_DropperIP.Get(),		// @SbjIP varchar(15),
							NULL );								// @ObjIP varchar(15)
			}

			this->Send_gsv_LOG_SQL();
		}
	}
	m_csSQL.Unlock ();

	return true;
}



bool GS_CThreadLOG::When_ChangeABILITY( classUSER * pSourAVT , BYTE btAbilityTYPE, short nUsedBPOINT )
{
	m_csSQL.Lock ();
	{
// SP_M_CHAGEABILITY "{call AddChangeAbility(%d, \'%s\', \'%s\', %d, %d, \'%s\', \'%s\', %d, %d)}"

		char *szZoneName = ZONE_NAME( pSourAVT->m_nZoneNO );
		this->m_QuerySTR.Printf( SP_M_CHAGEABILITY, 
			pSourAVT->m_dwDBID,
			pSourAVT->Get_ACCOUNT (), 
			pSourAVT->Get_NAME(),
			btAbilityTYPE,
			nUsedBPOINT,
			pSourAVT->Get_IP(),
			szZoneName?szZoneName:"null",
			(int)pSourAVT->m_PosCUR.x,		// @LocX int,
			(int)pSourAVT->m_PosCUR.y );	// @LocY int
		this->Send_gsv_LOG_SQL();
	}
	m_csSQL.Unlock ();
	return true;
}

//-------------------------------------------------------------------------------------------------
