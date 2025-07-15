/**
 * \ingroup SHO_GS
 * \file	CItem.h
 * \brief	개개의 아이템 데이타, 클라이언트는 6바이트, 서버는 6+8바이트(아이템고유ID)
 * <pre>
 * 돈(MONEY)	지정번호 : 40	(0 ~ 999,999)
 * 아이템 종류(ITEM_CLASS)		(1 ~ 20)		: 5  bit  0~31		
 * 아아템 번호(ITEM_ID)		(0 ~ 999)		: 10 bit  0~1023	
 * 
 * 재밍 번호1(JAMMING1)		(0~120)			: 7  bit  0~127
 * 재밍 번호2(JAMMING2)		(0~120)         : 7  bit  0~127
 * 재밍 번호3(JAMMING3)		(0~120)         : 7  bit  0~127
 * 
 * 강화 등급(RESMELT)			(0~9)			: 4  bit  0~15		장비 아이템일 경우만..
 * 품질(QUALITY)				(0~120)			: 7  bit  0~127		장비 아이템일 경우만..
 * 개수(QUANTITY)				(1~999)			: 10 bit  0~1023	소모, 기타 아이템일 경우
 * 
 *   장비 : 5 + 10 + 21 + 11 ==> 15+33 : 48   6 bytes
 *   기타 : 5 + 10 + 10      ==> 15+10
 *   돈   : 5 + 10 + xx
 * </pre>
 */
#ifndef	__CITEM_H
#define	__CITEM_H

#include  <Windows.h>
#include "DataTYPE.h"

//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
#include <time.h>

struct _DOWN_TIME {
	time_t down_start_t;
	time_t down_end_t;
	int nValue;
};
#include <map>
//extern std::map<int, _DOWN_TIME> g_mapServerDownTime;
#endif
//-------------------------------------

#define __MAX_ITEM_999_EXT //정의			// 2006.07.06/김대성/추가 - 아이템 999 제한 확장

#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#define	MAX_ITEM_LIFE			1000		/// 최대 아이템 수명
#define	MAX_DUP_ITEM_QUANTITY	999			/// 최대 중첩 가능한 아이템 갯수

#define DEF_DAY_CONVERT_SEC 86400
#define DEF_MIN_CONVERT_SEC 60

enum EN_MILEAGE_TYPE
{
	EN_FREE =0,								  //일반아이템..,
	EN_DAYRATE,               //정량제 아이템 (날짜단위),
	EN_METERRATE ,          //종량제 아이템,
	EN_DURABILITY ,         //내구도 아이템,
	EN_CONSUME ,              //소모성 아이템.
	EN_MIELAGE_TYPE,					 //마일리지 아이템 끝,
};


static int  MileageTypeCol[ITEM_TYPE_RIDE_PART+1]  =
{
	-1,
	48,    // 1	LIST_FACEITEM.stb	얼굴 장식	
	48,	 	// 2	LIST_CAP.stb
	48,	 	// 3	LIST_BODY.stb
	48,	 		// 4	LIST_ARMS.stb
	48,	 // 5	LIST_FOOT.stb
	48,	 		// 6	LIST_BACK.stb
	48,      //7 LIST_JEWEL.stb
	48,	 	// 8	LIST_WEAPON.stb		무기
	48,	 	// 9	LIST_SUBWPN.stb
	27,		//10	LIST_USEITEM.stb	소모			
	22,	// 11	LIST_JEMITEM.stb	기타 : 보석
	18,				// 12	LIST_NATURAL.stb
	-1,				// 13	LIST_QUESTITEM.stb
	72
};

static BYTE MileageDateCol[ITEM_TYPE_RIDE_PART+1] = 
{
	-1,
	49,    // 1	LIST_FACEITEM.stb	얼굴 장식	
	49,	 	// 2	LIST_CAP.stb
	49,	 	// 3	LIST_BODY.stb
	49,	 		// 4	LIST_ARMS.stb
	49,	 // 5	LIST_FOOT.stb
	49,	 		// 6	LIST_BACK.stb
	49,      //7 LIST_JEWEL.stb
	49,	 	// 8	LIST_WEAPON.stb		무기
	49,	 	// 9	LIST_SUBWPN.stb
	28,	 //	10 LIST_USEITEM.stb	소모		
	23,	// 11	LIST_JEMITEM.stb	기타 : 보석
	19,				// 12	LIST_NATURAL.stb
	-1,				// 13	LIST_QUESTITEM.stb
	73
};



#pragma pack (push, 1)

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	struct tagPartITEM 
	{
		unsigned int	m_nItemNo		: 10;	///< 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
		unsigned int	m_nGEM_OP		: 9;	///< 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
		unsigned int	m_bHasSocket	: 1;	///< 0~1		보석 소켓 여부
		unsigned int	m_cGrade	    : 4;	///< 0~15		등급						(0~9)

	} ;
#else	//------------------------------------- #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	struct tagPartITEM 
	{
		unsigned int	m_nItemNo		: 18;	///< 0~262143	아아템 번호(ITEM_ID)		(0 ~ 200000)
		unsigned int	m_nGEM_OP		: 9;	///< 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
		unsigned int	m_bHasSocket	: 1;	///< 0~1		보석 소켓 여부
		unsigned int	m_cGrade	    : 4;	///< 0~15		등급						(0~9)
	} ;
#endif	// #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
//-------------------------------------


#ifdef	__CLIENT
#define	tagITEM		tagBaseITEM
#endif


// 총 48 bits, 6 bytes
// 총 48 bits, 8 bytes
struct tagBaseITEM
{
//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	union 
	{
		/// 장비 아이템 구조
		struct
		{	
			// LSB ::
			// 아래 둘중 하나는 비트 늘려도 됨.
			unsigned short	m_cType			: 5;	///< 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo		: 10;	///< 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
			unsigned short	m_bCreated		: 1;	///< 0~1		제조된 아이템인가 ?

			unsigned int	m_nGEM_OP		: 9;	///< 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	///< 0~127	내구도

			unsigned int	m_nLife			: 10;	///< 0~1023	수명
			unsigned int	m_bHasSocket	: 1;	///< 0~1		보석 소켓 여부
			unsigned int	m_bIsAppraisal	: 1;	///< 0~1		옵션 검증 여부
			unsigned int	m_cGrade	    : 4;	///< 0~15		등급						(0~9)

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		/// 소모, 기타 아이템 구조
		struct {
			unsigned short	m_cType_1		: 5;	///< 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo_1		: 10;	///< 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
			unsigned int	m_uiQuantity	: 32;	///< 갯수(돈)
		} ;

		/// 돈 아이템 구조
		struct {
			unsigned short	m_cType_2		: 5;	///< 0~31
			unsigned short	m_nReserved1	: 11;
			unsigned int	m_uiMoney		: 32;
		} ;

		struct {
			unsigned short	m_wHeader		: 16;
			unsigned int	m_dwBody		: 32;
		} ;

		struct {
			DWORD	m_dwLSB;
			WORD	m_wMSB;
		} ;

		struct {
			WORD	m_wLSB;
			DWORD	m_dwMSB;
		} ;

		DWORD	m_dwITEM;

	} ;
#else	//-------------------------------------  #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	union 
	{
		/// 장비 아이템 구조
		struct
		{	
			// LSB ::
			// 아래 둘중 하나는 비트 늘려도 됨.
			unsigned int	m_cType			: 5;	///< 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo		: 26;	///< 0~4134303	아아템 번호(ITEM_ID)		(0 ~ 200000)
													/// tagPartITEM의 m_nItemNo와 일치 시키기 위해 18비트만 사용
			unsigned int	m_bCreated		: 1;	///< 0~1		제조된 아이템인가 ?

			unsigned int	m_nGEM_OP		: 9;	///< 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	///< 0~127	내구도

			unsigned int	m_nLife			: 10;	///< 0~1023	수명
			unsigned int	m_bHasSocket	: 1;	///< 0~1		보석 소켓 여부
			unsigned int	m_bIsAppraisal	: 1;	///< 0~1		옵션 검증 여부
			unsigned int	m_cGrade	    : 4;	///< 0~15		등급						(0~9)

			// 32 + 16 + 16 => 64
			// MSB ::
		} ;

		/// 소모, 기타 아이템 구조
		struct {
			unsigned int	m_cType_1		: 5;	///< 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo_1		: 26;	///< 0~4134303	아아템 번호(ITEM_ID)		(0 ~ 200000)
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiQuantity	: 32;	///< 갯수(돈)
		} ;

		/// 돈 아이템 구조
		struct {
			unsigned int	m_cType_2		: 5;	///< 0~31
			unsigned int	m_nReserved1	: 26;	
			unsigned int    __dummy_1       : 1;

			unsigned int	m_uiMoney		: 32;
		} ;

		struct {
			unsigned int	m_dwHeader		: 32;
			unsigned int	m_dwBody		: 32;
		} ;

		struct {
			unsigned int	m_dwLSB;
			unsigned int	m_dwMSB;
		};

		unsigned int	m_dwITEM;
	} ;
#endif	// #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
//-------------------------------------

//-------------------------------------
// 2007.01.10/김대성/추가 - 기간제 아이템
#ifdef __ITEM_TIME_LIMIT
	struct {
		DWORD dwPickOutTime;
		WORD  wPeriod;
	};

	// 2007년 1월 1일 0시 0분 0초부터 현재까지의 시간을 초단위로
	void set_PickOutTime(WORD period);

	// 1970년 1월 1일 0시 0분 0초부터 2007년 01월 01일 0시 0분 0초까지의 시간을 초단위로
	time_t get_basicTime();

	// 사용기간이 만료됐는가 체크 - true : 기간만료
	bool check_timeExpired();

	// 유효사용기간중에 서버다운이 있었으면 다운된 시간만큼 만료시간을 보정해 준다.
	time_t calc_endTime(time_t start_t, time_t end_t, time_t down_start_t, time_t down_end_t, int nValue);
#endif	//  __ITEM_TIME_LIMIT
//-------------------------------------

	void		   Init(int iItem, unsigned int uiQuantity=1);

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	void		   Clear ()				{	m_dwLSB=m_wMSB=0;				}
#else

//-------------------------------------
// 2007.01.10/김대성/수정 - 기간제 아이템
#ifndef __ITEM_TIME_LIMIT
	void		   Clear ()				{	m_dwLSB=m_dwMSB=0;				}
#else
	void		   Clear ()				{	m_dwLSB=m_dwMSB=0;	dwPickOutTime=0; wPeriod=0;	}
#endif
//-------------------------------------

#endif
//-------------------------------------

	unsigned short GetTYPE ()			{	return	m_cType;				}

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	unsigned short GetItemNO ()			{	return	m_nItemNo;				}
	unsigned short GetHEADER ()			{	return (m_wHeader & 0x7fff);	}	/// m_bCreated :: 헤더 비교시 제조비트 없이...
#else
	unsigned int   GetItemNO ()			{	return	m_nItemNo;				}
	unsigned int   GetHEADER ()			{	return (m_dwHeader & 0x7fffffff);	}	/// m_bCreated :: 헤더 비교시 제조비트 없이...
#endif
//-------------------------------------

	unsigned short GetGrade ()			{	return	m_cGrade;		}
	unsigned short GetOption ()			{	return  m_nGEM_OP;		}
	unsigned short GetGemNO ()			{	return  m_nGEM_OP;		}

	unsigned short GetLife()			{	return  m_nLife;		}
	unsigned short GetDurability()		{	return  m_cDurability;	}
	unsigned int   GetMoney ()			{	return	m_uiMoney;		}

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
	bool IsEmpty ()						{	return (0==m_wHeader);	}
#else
	bool IsEmpty ()						{	return (0==m_dwHeader);	}
#endif
//-------------------------------------


	bool IsAppraisal()					{	return (0!=m_bIsAppraisal);		}
	bool HasSocket()					{	return (0!=m_bHasSocket);		}

	bool IsEnableDROP ();					/// 버리기가 가능한 아이템인가 ?
	bool IsEnableSELL ();					/// 팔기가 가능한 아이템인가 ?
	bool IsEnableKEEPING ();				/// 은행에 보관 가능한 아이템인가 ?

//-------------------------------------
/*
	static bool IsValidITEM (WORD wType, WORD wItemNO);
*/
// 2006.07.11/김대성/수정 - 아이템 999 제한 확장
	static bool IsValidITEM (WORD wType, int wItemNO);
//-------------------------------------
	static bool IsValidITEM (tagBaseITEM *pItem)		{	return tagBaseITEM::IsValidITEM( pItem->GetTYPE(), pItem->GetItemNO() );	}
	bool IsValidITEM ()					{	return IsValidITEM( this->GetTYPE(), this->GetItemNO() );	}

	static bool IsEnableDupCNT( unsigned short cType )		
	{	
		// 중복 갯수적용 아이템이냐???
		return (cType>=ITEM_TYPE_USE && cType<ITEM_TYPE_RIDE_PART);
	}
	bool IsEnableDupCNT()				{	return IsEnableDupCNT(m_cType);	}
	bool IsCreated()					{	return (1==m_bCreated);			}
	bool IsEquipITEM()					{	return ( m_cType && m_cType < ITEM_TYPE_USE );					}	/// 장착 아이템인가?
	bool IsEtcITEM()					{	return ( m_cType>ITEM_TYPE_USE && m_cType<ITEM_TYPE_QUEST);		}

	bool IsTwoHands ();

	t_eSHOT			GetShotTYPE();
	t_EquipINDEX	GetEquipPOS();

	 BYTE GetMileageType();
	 DWORD GetMileageDate();
	

#ifdef	__SERVER
	unsigned int	GetQuantity ()		{	return	m_uiQuantity;			}
#else
	unsigned int	GetQuantity ();
	short			Subtract( tagITEM &sITEM );		/// 주어진 아이템 만큼 덜어 내고 빠진결과는 sITEM에 들어 있다.
	void			SubtractOnly (tagITEM &sITEM);	/// 주어진 아이템 만큼 덜어 낸다.

	bool			IsEnableAppraisal ();			/// 감정가능한 아이템인가?
	bool			IsEnableExchange ();			/// 버리기가 가능한 아이템인가 ?
	bool			IsEnableSeparate ();
	bool			IsEnableUpgrade ();
	bool			HasLife();
	bool			HasDurability();
	unsigned short	GetModelINDEX ();
	char*			GettingMESSAGE (int iInventoryListNo);
	char*			GettingMESSAGE ();
	char*			GettingQuestMESSAGE();
	char*			SubtractQuestMESSAGE();
	///소모탄아이템의 ShotType을 얻기
	static t_eSHOT GetNaturalBulletType( int iItemNo );
	t_eSHOT GetBulletType();
	///명중력
	int				GetHitRate();
	int				GetAvoidRate();
	bool			IsEqual( int iType, int iItemNo );
	int				GetUpgradeCost();
	int				GetSeparateCost();
	int				GetAppraisalCost();
	const	char*	GetName();
#endif
} ;



//-------------------------------------------------------------------------------------------------
#ifdef	__SERVER
struct tagITEM : public tagBaseITEM 
{
	union
	{
		DWORD	m_dwSN[2];
		__int64	m_iSN;
	} ;

	/// 현재 아이템에서 주어진 아이템 만큼 뺀후, 빠진 무게를 리턴한다.
	short	Subtract( tagITEM &sITEM );		/// 주어진 아이템 만큼 덜어 내고 빠진결과는 sITEM에 들어 있다.
	void	SubtractOnly (tagITEM &sITEM);	/// 주어진 아이템 만큼 덜어 낸다.
	bool SubQuantity(unsigned int uiCnt =1)
	{
		if ( GetQuantity() > uiCnt ) {
			m_uiQuantity -= uiCnt;
			return true;
		} else
		if ( m_uiQuantity == uiCnt ) {
			this->Clear ();
			return true;
		}
		return false;
	}

	void 	Init(int iItem, unsigned int uiQuantity=1)
	{
		tagBaseITEM::Init(iItem, uiQuantity);
		m_iSN = 0;
	}
	void	Clear ()
	{	
		tagBaseITEM::Clear();
		m_iSN = 0;
	}

#ifndef	__BORLANDC__
	void operator=(tagBaseITEM &rBASE)
	{
//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/김대성/수정 - 아이템 999 제한 확장
		m_wLSB  = rBASE.m_wLSB;
#else
		m_dwLSB  = rBASE.m_dwLSB;
#endif
//-------------------------------------
		m_dwMSB = rBASE.m_dwMSB;
		m_iSN   = 0;

#ifdef __ITEM_TIME_LIMIT
		dwPickOutTime = rBASE.dwPickOutTime;
		wPeriod = rBASE.wPeriod;
#endif

	}
#endif
} ;
#endif

//서버에서는 클라이언트랑통신을 위해 시리얼키를 제외하고 구조체를 구성..
//클라이언트에서는 그대로 CMileageInv에서 사용하면됨...

struct Net_MileageItem:public tagBaseITEM
{
	DWORD m_dwDate;

	DWORD& GetDate() {return m_dwDate; }
};

#ifdef  __SERVER
struct MileageItem:public tagITEM
{
	DWORD m_dwDate;
	void 	Init(int iItem, unsigned int uiQuantity=1)
	{
		tagITEM::Init(iItem, uiQuantity);
		m_dwDate = 0;
	}

	void	Clear ()
	{	
		tagITEM::Clear();
		m_dwDate =0;
	}
	DWORD& GetDate() {return m_dwDate; }
	DWORD& GetMeter() {return m_dwDate;};
	BOOL SetDate();                      //마일리지 아이템 날짜(시간)셋팅..
	BOOL SetDayRate();				//정량제 아이템 마일리지 데이트 셋팅
	BOOL SetMeterRate();          // 종량제 아이템 마일리지 셋팅..
	BOOL ChkPassMileage(DWORD dwCurTime);  //마일리지아이템 날짜및 종량체크
	BOOL SubMeter(BYTE btSubMeter =1);
	void Init_FirstPickOut();                                //첨으로 마일리지 몰에서 가져올때 아이템 셋팅해주는 함수..
	void Equip();														    

};
#else

typedef Net_MileageItem MileageItem;

#endif


#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif