#include "stdAFX.h"
#include "CItem.h"
#include "IO_STB.h"

#ifndef	__SERVER
	#include "CInventory.h"
	#include "Object.h"

	#include "../Util/LogWnd.h"
#endif

enum
{
	DROP_TIEM_GOLD = 0,
	DROP_TIEM_SILVER = 207,
	DROP_TIEM_BLOND = 208,
};


//-------------------------------------------------------------------------------------------------
// 서버에서는 구조체이름이 반드시 tagITEM...
short tagITEM::Subtract (tagITEM &sITEM)
{
	if ( this->GetHEADER() != sITEM.GetHEADER() ) {
		sITEM.Clear();
		return 0;
	}

	short nWeight;
	if ( this->IsEnableDupCNT() ) {
		if ( this->GetQuantity() > sITEM.GetQuantity() ) {
			this->m_uiQuantity -= sITEM.GetQuantity();

			return ( ITEM_WEIGHT( sITEM.m_cType, sITEM.m_nItemNo ) * sITEM.GetQuantity() );
		}

		// 현재 보유 수량.
		nWeight = ITEM_WEIGHT( sITEM.m_cType, sITEM.m_nItemNo ) * this->GetQuantity();
		sITEM.m_uiQuantity = this->GetQuantity();
	} else
		nWeight = ITEM_WEIGHT( sITEM.m_cType, sITEM.m_nItemNo );
	
	// 장비 아이템은 무조건 삭제..
	this->Clear();

	return nWeight;
}
//-------------------------------------------------------------------------------------------------
// 서버에서는 구조체이름이 반드시 tagITEM...
void tagITEM::SubtractOnly (tagITEM &sITEM)
{
	if ( this->GetHEADER() != sITEM.GetHEADER() )
		return;

	if ( this->IsEnableDupCNT() ) {
		if ( this->GetQuantity() > sITEM.GetQuantity() ) {
			this->m_uiQuantity -= sITEM.GetQuantity();
			return;
		}
	}
	
	// 장비 아이템은 무조건, 갯수 0개인 아이템 삭제..
	this->Clear();
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
void tagBaseITEM::Init(int iItem, unsigned int uiQuantity)
{	
	this->Clear();

#ifndef	__SERVER
	if( iItem == 0 )
	{
		//--------------------------------------------------------------------------------
		LOGERR( "tagITEM::Init( 0 ) : ITEM Type is 0" );
		//--------------------------------------------------------------------------------
		return;
	}
#endif
	
	if ( 1001 > iItem )
		return;
	
//-------------------------------------
/*
	this->m_cType    = (char)(iItem / 1000);
	this->m_nItemNo  = iItem % 1000;
*/
// 2006.0-7.19/김대성/수정 - 아이템 999 제한 확장
	if(iItem < 1000000)
	{
		this->m_cType    = (char)(iItem / 1000);
		this->m_nItemNo  = iItem % 1000;
	}
	else
	{
		this->m_cType    = (char)(iItem / 1000000);
		this->m_nItemNo  = iItem % 1000000;
	}
//-------------------------------------

	if ( this->IsEnableDupCNT() ) {
		if ( uiQuantity > MAX_DUP_ITEM_QUANTITY )
			uiQuantity = 1;
		this->m_uiQuantity = uiQuantity;
	} else {
		this->m_nLife	    = MAX_ITEM_LIFE;		// 기본 수명은 1000으로 ..
		this->m_cDurability = ITEM_DURABITY( this->m_cType, this->m_nItemNo );
	}


}

//-------------------------------------------------------------------------------------------------
//-------------------------------------
/*
bool tagBaseITEM::IsValidITEM (WORD wType, WORD wItemNO)
*/
// 2006.07.11/김대성/수정 - 아이템 999 제한 확장
bool tagBaseITEM::IsValidITEM (WORD wType, int wItemNO)
//-------------------------------------
{
	if ( 0 == wType || 0 == wItemNO )
		return false;

	if ( wType > ITEM_TYPE_RIDE_PART )
		return false;
	if ( NULL == g_pTblSTBs[ wType ] )
		return false;

	if ( wItemNO >= g_pTblSTBs[ wType ]->m_nDataCnt )
		return false;

	return true;
}
//-------------------------------------------------------------------------------------------------
//bool tagBaseITEM::IsValidITEM ()
//{
//	if ( 0 == this->GetTYPE() || 0 == this->GetItemNO() )
//		return false;
//
//	if ( this->GetTYPE() > ITEM_TYPE_RIDE_PART )
//		return false;
//	if ( NULL == g_pTblSTBs[ this->GetTYPE() ] )
//		return false;
//
//	if ( this->GetItemNO() >= g_pTblSTBs[ this->GetTYPE() ]->m_nDataCnt )
//		return false;
//
//	return true;
//}

//-------------------------------------------------------------------------------------------------
bool tagBaseITEM::IsEnableKEEPING ()
{
	if ( this->IsEmpty() )
		return false;

	// 0인것만 은행에 보관 가능함... 
	// 계정으로 공유되는 창고기 때문에 다른 케릭으로 아이템이 전이 안되도록...
	return ( 0 == ITEM_USE_RESTRICTION( this->m_cType, this->m_nItemNo ) ||
		     (ITEM_ENABLE_KEEPING & ITEM_USE_RESTRICTION( this->m_cType, this->m_nItemNo ) ) );
}

//-------------------------------------------------------------------------------------------------
bool tagBaseITEM::IsEnableDROP ()
{
	if ( this->IsEmpty() )
		return false;
	if ( this->m_cType > ITEM_TYPE_RIDE_PART )
		return false;

	if ( ITEM_DONT_DROP_EXCHANGE & ITEM_USE_RESTRICTION( this->m_cType, this->m_nItemNo ) ) {
		// 드롭 불가 아이템.
		return false;
	}

	return true;
}

//-------------------------------------------------------------------------------------------------
bool tagBaseITEM::IsEnableSELL ()
{
	if ( this->IsEmpty() )
		return false;
	if ( this->m_cType > ITEM_TYPE_RIDE_PART )
		return false;

	if ( ITEM_DONT_SELL & ITEM_USE_RESTRICTION( this->m_cType, this->m_nItemNo ) ) {
		// 판매 불가 아이템.
		return false;
	}

// 아이템 유료화 부분에서 들어가야 체크사용된 아이템을 팔수 없다..
#ifdef __KRRE_NEW_AVATAR_DB

#endif
	

	return true;
}

//-------------------------------------------------------------------------------------------------
bool tagBaseITEM::IsTwoHands ()
{
	if ( m_cType == ITEM_TYPE_WEAPON ) {
		short nWeaponTYPE = WEAPON_TYPE(m_nItemNo);
		if ( nWeaponTYPE >= 221 && nWeaponTYPE <= 255 ) {
			// 양손검 : 221 ~
			// 원거리 : 231 ~
			// 마법무기 : 241 ~
			// 카트르계열 : 251 ~
			if ( nWeaponTYPE != 242 ) {
				return true;
			} // else ( 242 한손 마법 도구 )
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
t_eSHOT tagBaseITEM::GetShotTYPE ()
{
	/// 맨손 일때도 호출된다.
	if ( ITEM_TYPE_WEAPON == m_cType ) {
		switch( WEAPON_TYPE( m_nItemNo ) ) {

			case WEAPON_ITEM_USE_ARROW   :	
			case WEAPON_ITEM_USE_ARROW2  :	return SHOT_TYPE_ARROW;
			case WEAPON_ITEM_USE_BULLET  :
			case WEAPON_ITEM_USE_BULLET2 :	return SHOT_TYPE_BULLET;
			case WEAPON_ITEM_USE_THROW   :	return SHOT_TYPE_THROW;			
		}
	}

	return MAX_SHOT_TYPE;
}

//-------------------------------------------------------------------------------------------------
const t_EquipINDEX s_ItemType2EquipPos[] = 
{
	MAX_EQUIP_IDX,

	EQUIP_IDX_FACE_ITEM,	// ITEM_TYPE_FACE_ITEM = 1,		// 1	LIST_FACEITEM.stb	얼굴 장식	
	EQUIP_IDX_HELMET,		// ITEM_TYPE_HELMET,			// 2	LIST_CAP.stb
	EQUIP_IDX_ARMOR,		// ITEM_TYPE_ARMOR,				// 3	LIST_BODY.stb
	EQUIP_IDX_GAUNTLET,		// ITEM_TYPE_GAUNTLET,			// 4	LIST_ARMS.stb
	EQUIP_IDX_BOOTS,		// ITEM_TYPE_BOOTS,				// 5	LIST_FOOT.stb
	EQUIP_IDX_KNAPSACK,		// ITEM_TYPE_KNAPSACK,			// 6	LIST_BACK.stb
	EQUIP_IDX_NECKLACE,		// ITEM_TYPE_JEWEL,				// 7	LIST_JEWEL.stb		장신구 : 목걸이 반지

	EQUIP_IDX_WEAPON_R,		// ITEM_TYPE_WEAPON,			// 8	LIST_WEAPON.stb		무기
	EQUIP_IDX_WEAPON_L,		// ITEM_TYPE_SUBWPN,			// 9	LIST_SUBWPN.stb
} ;
t_EquipINDEX tagBaseITEM::GetEquipPOS ()
{
	if ( m_cType >= ITEM_TYPE_FACE_ITEM && m_cType <= ITEM_TYPE_SUBWPN ) {
		t_EquipINDEX eIDX = s_ItemType2EquipPos[ m_cType ];

		if ( eIDX == EQUIP_IDX_NECKLACE ) {
			switch( ITEM_TYPE(m_cType, m_nItemNo) ) 
			{
				case JEWEL_ITEM_RING	:	return EQUIP_IDX_RING;
				case JEWEL_ITEM_NECKLACE:	return EQUIP_IDX_NECKLACE;
				case JEWEL_ITEM_EARRING :	return EQUIP_IDX_EARRING;
			}
			return MAX_EQUIP_IDX;
		}
		return eIDX;
	}

	return MAX_EQUIP_IDX;
}

BYTE tagBaseITEM::GetMileageType()
{
	
	if(GetTYPE()<ITEM_TYPE_FACE_ITEM||GetTYPE()>ITEM_TYPE_RIDE_PART)
	{
		g_LOG.OutputString(0xffff," Error GetMileageType%d\n",GetTYPE());
		return 0;
	}

	if(GetTYPE() ==ITEM_TYPE_QUEST)  //퀘스트 아이템일경우
		return 0;

   return g_pTblSTBs[GetTYPE()]->GetData(GetItemNO(),MileageTypeCol[GetTYPE()]);
}

DWORD tagBaseITEM::GetMileageDate()
{
	
	if(GetTYPE()<ITEM_TYPE_FACE_ITEM||GetTYPE()>ITEM_TYPE_RIDE_PART)
	{
		g_LOG.OutputString(0xffff," Error GetMileageDate%d",GetTYPE());
		return 0;
	}

	if(GetTYPE() ==ITEM_TYPE_QUEST)  //퀘스트 아이템일경우
		return 0;


	return g_pTblSTBs[GetTYPE()]->GetData(GetItemNO(),MileageDateCol[GetTYPE()]);
}

//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER	
/// 2004 / 6 /17 : Client에서는 수량 없는 아이템일경우에는 항상 0 or 1을 리턴하도록 수정
unsigned int   tagBaseITEM::GetQuantity ()
{	
	if( IsEnableDupCNT() )
		return m_iQuantity;

	if( IsEmpty() )
		return 0;

	return 1;
}
#endif


//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER
unsigned short tagBaseITEM::GetModelINDEX ()
{
	switch( m_cType ) 
	{		
		/*
		case ITEM_TYPE_FACE		:	return	FACEITEM_ICON_NO	( m_nItemNo );
		case ITEM_TYPE_HELMET	:	return	HELMET_ICON_NO		( m_nItemNo );
		case ITEM_TYPE_ARMOR	:	return	ARMOR_ICON_NO		( m_nItemNo );
		case ITEM_TYPE_GAUNTLET	:	return	GAUNTLET_ICON_NO	( m_nItemNo );
		case ITEM_TYPE_BOOTS	:	return	BOOTS_ICON_NO		( m_nItemNo );
		case ITEM_TYPE_KNAPSACK	:	return	BACKITEM_ICON_NO	( m_nItemNo );
		case ITEM_TYPE_JEWEL	:	return	JEWEL_ICON_NO		( m_nItemNo );

		case ITEM_TYPE_WEAPON	:	return	WEAPON_ICON_NO		( m_nItemNo );
		case ITEM_TYPE_SUBWPN	:	return	SUBWPN_ICON_NO		( m_nItemNo );

		case ITEM_TYPE_USE		:	return	USEITEM_ICON_NO		( m_nItemNo );

		case ITEM_TYPE_JEM		:	return	GEMITEM_ICON_NO		( m_nItemNo );
		case ITEM_TYPE_NATURAL	:	return	NATURAL_ICON_NO		( m_nItemNo );
		*/
		case ITEM_TYPE_QUEST	:
			return 0;
		case ITEM_TYPE_MONEY	:
			return DROP_TIEM_GOLD;
	}

	if( m_cType == 0 )
		return 0;

	return ITEM_FIELD_MODEL( m_cType, m_nItemNo );	// 없는경우는 대표 박스나 보따리로...
}
#endif

//-------------------------------------------------------------------------------------------------
#ifndef	__SERVER
char* tagBaseITEM::GettingMESSAGE ()
{
	if ( m_cType == ITEM_TYPE_MONEY ) 
	{
		return CStr::Printf(F_STR_GETTING_MONEY, m_iMoney );
	} 
	else if ( IsEnableDupCNT() ) 
	{
		return CStr::Printf(F_STR_GETTING_ITEMS, ITEM_NAME( m_cType, m_nItemNo ), GetQuantity() );
	} 
	else 
	{
		// 장비..
		return CStr::Printf(F_STR_GETTING_ITEM, ITEM_NAME( m_cType, m_nItemNo ) );
	} 

	return NULL;
}
#endif
#ifndef	__SERVER
char* tagBaseITEM::GettingMESSAGE (int iInventoryListNo )
{
	CInventory* pInventory = NULL;
	if ( m_cType == ITEM_TYPE_MONEY ) {
		return CStr::Printf(F_STR_GETTING_MONEY, m_iMoney );
	} 
	else if ( IsEnableDupCNT() ) 
	{
		pInventory = g_pAVATAR->GetInventory();
		tagITEM sItem = pInventory->LST_GetITEM (iInventoryListNo);

		if( sItem.GetTYPE() == 0 )
			sItem.m_iQuantity = 0;

		return CStr::Printf(F_STR_GETTING_ITEMS, ITEM_NAME( m_cType, m_nItemNo ), m_iQuantity - sItem.GetQuantity() );
	} 
	else 
	{
		// 장비..
		return CStr::Printf(F_STR_GETTING_ITEM, ITEM_NAME( m_cType, m_nItemNo ) );
	} 

	return NULL;
}
#endif

#ifndef	__SERVER
char*	tagBaseITEM::GettingQuestMESSAGE()
{
	if( m_cType == ITEM_TYPE_MONEY )
		return CStr::Printf(F_STR_QUEST_GETTING_MONEY, m_iMoney );
	else if(  IsEnableDupCNT() ) 
		return CStr::Printf(F_STR_QUEST_GETTING_ITEMS, ITEM_NAME( m_cType, m_nItemNo ) , m_iQuantity );
	else
		return CStr::Printf(F_STR_QUEST_GETTING_ITEM, ITEM_NAME( m_cType, m_nItemNo ) );
	return NULL;
}
#endif

#ifndef	__SERVER
char*	tagBaseITEM::SubtractQuestMESSAGE()
{
	if( m_cType == ITEM_TYPE_MONEY )
		return CStr::Printf(F_STR_QUEST_SUBTRACT_MONEY, m_iMoney );
	else if( IsEnableDupCNT() )
		return CStr::Printf(F_STR_QUEST_SUBTRACT_ITEMS, ITEM_NAME( m_cType, m_nItemNo ), m_iQuantity );
	else
		return CStr::Printf(F_STR_QUEST_SUBTRACT_ITEM, ITEM_NAME( m_cType, m_nItemNo ) );

	return NULL;
}
#endif

//-------------------------------------------------------------------------------------------------
///소모탄아이템의 ShotType을 얻기
#ifndef	__SERVER
t_eSHOT tagBaseITEM::GetBulletType()
{
	if( IsEmpty() || m_cType != ITEM_TYPE_NATURAL )
		return MAX_SHOT_TYPE;

	return GetNaturalBulletType( m_nItemNo );
}
#endif

#ifndef	__SERVER
t_eSHOT tagBaseITEM::GetNaturalBulletType( int iItemNo )
{

	t_eSHOT retType = MAX_SHOT_TYPE;

	if( iItemNo == 0)
		return retType;

	switch( ITEM_TYPE( ITEM_TYPE_NATURAL, iItemNo ) )
	{
	case 431:///화살
		retType = SHOT_TYPE_ARROW;
		break;
	case 432:///총알
		retType = SHOT_TYPE_BULLET;
		break;
	case 433:///투척
	case 421:
	case 422:
	case 423:
		retType = SHOT_TYPE_THROW;
		break;
	default:
		break;
	}
	return retType;
}
#endif
#ifndef	__SERVER
int	 tagBaseITEM::GetHitRate()
{
	if( m_cType != ITEM_TYPE_WEAPON )
		return 0;

	return ITEM_QUALITY( m_cType, m_nItemNo ) * 0.6 + m_cDurability * 0.8 - 15;
}

int tagBaseITEM::GetAvoidRate()
{
	int iRet = 0;
	switch( m_cType )
	{
	case ITEM_TYPE_HELMET:
	case ITEM_TYPE_ARMOR:
	case ITEM_TYPE_GAUNTLET:
	case ITEM_TYPE_BOOTS:
	case ITEM_TYPE_KNAPSACK:
	case ITEM_TYPE_SUBWPN:
		iRet = m_cDurability * 0.3;
		break;
	default:
		iRet = 0;
	}
	return iRet;
}
#endif
#ifndef	__SERVER
bool tagBaseITEM::IsEqual( int iType, int iItemNo )
{
	if( m_cType == iType && m_nItemNo == iItemNo )
		return true;

	return false;
}
#endif

#ifndef	__SERVER
bool tagBaseITEM::IsEnableExchange ()
{
	if ( this->IsEmpty() )
		return false;

	if ( ITEM_DONT_DROP_EXCHANGE & ITEM_USE_RESTRICTION( this->m_cType, this->m_nItemNo ) ) {
		// 드롭 ,교환 불가 아이템.
		return false;
	}

	return true;
}
#endif

#ifndef	__SERVER
bool tagBaseITEM::IsEnableSeparate ()
{
	if( IsEmpty() )
		return false;

	if( GetTYPE() == ITEM_TYPE_GEM )///보석은 분리가 불가능하다
		return false;

	if( HasSocket() && GetGemNO() > 300 )///재밍된 경우
		return true;
	
	if( ITEM_PRODUCT_IDX( GetTYPE(), GetItemNO() ) )
		return true;

	return false;
}
#endif

#ifndef	__SERVER
bool tagBaseITEM::IsEnableUpgrade ()
{
	if( IsEmpty() )
		return false;

	int iClass = ITEM_TYPE( GetTYPE(), GetItemNO() );

	if( GetGrade() < 9 )
	{
		if( iClass >= 211 && iClass <= 280 )///무기 - 공격력, 명중력상승
			return true;

		if( iClass >= 121 && iClass <= 153 )///방어구 - 방어력, 항마력, 회피력상승
			return true;

		if( iClass == 161 || iClass == 163 )///등장비( 가방 제외)
			return true;

		if( iClass == 261 )///방패 - 방어력,항마력,회피력상승
			return true;
	}
	return false;
}
#endif

#ifndef	__SERVER
bool tagBaseITEM::HasLife()
{
	if( IsEquipITEM() || GetTYPE() == ITEM_TYPE_RIDE_PART )
		return true;

	return false;
}
#endif
#ifndef	__SERVER
bool tagBaseITEM::HasDurability()
{
	if( IsEquipITEM() )
		return true;

	return false;
}
#endif


#ifndef	__SERVER
bool tagBaseITEM::IsEnableAppraisal ()
{
	if( IsEmpty() ) 
		return false;

	bool b = IsEquipITEM();
	if( IsEquipITEM() && m_nGEM_OP && m_nGEM_OP <= 300 && m_bIsAppraisal == 0 )
		return true;

	return false;
}
#endif


#ifndef	__SERVER
int	tagBaseITEM::GetUpgradeCost()
{
	if( !IsEnableUpgrade() )
		return 0;
	return (GetGrade() + 2 ) * (GetGrade() + 5 ) * ( ITEM_QUALITY( GetTYPE(), GetItemNO() ) + 20 ) * 0.6;
//	return ITEM_QUALITY( GetTYPE(), GetItemNO() ) * 10 + GetGrade() * 100 ;
}

int tagBaseITEM::GetSeparateCost()
{
	if( !IsEnableSeparate() )
		return 0;

	return ITEM_QUALITY( GetTYPE(), GetItemNO() ) * 10 + 20;
}

int tagBaseITEM::GetAppraisalCost()
{
	if( !IsEnableAppraisal () )
		return 0;

	__int64 i64BasePrice = ITEM_BASE_PRICE( GetTYPE(), GetItemNO() );
	return ( i64BasePrice + 10000 ) * ( GetDurability() + 50 ) / 10000;
}

const	char*	tagBaseITEM::GetName()
{
	if( IsEmpty() )
		return "NULL";

	if( m_cType == ITEM_TYPE_MONEY )
		return STR_MONEY;
	else if( m_cType >= ITEM_TYPE_FACE_ITEM && m_cType < ITEM_TYPE_MONEY )
		return ITEM_NAME( m_cType, m_nItemNo );

	return "NULL";
}
#endif





//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
// 2007년 1월 1일 0시 0분 0초부터 현재까지의 시간을 초단위로
void tagBaseITEM::set_PickOutTime(WORD period)
{
	time_t current_t;
	time( &current_t );	// 현재시간

	dwPickOutTime = (DWORD)(current_t - get_basicTime());
	wPeriod = period;
}

// 1970년 1월 1일 0시 0분 0초부터 2007년 01월 01일 0시 0분 0초까지의 시간을 초단위로
time_t tagBaseITEM::get_basicTime()
{
	time_t basic_t;
	struct tm t;

	// 2007년 01월 01일 0시 0분 0초 기준
	t.tm_year = 2007 - 1900; t.tm_mon = 1 - 1; t.tm_mday = 1; t.tm_hour = 0; t.tm_min = 0; t.tm_sec = 0;
	basic_t = mktime( &t);	

	return basic_t;
}

// 사용기간이 만료됐는가 체크 - true : 기간만료
bool tagBaseITEM::check_timeExpired()
{
	return false;
}

// 유효사용기간중에 서버다운이 있었으면 다운된 시간만큼 만료시간을 보정해 준다.
time_t tagBaseITEM::calc_endTime(time_t start_t, time_t end_t, time_t down_start_t, time_t down_end_t, int nValue)
{
	time_t new_end_t = end_t;
	if(start_t < down_start_t && down_start_t < end_t)		// 유효사용기간중에 서버다운이 있었는가?
	{
		new_end_t = end_t + (down_end_t - down_start_t);	// 서버다운된 시간만큼 만료시간을 늘려준다.

		// 손해본 시간의 몇배를 보상해 준다.
		if(end_t < down_end_t)	
			new_end_t = new_end_t + ((end_t - down_start_t) * nValue);
		else
			new_end_t = new_end_t + ((down_end_t - down_start_t) * nValue);
	}

	return new_end_t;	// 보정된 만료시간
}


#endif	//  __ITEM_TIME_LIMIT
//-------------------------------------



#ifdef __SERVER

BOOL MileageItem::SetDate()
{
	switch(GetMileageType())
	{
	case EN_DAYRATE:               //정량제 아이템 (날짜단위),
		return SetDayRate();
		
	case EN_METERRATE:          //종량제 아이템,
		return SetMeterRate();

	default:
		return FALSE;
	}

	return FALSE;
}

BOOL MileageItem::SetDayRate()				//정량제 아이템 마일리지 데이트 셋팅
{
	if(0!=GetDate())
		return FALSE;
	
	DWORD dwMileage = GetMileageDate()*DEF_DAY_CONVERT_SEC;

	SYSTEMTIME SysTime;
	::GetLocalTime(&SysTime);
	SysTime.wHour = 23; SysTime.wMonth = 59; SysTime.wSecond =0; SysTime.wMilliseconds =0;
	DWORD dwDate = classTIME::GetAbsSecond(SysTime);
	GetDate() = dwMileage+dwDate;                                         //정량제아이템 만료날짜 셋팅..
	return TRUE;
	
}

BOOL MileageItem::SetMeterRate()          // 종량제 아이템 마일리지 셋팅..
{
	if(0!=GetDate())
		return FALSE;
	DWORD dwMileage = GetMileageDate();
	GetMeter() = dwMileage;															//종량제아이템 만료분(MIN)셋팅..
	return TRUE;
}



BOOL MileageItem::ChkPassMileage(DWORD dwCurTime)  //마일리지아이템 날짜및 종량체크
{
		if(!GetMileageType())
			return TRUE;

		if(GetMileageType() == EN_DAYRATE&&GetDate())  //정량제 타입이고.. 착용한적이 있으면...
		{
			if(!GetDate()<=dwCurTime)																								//검사한다..
				return FALSE;
		}
		else if(GetMileageType() == EN_METERRATE)
		{
			if(!GetMeter())
				return FALSE;
		}
		return TRUE;
}

BOOL MileageItem::SubMeter(BYTE btSubMeter)
{
	 if(GetMileageType() != EN_METERRATE)
		 return FALSE;

	if(GetMeter()< btSubMeter)
		return FALSE;

	GetMeter()-=btSubMeter;

	return TRUE;
}

void   MileageItem::Init_FirstPickOut()                                //첨으로 마일리지 몰에서 가져올때 아이템 셋팅해주는 함수..
{
	if(GetMileageType() != EN_METERRATE)
		return;

	SetMeterRate();                                     //종량제 아이템일경우 몰에서 찾을때 마일리지 테이트를 셋팅해준다..
}

void MileageItem::Equip()
{
	if(GetMileageType() == EN_METERRATE&&GetDate()==0)
	{
		SetDayRate();
	}

//
}

#endif