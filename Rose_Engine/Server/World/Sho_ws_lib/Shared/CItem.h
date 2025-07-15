/**
 * \ingroup SHO_GS
 * \file	CItem.h
 * \brief	������ ������ ����Ÿ, Ŭ���̾�Ʈ�� 6����Ʈ, ������ 6+8����Ʈ(�����۰���ID)
 * <pre>
 * ��(MONEY)	������ȣ : 40	(0 ~ 999,999)
 * ������ ����(ITEM_CLASS)		(1 ~ 20)		: 5  bit  0~31		
 * �ƾ��� ��ȣ(ITEM_ID)		(0 ~ 999)		: 10 bit  0~1023	
 * 
 * ��� ��ȣ1(JAMMING1)		(0~120)			: 7  bit  0~127
 * ��� ��ȣ2(JAMMING2)		(0~120)         : 7  bit  0~127
 * ��� ��ȣ3(JAMMING3)		(0~120)         : 7  bit  0~127
 * 
 * ��ȭ ���(RESMELT)			(0~9)			: 4  bit  0~15		��� �������� ��츸..
 * ǰ��(QUALITY)				(0~120)			: 7  bit  0~127		��� �������� ��츸..
 * ����(QUANTITY)				(1~999)			: 10 bit  0~1023	�Ҹ�, ��Ÿ �������� ���
 * 
 *   ��� : 5 + 10 + 21 + 11 ==> 15+33 : 48   6 bytes
 *   ��Ÿ : 5 + 10 + 10      ==> 15+10
 *   ��   : 5 + 10 + xx
 * </pre>
 */
#ifndef	__CITEM_H
#define	__CITEM_H

#include  <Windows.h>
#include "DataTYPE.h"

//-------------------------------------
// 2007.01.10/��뼺/�߰� - �Ⱓ�� ������
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

#define __MAX_ITEM_999_EXT //����			// 2006.07.06/��뼺/�߰� - ������ 999 ���� Ȯ��

#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------

#define	MAX_ITEM_LIFE			1000		/// �ִ� ������ ����
#define	MAX_DUP_ITEM_QUANTITY	999			/// �ִ� ��ø ������ ������ ����

#define DEF_DAY_CONVERT_SEC 86400
#define DEF_MIN_CONVERT_SEC 60

enum EN_MILEAGE_TYPE
{
	EN_FREE =0,								  //�Ϲݾ�����..,
	EN_DAYRATE,               //������ ������ (��¥����),
	EN_METERRATE ,          //������ ������,
	EN_DURABILITY ,         //������ ������,
	EN_CONSUME ,              //�Ҹ� ������.
	EN_MIELAGE_TYPE,					 //���ϸ��� ������ ��,
};


static int  MileageTypeCol[ITEM_TYPE_RIDE_PART+1]  =
{
	-1,
	48,    // 1	LIST_FACEITEM.stb	�� ���	
	48,	 	// 2	LIST_CAP.stb
	48,	 	// 3	LIST_BODY.stb
	48,	 		// 4	LIST_ARMS.stb
	48,	 // 5	LIST_FOOT.stb
	48,	 		// 6	LIST_BACK.stb
	48,      //7 LIST_JEWEL.stb
	48,	 	// 8	LIST_WEAPON.stb		����
	48,	 	// 9	LIST_SUBWPN.stb
	27,		//10	LIST_USEITEM.stb	�Ҹ�			
	22,	// 11	LIST_JEMITEM.stb	��Ÿ : ����
	18,				// 12	LIST_NATURAL.stb
	-1,				// 13	LIST_QUESTITEM.stb
	72
};

static BYTE MileageDateCol[ITEM_TYPE_RIDE_PART+1] = 
{
	-1,
	49,    // 1	LIST_FACEITEM.stb	�� ���	
	49,	 	// 2	LIST_CAP.stb
	49,	 	// 3	LIST_BODY.stb
	49,	 		// 4	LIST_ARMS.stb
	49,	 // 5	LIST_FOOT.stb
	49,	 		// 6	LIST_BACK.stb
	49,      //7 LIST_JEWEL.stb
	49,	 	// 8	LIST_WEAPON.stb		����
	49,	 	// 9	LIST_SUBWPN.stb
	28,	 //	10 LIST_USEITEM.stb	�Ҹ�		
	23,	// 11	LIST_JEMITEM.stb	��Ÿ : ����
	19,				// 12	LIST_NATURAL.stb
	-1,				// 13	LIST_QUESTITEM.stb
	73
};



#pragma pack (push, 1)

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	struct tagPartITEM 
	{
		unsigned int	m_nItemNo		: 10;	///< 0~1023	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 999)
		unsigned int	m_nGEM_OP		: 9;	///< 0~512	������ȣ(m_bHasSocket==1) �Ǵ� �ɼ� ��ȣ(m_bHasSocket==0)
		unsigned int	m_bHasSocket	: 1;	///< 0~1		���� ���� ����
		unsigned int	m_cGrade	    : 4;	///< 0~15		���						(0~9)

	} ;
#else	//------------------------------------- #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	struct tagPartITEM 
	{
		unsigned int	m_nItemNo		: 18;	///< 0~262143	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 200000)
		unsigned int	m_nGEM_OP		: 9;	///< 0~512	������ȣ(m_bHasSocket==1) �Ǵ� �ɼ� ��ȣ(m_bHasSocket==0)
		unsigned int	m_bHasSocket	: 1;	///< 0~1		���� ���� ����
		unsigned int	m_cGrade	    : 4;	///< 0~15		���						(0~9)
	} ;
#endif	// #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
//-------------------------------------


#ifdef	__CLIENT
#define	tagITEM		tagBaseITEM
#endif


// �� 48 bits, 6 bytes
// �� 48 bits, 8 bytes
struct tagBaseITEM
{
//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	union 
	{
		/// ��� ������ ����
		struct
		{	
			// LSB ::
			// �Ʒ� ���� �ϳ��� ��Ʈ �÷��� ��.
			unsigned short	m_cType			: 5;	///< 0~31		������ ����(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo		: 10;	///< 0~1023	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 999)
			unsigned short	m_bCreated		: 1;	///< 0~1		������ �������ΰ� ?

			unsigned int	m_nGEM_OP		: 9;	///< 0~512	������ȣ(m_bHasSocket==1) �Ǵ� �ɼ� ��ȣ(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	///< 0~127	������

			unsigned int	m_nLife			: 10;	///< 0~1023	����
			unsigned int	m_bHasSocket	: 1;	///< 0~1		���� ���� ����
			unsigned int	m_bIsAppraisal	: 1;	///< 0~1		�ɼ� ���� ����
			unsigned int	m_cGrade	    : 4;	///< 0~15		���						(0~9)

			// 16 + 16 + 16 => 48
			// MSB ::
		} ;

		/// �Ҹ�, ��Ÿ ������ ����
		struct {
			unsigned short	m_cType_1		: 5;	///< 0~31		������ ����(ITEM_CLASS)		(1 ~ 20)
			unsigned short	m_nItemNo_1		: 10;	///< 0~1023	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 999)
			unsigned int	m_uiQuantity	: 32;	///< ����(��)
		} ;

		/// �� ������ ����
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
#else	//-------------------------------------  #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	union 
	{
		/// ��� ������ ����
		struct
		{	
			// LSB ::
			// �Ʒ� ���� �ϳ��� ��Ʈ �÷��� ��.
			unsigned int	m_cType			: 5;	///< 0~31		������ ����(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo		: 26;	///< 0~4134303	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 200000)
													/// tagPartITEM�� m_nItemNo�� ��ġ ��Ű�� ���� 18��Ʈ�� ���
			unsigned int	m_bCreated		: 1;	///< 0~1		������ �������ΰ� ?

			unsigned int	m_nGEM_OP		: 9;	///< 0~512	������ȣ(m_bHasSocket==1) �Ǵ� �ɼ� ��ȣ(m_bHasSocket==0)
			unsigned int	m_cDurability	: 7;	///< 0~127	������

			unsigned int	m_nLife			: 10;	///< 0~1023	����
			unsigned int	m_bHasSocket	: 1;	///< 0~1		���� ���� ����
			unsigned int	m_bIsAppraisal	: 1;	///< 0~1		�ɼ� ���� ����
			unsigned int	m_cGrade	    : 4;	///< 0~15		���						(0~9)

			// 32 + 16 + 16 => 64
			// MSB ::
		} ;

		/// �Ҹ�, ��Ÿ ������ ����
		struct {
			unsigned int	m_cType_1		: 5;	///< 0~31		������ ����(ITEM_CLASS)		(1 ~ 20)
			unsigned int	m_nItemNo_1		: 26;	///< 0~4134303	�ƾ��� ��ȣ(ITEM_ID)		(0 ~ 200000)
			unsigned int    __dummy_0       : 1;

			unsigned int	m_uiQuantity	: 32;	///< ����(��)
		} ;

		/// �� ������ ����
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
#endif	// #ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
//-------------------------------------

//-------------------------------------
// 2007.01.10/��뼺/�߰� - �Ⱓ�� ������
#ifdef __ITEM_TIME_LIMIT
	struct {
		DWORD dwPickOutTime;
		WORD  wPeriod;
	};

	// 2007�� 1�� 1�� 0�� 0�� 0�ʺ��� ��������� �ð��� �ʴ�����
	void set_PickOutTime(WORD period);

	// 1970�� 1�� 1�� 0�� 0�� 0�ʺ��� 2007�� 01�� 01�� 0�� 0�� 0�ʱ����� �ð��� �ʴ�����
	time_t get_basicTime();

	// ���Ⱓ�� ����ƴ°� üũ - true : �Ⱓ����
	bool check_timeExpired();

	// ��ȿ���Ⱓ�߿� �����ٿ��� �־����� �ٿ�� �ð���ŭ ����ð��� ������ �ش�.
	time_t calc_endTime(time_t start_t, time_t end_t, time_t down_start_t, time_t down_end_t, int nValue);
#endif	//  __ITEM_TIME_LIMIT
//-------------------------------------

	void		   Init(int iItem, unsigned int uiQuantity=1);

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	void		   Clear ()				{	m_dwLSB=m_wMSB=0;				}
#else

//-------------------------------------
// 2007.01.10/��뼺/���� - �Ⱓ�� ������
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
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	unsigned short GetItemNO ()			{	return	m_nItemNo;				}
	unsigned short GetHEADER ()			{	return (m_wHeader & 0x7fff);	}	/// m_bCreated :: ��� �񱳽� ������Ʈ ����...
#else
	unsigned int   GetItemNO ()			{	return	m_nItemNo;				}
	unsigned int   GetHEADER ()			{	return (m_dwHeader & 0x7fffffff);	}	/// m_bCreated :: ��� �񱳽� ������Ʈ ����...
#endif
//-------------------------------------

	unsigned short GetGrade ()			{	return	m_cGrade;		}
	unsigned short GetOption ()			{	return  m_nGEM_OP;		}
	unsigned short GetGemNO ()			{	return  m_nGEM_OP;		}

	unsigned short GetLife()			{	return  m_nLife;		}
	unsigned short GetDurability()		{	return  m_cDurability;	}
	unsigned int   GetMoney ()			{	return	m_uiMoney;		}

//-------------------------------------
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
	bool IsEmpty ()						{	return (0==m_wHeader);	}
#else
	bool IsEmpty ()						{	return (0==m_dwHeader);	}
#endif
//-------------------------------------


	bool IsAppraisal()					{	return (0!=m_bIsAppraisal);		}
	bool HasSocket()					{	return (0!=m_bHasSocket);		}

	bool IsEnableDROP ();					/// �����Ⱑ ������ �������ΰ� ?
	bool IsEnableSELL ();					/// �ȱⰡ ������ �������ΰ� ?
	bool IsEnableKEEPING ();				/// ���࿡ ���� ������ �������ΰ� ?

//-------------------------------------
/*
	static bool IsValidITEM (WORD wType, WORD wItemNO);
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	static bool IsValidITEM (WORD wType, int wItemNO);
//-------------------------------------
	static bool IsValidITEM (tagBaseITEM *pItem)		{	return tagBaseITEM::IsValidITEM( pItem->GetTYPE(), pItem->GetItemNO() );	}
	bool IsValidITEM ()					{	return IsValidITEM( this->GetTYPE(), this->GetItemNO() );	}

	static bool IsEnableDupCNT( unsigned short cType )		
	{	
		// �ߺ� �������� �������̳�???
		return (cType>=ITEM_TYPE_USE && cType<ITEM_TYPE_RIDE_PART);
	}
	bool IsEnableDupCNT()				{	return IsEnableDupCNT(m_cType);	}
	bool IsCreated()					{	return (1==m_bCreated);			}
	bool IsEquipITEM()					{	return ( m_cType && m_cType < ITEM_TYPE_USE );					}	/// ���� �������ΰ�?
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
	short			Subtract( tagITEM &sITEM );		/// �־��� ������ ��ŭ ���� ���� ��������� sITEM�� ��� �ִ�.
	void			SubtractOnly (tagITEM &sITEM);	/// �־��� ������ ��ŭ ���� ����.

	bool			IsEnableAppraisal ();			/// ���������� �������ΰ�?
	bool			IsEnableExchange ();			/// �����Ⱑ ������ �������ΰ� ?
	bool			IsEnableSeparate ();
	bool			IsEnableUpgrade ();
	bool			HasLife();
	bool			HasDurability();
	unsigned short	GetModelINDEX ();
	char*			GettingMESSAGE (int iInventoryListNo);
	char*			GettingMESSAGE ();
	char*			GettingQuestMESSAGE();
	char*			SubtractQuestMESSAGE();
	///�Ҹ�ź�������� ShotType�� ���
	static t_eSHOT GetNaturalBulletType( int iItemNo );
	t_eSHOT GetBulletType();
	///���߷�
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

	/// ���� �����ۿ��� �־��� ������ ��ŭ ����, ���� ���Ը� �����Ѵ�.
	short	Subtract( tagITEM &sITEM );		/// �־��� ������ ��ŭ ���� ���� ��������� sITEM�� ��� �ִ�.
	void	SubtractOnly (tagITEM &sITEM);	/// �־��� ������ ��ŭ ���� ����.
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
#ifndef __MAX_ITEM_999_EXT	// 2006.07.06/��뼺/���� - ������ 999 ���� Ȯ��
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

//���������� Ŭ���̾�Ʈ������� ���� �ø���Ű�� �����ϰ� ����ü�� ����..
//Ŭ���̾�Ʈ������ �״�� CMileageInv���� ����ϸ��...

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
	BOOL SetDate();                      //���ϸ��� ������ ��¥(�ð�)����..
	BOOL SetDayRate();				//������ ������ ���ϸ��� ����Ʈ ����
	BOOL SetMeterRate();          // ������ ������ ���ϸ��� ����..
	BOOL ChkPassMileage(DWORD dwCurTime);  //���ϸ��������� ��¥�� ����üũ
	BOOL SubMeter(BYTE btSubMeter =1);
	void Init_FirstPickOut();                                //÷���� ���ϸ��� ������ �����ö� ������ �������ִ� �Լ�..
	void Equip();														    

};
#else

typedef Net_MileageItem MileageItem;

#endif


#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif