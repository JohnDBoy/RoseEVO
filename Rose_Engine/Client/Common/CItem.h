
#ifndef	__CITEM_H
#define	__CITEM_H
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------
/*
돈(MONEY)	지정번호 : 40	(0 ~ 999,999)
아이템 종류(ITEM_CLASS)		(1 ~ 20)		: 5  bit  0~31		
아아템 번호(ITEM_ID)		(0 ~ 999)		: 10 bit  0~1023	

재밍 번호1(JAMMING1)		(0~120)			: 7  bit  0~127
재밍 번호2(JAMMING2)		(0~120)         : 7  bit  0~127
재밍 번호3(JAMMING3)		(0~120)         : 7  bit  0~127

강화 등급(RESMELT)			(0~9)			: 4  bit  0~15		장비 아이템일 경우만..
품질(QUALITY)				(0~120)			: 7  bit  0~127		장비 아이템일 경우만..
개수(QUANTITY)				(1~999)			: 10 bit  0~1023	소모, 기타 아이템일 경우

  장비 : 5 + 10 + 21 + 11 ==> 15+33 : 48   6 bytes
  기타 : 5 + 10 + 10      ==> 15+10
  돈   : 5 + 10 + xx
*/


#include <time.h>

#define	MAX_ITEM_LIFE			1000
#define	MAX_DUP_ITEM_QUANTITY	999

#ifdef __ITEM_MAX

#pragma pack (push, 1)
struct tagPartITEM {
	unsigned int	m_nItemNo		: 18;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
	unsigned int	m_nGEM_OP		: 9;	// 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
	unsigned int	m_bHasSocket	: 1;	// 0~1		보석 소켓 여부
	unsigned int	m_cGrade	    : 4;	// 0~15		등급						(0~9)
} ;

#else

#pragma pack (push, 1)
struct tagPartITEM {
	unsigned int	m_nItemNo		: 10;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
	unsigned int	m_nGEM_OP		: 9;	// 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
	unsigned int	m_bHasSocket	: 1;	// 0~1		보석 소켓 여부
	unsigned int	m_cGrade	    : 4;	// 0~15		등급						(0~9)
} ;

#endif



#ifndef	__SERVER
#define	tagITEM		tagBaseITEM
#endif

int getItemNo(int iFullItemNo);
int getItemType(int iFullItemNo);

int setItemFullNo( int iItemType, int iItemNo );

// 총 48 bits, 6 bytes
struct tagBaseITEM {
	
#ifdef __ITEM_MAX

	union {
		// 장비 아이템 구조
		struct {	
			// LSB ::
			// 아래 둘중 하나는 비트 늘려도 됨.
			unsigned int	m_cType			: 5;	// 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)			
			unsigned int	m_nItemNo		: 26;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
			unsigned int	m_bCreated		: 1;	// 0~1		제조된 아이템인가 ?

			unsigned int	m_nGEM_OP		: 9;	// 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	// 0~127	내구도

			unsigned int	m_nLife			: 10;	// 0~1023	수명
			unsigned int	m_bHasSocket	: 1;	// 0~1		보석 소켓 여부
			unsigned int	m_bIsAppraisal	: 1;	// 0~1		옵션 검증 여부
			unsigned int	m_cGrade	    : 4;	// 0~15		등급						(0~9)

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		// 소모, 기타 아이템 구조
		struct {
			unsigned int	m_cType_1		: 5;	// 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo_1		: 26;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiQuantity	: 32;	// 갯수(돈)
		} ;

		// 돈 아이템 구조
		struct {
			unsigned int	m_cType_2		: 5;	// 0~31
			unsigned int	m_nReserved1	: 26;
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiMoney		: 32;
		} ;

		struct {
			unsigned int	m_wHeader		: 32;	//서버는 m_dwHeader인데 고칠곳이 많아서 이름은 수정하지 않았다.
			unsigned int	m_dwBody		: 32;
		} ;

		struct {
			unsigned int	m_dwLSB;
			unsigned int	m_dwMSB;
		} ;		

		unsigned int	m_dwITEM;
	} ;


#ifdef __ITEM_TIME_LIMMIT

	struct {
		DWORD dwPickOutTime;
		WORD  wPeriod;
	};	

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

	//적용시간
	tm tagBaseITEM::get_startTime()
	{		
		time_t long_time = (dwPickOutTime) + get_basicTime();
		struct tm l = *localtime( &long_time );
		l.tm_year += 1900;
		l.tm_mon += 1;
		return l;
	}
	
	//소멸시간
	tm tagBaseITEM::get_endTime()
	{		
		time_t long_time = (dwPickOutTime) + get_basicTime() + (wPeriod) * 3600 ;
		
		struct tm l = *localtime( &long_time );
		//struct tm l = *gmtime( &long_time );
		
		l.tm_year += 1900;
		l.tm_mon += 1;
		if( l.tm_min )
		{
            l.tm_hour += 1;            
			l.tm_min = 0;
			l.tm_sec = 0;
		}		
		return l;
	}

#endif


#else

	union {
		// 장비 아이템 구조
		struct {	
			// LSB ::
			// 아래 둘중 하나는 비트 늘려도 됨.
			unsigned short	m_cType			: 5;	// 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)			
			unsigned short	m_nItemNo		: 10;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)
			unsigned short	m_bCreated		: 1;	// 0~1		제조된 아이템인가 ?

			unsigned int	m_nGEM_OP		: 9;	// 0~512	보석번호(m_bHasSocket==1) 또는 옵션 번호(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	// 0~127	내구도

			unsigned int	m_nLife			: 10;	// 0~1023	수명
			unsigned int	m_bHasSocket	: 1;	// 0~1		보석 소켓 여부
			unsigned int	m_bIsAppraisal	: 1;	// 0~1		옵션 검증 여부
			unsigned int	m_cGrade	    : 4;	// 0~15		등급						(0~9)

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		// 소모, 기타 아이템 구조
		struct {
			unsigned short	m_cType_1		: 5;	// 0~31		아이템 종류(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo_1		: 10;	// 0~1023	아아템 번호(ITEM_ID)		(0 ~ 999)

			unsigned int	m_uiQuantity	: 32;	// 갯수(돈)
		} ;

		// 돈 아이템 구조
		struct {
			unsigned short	m_cType_2		: 5;	// 0~31
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
#endif


	void		   Init(int iItem, unsigned int uiQuantity=1);

#ifdef __ITEM_MAX
	void			Clear ()				
	{
		m_dwLSB=m_dwMSB=0;
#ifdef __ITEM_TIME_LIMMIT
		dwPickOutTime = wPeriod = 0;
#endif
	}
	unsigned int	GetItemNO ()			{	return	m_nItemNo;				}
	bool			IsEmpty ()				{	return (0==m_wHeader);			}
	unsigned short	GetHEADER ()			{	return (m_wHeader & 0x7fffffff);	}	// m_bCreated :: 헤더 비교시 제조비트 없이...
#else
	void			Clear ()				{	m_dwLSB=m_wMSB=0;				}
	unsigned short	GetItemNO ()			{	return	m_nItemNo;				}
	bool			IsEmpty ()				{	return (0==m_wHeader);			}
	unsigned short	GetHEADER ()			{	return (m_wHeader & 0x7fff);	}	// m_bCreated :: 헤더 비교시 제조비트 없이...

#endif
	

	unsigned short GetTYPE ()			{	return	m_cType;				}
	
	

	unsigned short GetGrade ()			{	return	m_cGrade;		}
	unsigned short GetOption ()			{	return  m_nGEM_OP;		}
	unsigned short GetGemNO ()			{	return  m_nGEM_OP;		}

	unsigned short GetLife()			{	return  m_nLife;		}
	unsigned short GetDurability()		{	return  m_cDurability;	}
	unsigned int   GetMoney ()			{	return	m_uiMoney;		}

	

	bool IsAppraisal()					{	return (0!=m_bIsAppraisal);		}
	bool HasSocket()					{	return (0!=m_bHasSocket);		}

	bool IsEnableDROP ();					// 버리기가 가능한 아이템인가 ?
	bool IsEnableSELL ();					// 팔기가 가능한 아이템인가 ?
	bool IsEnableKEEPING ();				// 은행에 보관 가능한 아이템인가 ?

	bool IsValidITEM ();

	static bool IsEnableDupCNT( unsigned short cType )		
	{	
		// 중복 갯수적용 아이템이냐???
		return (cType>=ITEM_TYPE_USE && cType<ITEM_TYPE_RIDE_PART);
	}
	bool IsEnableDupCNT()				{	return IsEnableDupCNT(m_cType);	}
	bool IsCreated()					{	return (1==m_bCreated);			}
	bool IsEquipITEM()					{	return ( m_cType && m_cType < ITEM_TYPE_USE );					}	// 장착 아이템인가?
	bool IsEtcITEM()					{	return ( m_cType>ITEM_TYPE_USE && m_cType<ITEM_TYPE_QUEST);		}

	bool IsTwoHands ();

	t_eSHOT			GetShotTYPE();
	t_EquipINDEX	GetEquipPOS();
#ifdef	__SERVER
	unsigned int	GetQuantity ()		{	return	m_uiQuantity;			}
#else
	unsigned int	GetQuantity ();
	short			Subtract( tagITEM &sITEM );		// 주어진 아이템 만큼 덜어 내고 빠진결과는 sITEM에 들어 있다.
	void			SubtractOnly (tagITEM &sITEM);	// 주어진 아이템 만큼 덜어 낸다.

	bool			IsEnableAppraisal ();					///감정가능한 아이템인가?
	bool			IsEnableExchange ();					// 버리기가 가능한 아이템인가 ?
	bool			IsEnableSeparate ();
	bool			IsEnableUpgrade ();
	bool			HasLife();
	bool			HasDurability();
	unsigned short	GetModelINDEX ();
	char*			GettingMESSAGE (int iInventoryListNo);
	char*			GettingMESSAGE ();
	char*			GettingMESSAGE_Party (const char * partyName_);
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
struct tagITEM : public tagBaseITEM {
	union {
		DWORD	m_dwSN[2];
		__int64	m_iSN;
	} ;

	// 현재 아이템에서 주어진 아이템 만큼 뺀후, 빠진 무게를 리턴한다.
	short	Subtract( tagITEM &sITEM );		// 주어진 아이템 만큼 덜어 내고 빠진결과는 sITEM에 들어 있다.
	void	SubtractOnly (tagITEM &sITEM);	// 주어진 아이템 만큼 덜어 낸다.

	bool SubQuantity ()		{	
		if ( GetQuantity() > 0 ) {
			--m_uiQuantity;
			return true; 
		}
		return false;
	}
	bool SubQuantity(unsigned int uiCnt)
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

#ifdef __ITEM_MAX
	void operator=(tagBaseITEM &rBASE)
	{
		m_dwLSB  = rBASE.m_dwLSB;
		m_dwMSB = rBASE.m_dwMSB;
		m_iSN   = 0;
	}
#else
	void operator=(tagBaseITEM &rBASE)
	{
		m_wLSB  = rBASE.m_wLSB;
		m_dwMSB = rBASE.m_dwMSB;
		m_iSN   = 0;
	}
#endif
	
#endif
} ;
#endif

//서버에서는 클라이언트랑통신을 위해 시리얼키를 제외하고 구조체를 구성..
//클라이언트에서는 그대로 CMileageInv에서 사용하면됨...
struct Net_MileageItem:public tagBaseITEM
{
	DWORD m_dwDate;
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


};

#else

typedef  Net_MileageItem MileageItem;

#endif

#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif