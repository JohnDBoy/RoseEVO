#ifndef	__IO_STB_H
#define	__IO_STB_H
#include "DataTYPE.h"
#include "classHASH.h"
#include "classSTR.h"
#include "classLOG.h"
#ifndef __SERVER
	#include "../GameCommon/StringManager.h"
#endif
//-------------------------------------------------------------------------------------------------

/**
 * \ingroup SHO_GS_LIB
 * \date 2005-08-25
 *
 * \brief
 *
 * \todo 
 *
 * \bug 
 *
 * \warning
 *
 */
class STBVALUE {
private:
	bool	m_bString;

	int		m_iValue;
	CStrVAR*m_pString;

public :
	STBVALUE ();
	~STBVALUE ();

	void	SetTYPE (bool bString)	{	m_bString=bString;		}
	bool	IsString ()				{	return m_bString;		}

	void	SetVALUE (char *szValue);
	void	SetVALUE (int iValue);

	int		GetSHORT ()		{	return GetINT();			}
	int		GetINT ()		{	return m_iValue;			}

	char   *GetSTR () // kchs ���� .. ���⼭ ����.
	{	
		if( !m_bString )
			return NULL;

		if( !m_pString )
		{
			g_LOG.CS_ODS (0xffff, "Null String Founded\n");
			return NULL;
		}

		return m_pString->Get();
		// return (m_bString)?m_pString->Get():NULL;
	}
#ifdef	__SERVER
	int		GetStrLEN()		{	return IsString()?m_pString->BuffLength():0;	}
#endif
} ;	


/**
 * \ingroup SHO_GS_LIB
 * \date 2005-08-25
 *
 * \brief
 *
 * \todo 
 *
 * \bug 
 *
 * \warning
 *
 */
class STBDATA {
private:
//-------------------------------------
/*
	classHASH< short >	m_KeyTable;
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	classHASH< int >	m_KeyTable;
//-------------------------------------

	bool	m_bLoad2;
	char   m_strSTBName[64];
public :
//-------------------------------------
/*
	short	m_nDataCnt;
	short	m_nColCnt;
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	int	m_nDataCnt;
	int	m_nColCnt;
//-------------------------------------

	char  **m_ppNAME;
	char  **m_ppDESC;
	union {
		int		**m_ppDATA;
		STBVALUE**m_ppVALUE;
	} ;
	
	STBDATA ()				{	m_bLoad2=false, m_ppDATA=NULL,m_nColCnt=m_nDataCnt=0;	}
	STBDATA(char *pszName) {	m_bLoad2=false, m_ppDATA=NULL,m_nColCnt=m_nDataCnt=0; SetSTBName(pszName); }
	virtual ~STBDATA ()		{	this->Free();					}

	bool	Load2 (char *szFileName, bool bHasTYPE, bool bMakeKEY);

	bool	Load (char *szFileName, bool bHasNameCol=false, bool bHasDescCol=false, bool bMakeKEY=false);
	void	Free (void);

	//	xls2stb.exe�� ����� �����ڵ�stb���� �б�..
	bool	LoadWSTB (bool bCheckQuotationMark, char *szFileName, int iKeyColIDX, ...);

	void	SetSTBName(char *pszName)
	{
		if(!pszName)
		{
			return;
		}

		char szTemp[64];

		strcpy(szTemp,pszName);

		//�ҹ��ڷ� �ٲ㼭 �̸��� ����..	
		//���߿� �� �̸����� ����ã�Ƽ� �о�� ������..
		strcpy(m_strSTBName,_strlwr(szTemp));
	}

	char* GetSTBName() { return m_strSTBName; }

//-------------------------------------
/*
	char   *GetValueSTR(short nCol, short nRow)
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	char   *GetValueSTR(int nCol, int nRow)
//-------------------------------------
	{
		if ( nCol < 0 || nRow < 0 )
			return NULL;
		if ( nCol >= m_nColCnt || nRow >= m_nDataCnt )
			return NULL;
		return m_ppVALUE[ nRow ][ nCol ].GetSTR();		
	}

//-------------------------------------
/*
	short	GetRowIndex (t_HASHKEY HashKey)		
	{	
		tagHASH< short > *pNode;
		pNode = m_KeyTable.Search( HashKey );
		return ( pNode ) ? pNode->m_DATA : 0;
	}
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	int	GetRowIndex (t_HASHKEY HashKey)		
	{	
		tagHASH< int > *pNode;
		pNode = m_KeyTable.Search( HashKey );
		return ( pNode ) ? pNode->m_DATA : 0;
	}
//-------------------------------------

//-------------------------------------
/*
	short	GetRowIndex (char *szName)			{	return GetRowIndex( StrToHashKey( szName ) );	}
*/
// 2006.07.11/��뼺/���� - ������ 999 ���� Ȯ��
	int	GetRowIndex (char *szName)			{	return GetRowIndex( StrToHashKey( szName ) );	}
//-------------------------------------

	int	GetData(DWORD nRow,DWORD  nCol)
	{
		if(nRow>=(DWORD)m_nDataCnt)
		{
			g_LOG.OutputString(0xffff,"%s GetData Count  Error  %d, %d ",m_strSTBName,m_nDataCnt,nRow);
			return 0;
		}

		if(nCol>=(DWORD)m_nColCnt)
		{
			g_LOG.OutputString(0xffff,"%s GetData column  Error  %d, %d ",m_strSTBName,m_nColCnt,nCol);
			return 0;
		}
		return m_ppDATA[nRow][nCol];
	}

	BOOL	SetData(DWORD nRow,DWORD  nCol,int nData)
	{
		if(nRow>=(DWORD)m_nDataCnt)
		{
			g_LOG.OutputString(0xffff,"%s SetData Count  Error  %d, %d ",m_strSTBName,m_nDataCnt,nRow);
			return FALSE;
		}

		if(nCol>=(DWORD)m_nColCnt)
		{
			g_LOG.OutputString(0xffff,"%s SetData column  Error  %d, %d ",m_strSTBName,m_nColCnt,nCol);
			return FALSE;
		}

		 m_ppDATA[nRow][nCol] = nData;
		 return TRUE;

	}
} ;

//-------------------------------------------------------------------------------------------------
// ���� ������ STB 21,22Column�߰�( ���� ���� ���� �߰� ): 2004/1/12 - nAvy

extern STBDATA *g_pTblSTBs[ ITEM_TYPE_RIDE_PART+1 ];

static int ItemSTBGetData(DWORD T,DWORD I,DWORD C)
{
	if(T<1||T>ITEM_TYPE_RIDE_PART)
	{
		g_LOG.OutputString(LOG_NORMAL,"GetItem TYPE(%d) Erorr!!",T);
		return 0;
	}

	return g_pTblSTBs[T]->GetData(I,C);
}

static char* ItemName(DWORD T,DWORD I)
{
	if(T<1||T>ITEM_TYPE_RIDE_PART)
	{
		g_LOG.OutputString(LOG_NORMAL,"GetItem TYPE(%d) Erorr!!",T);
		return NULL;
	}

	if( I>=(DWORD)g_pTblSTBs[T]->m_nDataCnt)
		return NULL;

	return g_pTblSTBs[T]->m_ppNAME[ I ];
}

static char* ItemDesc(DWORD T,DWORD I)
{
	if(T<1||T>ITEM_TYPE_RIDE_PART)
	{
		g_LOG.OutputString(LOG_NORMAL,"GetItem TYPE(%d) Erorr!!",T);
		return NULL;
	}

	if( I>=(DWORD)g_pTblSTBs[T]->m_nDataCnt)
		return NULL;

	return g_pTblSTBs[T]->m_ppDESC[ I ];
}




// ��� �ߺз� �������� ����
#ifdef __SERVER
	#define	ITEM_NAME(T,I)					ItemName(T,I)			// ������ �̸�
	#define ITEM_DESC(T,I)					ItemDesc(T,I)			// ������ ����
#else
	#define	ITEM_NAME(T,I)					CStringManager::GetSingleton().GetItemName( T, I )// ������ �̸�
	#define ITEM_DESC(T,I)					CStringManager::GetSingleton().GetItemDesc( T, I )// ������ ����
#endif

///������� - 0: ��� ��밡��, 1: ���� �ǸŸ� �Ұ�, 2: ������,��ȯ�� �Ұ�, 3:�����Ǹ�,������,��ȯ �Ұ�
#define	ITEM_ANNOUNCE_TYPE(T,I)			ItemSTBGetData(T, I ,  2 )
#define ITEM_USE_RESTRICTION(T,I)		ItemSTBGetData(T, I ,  3 )

#define ITEM_NO_RESTRICTION				0x00
#define ITEM_DONT_SELL					0x01
#define ITEM_DONT_DROP_EXCHANGE			0x02
#define ITEM_DONT_SELL_DROP_EXCHANGE	0x03
#define	ITEM_ENABLE_KEEPING				0x04




#define	ITEM_TYPE(T,I)					ItemSTBGetData(T, I ,  4 )	// ������ ���� : �ߺз�?
#define	ITEM_BASE_PRICE(T,I)			ItemSTBGetData(T, I ,  5 )	// ���ذ���
#define	ITEM_PRICE_RATE(T,I)			ItemSTBGetData(T, I ,  6 )	// ���� ������
#define	ITEM_WEIGHT(T,I)				ItemSTBGetData(T, I ,  7 )	// ����
#define	ITEM_QUALITY(T,I)				ItemSTBGetData(T, I ,  8 )	// ǰ��
#define	ITEM_ICON_NO(T,I)				ItemSTBGetData(T, I ,  9 )	// ������ ��ȣ
#define	ITEM_FIELD_MODEL(T,I)			ItemSTBGetData(T, I , 10 )	// �ʵ� �� ����Ÿ
#define	ITEM_EQUIP_SOUND(T,I)			ItemSTBGetData(T, I , 11 )	// ���� ȿ����
#define	ITEM_MAKE_NUM(T,I)				ItemSTBGetData(T, I , 12 )	// ���� ��ȣ
#define ITEM_SKILL_LEV(T,I)				ItemSTBGetData(T, I , 13 )	// ��ų ����
#define ITEM_PRODUCT_IDX(T,I)			ItemSTBGetData(T, I , 14 )	// ��� ��ȣ
#define	ITEM_MAKE_DIFFICULT(T,I)		ItemSTBGetData(T, I , 15 )	// ���� ���̵�
#define ITEM_TRADE_UNIONPOINT(T,I)		ITEM_MAKE_DIFFICULT(T,I)			// ���ջ������� ������ ���Խ� �ʿ��� ��������Ʈ

// �Ҹ�, ��Ÿ ������ ����
#define	ITEM_RATE_TYPE(T,I)				ItemSTBGetData(T, I , 16)	// ����ǰ ����

// ���� ������ �������� ����
/*
#define	ITEM_JOB_TYPE_CNT				3
#define	ITEM_JOB_TYPE(T,I,C)			ItemSTBGetData(T, I , 16+C ]
#define	ITEM_JOB_TYPE1(T,I)				ItemSTBGetData(T, I , 16 ]	// �ش� ����1
#define	ITEM_JOB_TYPE2(T,I)				ItemSTBGetData(T, I , 17 ]	// �ش� ����2
#define	ITEM_JOB_TYPE3(T,I)				ItemSTBGetData(T, I , 18 ]	// �ش� ����3
*/
#define ITEM_EQUIP_REQUIRE_CLASS(T,I)		ItemSTBGetData(T, I , 16 )
#define ITEM_EQUIP_REQUIRE_UNION_CNT		2
#define ITEM_EQUIP_REQUIRE_UNION(T,I,C)		ItemSTBGetData(T, I , 17 + C )


#define ITEM_NEED_DATA_CNT				2
#define	ITEM_NEED_DATA_TYPE(T,I,C)		ItemSTBGetData(T, I , 19+(C*2) )	// ������ ����
#define	ITEM_NEED_DATA_VALUE(T,I,C)		ItemSTBGetData(T, I , 20+(C*2) )	// ������ ����
#define	ITEM_NEED_UNION_CNT				2
#define	ITEM_NEED_UNION(T,I,C)			ItemSTBGetData(T, I , 23+(C*3) )
#define ITEM_ADD_DATA_TYPE(T,I,C)		ItemSTBGetData(T, I , 24+(C*3) )
#define	ITEM_ADD_DATA_VALUE(T,I,C)		ItemSTBGetData(T, I , 25+(C*3) )
#define	ITEM_DURABITY(T,I)				ItemSTBGetData(T, I , 29 )	// ������ ������

#define	ITEM_RARE_TYPE(T,I)				ItemSTBGetData(T, I , 30 )
#define ITEM_DEFENCE(T,I)				ItemSTBGetData(T, I , 31 )	// ����
#define ITEM_RESISTENCE(T,I)			ItemSTBGetData(T, I , 32 )	// �׸���

// UPGRADE_KRRE
#define ITEM_UPGRADE_NO(T,I)			ItemSTBGetData(T, I , 45 )	// ��� ��ȣ
#define ITEM_UPGRADE_ITEMGRADE(T,I)		ItemSTBGetData(T, I , 46 )  // ��ý� ������ ���


//-------------------------------------------------------------------------------------------------
// LIST_CAP.STB
#define HELMET_NAME(I)					ITEM_NAME( ITEM_TYPE_HELMET, I );


//	@Use ITEM_xxxx
#define HELMET_HAIR_TYPE(I)				g_TblHELMET.GetData( I , 33 )

//-------------------------------------------------------------------------------------------------
// LIST_BODY.STB
#define	ARMOR_NAME(I)					ITEM_NAME( ITEM_TYPE_ARMOR, I );
//	@Use ITEM_xxxx
#define	ARMOR_MAT_TYPE(I)				g_TblARMOR.GetData( I , 33 )		// ���� Ÿ��

//-------------------------------------------------------------------------------------------------
// LIST_FOOT.STB
#define	BOOTS_NAME(I)				ITEM_NAME( ITEM_TYPE_BOOTS, I )
//	@Use ITEM_xxxx
#define	BOOTS_MOVE_SPEED(I)				g_TblBOOTS.GetData( I , 33 )

//-------------------------------------------------------------------------------------------------
// LIST_ARMS.STB
#define	GAUNTLET_NAME(I)			ITEM_NAME( ITEM_TYPE_GAUNTLET, I )

//	@Use ITEM_xxxx

//-------------------------------------------------------------------------------------------------
// LIST_FACEITEM.STB	�󱼾����� 
#define FACEITEM_NAME(I)			ITEM_NAME( ITEM_TYPE_FACE_ITEM, I )
#define FACEITEM_DESC(I)			ITEM_DESC( ITEM_TYPE_FACE_ITEM, I )


//-------------------------------------------------------------------------------------------------
// LIST_BACK.STB		��������� 
#define BACKITEM_NAME(I)				ITEM_NAME( ITEM_TYPE_KNAPSACK, I )
#define BACKITEM_DESC(I)				ITEM_DESC( ITEM_TYPE_KNAPSACK, I )	


//	@Use ITEM_xxxx
#define	BACKITEM_MOVE_SPEED(I)			g_TblBACKITEM.GetData( I , 33 )


//-------------------------------------------------------------------------------------------------
// LIST_WEAPON.STB
#define	WEAPON_NAME(I)					ITEM_NAME( ITEM_TYPE_WEAPON, I )

#define	WEAPON_TYPE(I)					g_TblWEAPON.GetData( I ,  4 )
//	@Use ITEM_xxxx
#define	WEAPON_GEM_COUNT(I)				g_TblWEAPON.GetData( I , 5+25 )

#define	WEAPON_ATTACK_RANGE(I)			g_TblWEAPON.GetData( I , 5+28 )
#define	WEAPON_MOTION_TYPE(I)			g_TblWEAPON.GetData( I , 5+29 )
#define	WEAPON_ATTACK_POWER(I)			g_TblWEAPON.GetData( I , 5+30 )
#define	WEAPON_ATTACK_SPEED(I)			g_TblWEAPON.GetData( I , 5+31 )
#define	WEAPON_IS_MAGIC_DAMAGE(I)		g_TblWEAPON.GetData( I , 5+32 )
#define	WEAPON_BULLET_EFFECT(I)			g_TblWEAPON.GetData( I , 5+33 )
#define	WEAPON_DEFAULT_EFFECT(I)		g_TblWEAPON.GetData( I , 5+34 )
#define	WEAPON_ATK_START_SOUND(I)		g_TblWEAPON.GetData( I , 5+35 )
#define	WEAPON_ATK_FIRE_SOUND(I)		g_TblWEAPON.GetData( I , 5+36 )
#define	WEAPON_ATK_HIT_SOUND(I)			g_TblWEAPON.GetData( I , 5+37 )
#define	WEAPON_GEMMING_POSITION(I)		g_TblWEAPON.GetData( I , 43 )

// UPGARDE_KRRE
#define UPGRADE_WEAPON_SUCPOS(I)		g_TblUPGRADE.GetData( I , 1 )		// �������� ���� Ȯ��
#define UPGRADE_SUBWPN_SUCPOS(I)		g_TblUPGRADE.GetData( I , 3 )		// ������ ���� Ȯ��

//-------------------------------------------------------------------------------------------------
// LIST_SUBWPN.STB
#define	SUBWPN_NAME(I)					ITEM_NAME( ITEM_TYPE_SUBWPN, I )
#define	SUBWPN_TYPE(I)					g_TblSUBWPN.GetData( I ,  4 )
//	@Use ITEM_xxxx
#define SUBWPN_GEM_COUNT(I)				g_TblSUBWPN.GetData( I , 5+25 )

//#define SUBWPN_ATTACK_RANGE(I)			g_TblSUBWPN.GetData( I , 5+28 )
//#define SUBWPN_MOTION_TYPE(I)			g_TblSUBWPN.GetData( I , 5+29 )
//#define SUBWPN_ATTACK_POWER(I)			g_TblSUBWPN.GetData( I , 5+30 )
//#define	SUBWPN_ATTACK_SPEED(I)			g_TblSUBWPN.GetData( I , 5+31 )
//#define	SUBWPN_IS_MAGIC_DAMAGE(I)		g_TblWEAPON.GetData( I , 5+32 )
//#define	SUBWPN_BULLET_TYPE(I)			g_TblWEAPON.GetData( I , 5+33 )
//#define SUBWPN_LIST_POSITION(I)			g_TblWEAPON.GetData( I , 5+34 )
//#define SUBWPN_RES(I)					g_TblSUBWPN.GetData( I , 5+35 )

#define	SUBWPN_GEMMING_POSITION(I)		g_TblSUBWPN.GetData( I , 34 )

//-------------------------------------------------------------------------------------------------
// LIST_JEWEL.STB		��ű������� 
#define	JEWEL_NAME(I)						ITEM_NAME( ITEM_TYPE_JEWEL, I)

//	@Use ITEM_xxxx

#define JEWEL_NEED_DATA_TYPE(I)				g_TblJEWELITEM.GetData( I , 19 )
#define JEWEL_NEED_DATA_VALUE(I)			g_TblJEWELITEM.GetData( I , 20 )

#define	JEWEL_NEED_UNION(I,C)				g_TblJEWELITEM.GetData( I , 23+(C*3) )
#define JEWEL_ADD_DATA_TYPE(I,C)			g_TblJEWELITEM.GetData( I , 24+(C*3) )
#define	JEWEL_ADD_DATA_VALUE(I,C)			g_TblJEWELITEM.GetData( I , 25+(C*3) )
///�̻� ���� ������
//-------------------------------------------------------------------------------------------------
// LIST_USEITEM.STB		�Ҹ������
#define USEITEM_NAME(I)						ITEM_NAME(ITEM_TYPE_USE, I)
#define USEITEM_DESC(I)						ITEM_DESC(ITEM_TYPE_USE, I)

#define	USEITEM_STORE_SKIN(I)				g_TblUSEITEM.GetData( I ,  8 )	// ���λ��� �̹��� ��ȣ...

#define USEITEM_CONFILE_IDX(I)				g_TblUSEITEM.GetData( I , 22 )
//	@Use ITEM_xxxx
#define	USEITEM_NEED_DATA_TYPE(I)			g_TblUSEITEM.GetData( I , 17 )
#define	USEITEM_NEED_DATA_VALUE(I)			g_TblUSEITEM.GetData( I , 18 )
#define	USEITEM_ADD_DATA_TYPE(I)			g_TblUSEITEM.GetData( I , 19 )	// ������ ����
#define	USEITEM_ADD_DATA_VALUE(I)			g_TblUSEITEM.GetData( I , 20 )	// ������ ����

#define	USEITEM_SCROLL_LEARN_SKILL			USEITEM_ADD_DATA_VALUE				// ��ų ���� ��ũ��..
#define	USEITEM_SCROLL_USE_SKILL			USEITEM_ADD_DATA_VALUE				// ��ų ����ϴ� ��ũ��.
#define USEITEM_ALLPY_VALUE						USEITEM_ADD_DATA_VALUE               //������ ���� ��ġ ������...(�帱������ ����Ȯ��..)

#define	USEITEM_SCRIPT(I)					g_TblUSEITEM.GetData( I , 21 )
#define	USEITEM_USE_EFFECT(I)				g_TblUSEITEM.GetData( I , 22 )	// ��� ȿ����
#define	USEITEM_USE_SOUND(I)				g_TblUSEITEM.GetData( I , 23 )	// ��� ȿ����
#define	USEITME_STATUS_STB(I)				g_TblUSEITEM.GetData( I , 24 )	// ������ ����

//-------------------------------------
/*
#define	MAX_USEITEM_COOLTIME_TYPE			4	// 0~3
*/
// 2007.01.30/��뼺/���� - ��Ÿ�� Ÿ�� �ִ밪 �ø�
#define	MAX_USEITEM_COOLTIME_TYPE			50	// 0~49
//-------------------------------------

#define	USEITEM_COOLTIME_TYPE(I)			g_TblUSEITEM.GetData( I , 25 )
#define	USEITEM_COOLTIME_DELAY(I)			g_TblUSEITEM.GetData( I , 26 )

#define SET_USEITEM_COOLTIME_TYPE(I,N)         g_TblUSEITEM.SetData( I , 25 ,N)
#define	SET_USEITEM_COOLTIME_DELAY(I,N)			g_TblUSEITEM.SetData( I , 26 ,N)



//-------------------------------------------------------------------------------------------------
// LIST_JEMITEM.STB		����������
#define GEMITEM_NAME(I)						ITEM_NAME( ITEM_TYPE_GEM, I )//�̸�
#define GEMITEM_DESC(I)						ITEM_DESC( ITEM_TYPE_GEM, I )//���� 

//	@Use ITEM_xxxx
#define	GEMITEM_BASE_PRICE(I)				g_TblGEMITEM.GetData( I ,  5 )	// ���ذ���
#define	GEMITEM_PRICE_RATE(I)				g_TblGEMITEM.GetData( I ,  6 )	// ���� ������

#define GEMITEM_ADD_DATA_TYPE(I,C)			g_TblGEMITEM.GetData( I , 16+(C*2) )
#define	GEMITEM_ADD_DATA_VALUE(I,C)			g_TblGEMITEM.GetData( I , 17+(C*2) )

#define GEMITEM_MARK_IMAGE(I)				g_TblGEMITEM.GetData( I , 20 )	//��ũ�̹��� 
#define GEMITEM_ATTACK_EFFECT(I)			g_TblGEMITEM.GetData( I , 21 )	//�����ܿ� ���� �̹���




//-------------------------------------------------------------------------------------------------
// LIST_NATURAL.STB		����� 
#define NATURAL_NAME(I)				ITEM_NAME( ITEM_TYPE_NATURAL, I )
#define NATURAL_DESC(I)				ITEM_DESC( ITEM_TYPE_NATURAL, I )
//	@Use ITEM_xxxx
#define NATURAL_BULLET_NO(I)		g_TblNATUAL.GetData( I , 17 )///���� 2004 / 2 /18 :nAvy g_TblNPC => g_TblNATUAL



#ifdef __SERVER
	#define	NPC_NAME_STR_COLUMN			0

	#define	NPC_NAME(I)					g_TblNPC.m_ppVALUE[ I ][ NPC_NAME_STR_COLUMN ].GetSTR()
	#define	NPC_DEAD_EVENT(I)			g_TblNPC.m_ppVALUE[ I ][ 41 ].GetSTR()

	#define	SET_NPC_LEVEL(I,V)			g_TblNPC.m_ppVALUE[ I ][  7 ].SetVALUE(V)
	#define SET_NPC_ATK_SPEED(I,V)		g_TblNPC.m_ppVALUE[ I ][ 14 ].SetVALUE(V)
	#define SET_NPC_DROP_TYPE(I,V)		g_TblNPC.m_ppVALUE[ I ][ 18 ].SetVALUE(V)
	#define	SET_NPC_DEAD_EVENT(I,V)		g_TblNPC.m_ppVALUE[ I ][ 41 ].SetVALUE(V)
#else
/// use load2 function
	#define	NPC_NAME(I)					CStringManager::GetSingleton().GetNpcName( I )	// NPC �̸�
	#define NPC_DESC(I)					g_TblNPC.m_ppVALUE[ I ][ 41 ].GetSTR()			/// ������ �̺�Ʈ
	#define NPC_HEIGHT(I)				g_TblNPC.m_ppVALUE[ I ][ 42 ].GetINT()			/// NPCŰ
#endif
#define	NPC_WALK_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 2 ].GetINT()
#define NPC_RUN_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 3 ].GetINT()
#define	NPC_SCALE(I)				g_TblNPC.m_ppVALUE[ I ][ 4 ].GetINT()
#define	NPC_R_WEAPON(I)				g_TblNPC.m_ppVALUE[ I ][ 5 ].GetINT()
#define NPC_L_WEAPON(I)				g_TblNPC.m_ppVALUE[ I ][ 6 ].GetINT()
#define	NPC_LEVEL(I)				g_TblNPC.m_ppVALUE[ I ][ 7 ].GetINT()
#define NPC_HP(I)					g_TblNPC.m_ppVALUE[ I ][ 8 ].GetINT()
#define NPC_ATK(I)					g_TblNPC.m_ppVALUE[ I ][ 9 ].GetINT()
#define NPC_HIT(I)					g_TblNPC.m_ppVALUE[ I ][ 10].GetINT()
#define NPC_DEF(I)					g_TblNPC.m_ppVALUE[ I ][ 11 ].GetINT()
#define NPC_RES(I)					g_TblNPC.m_ppVALUE[ I ][ 12 ].GetINT()
#define NPC_AVOID(I)				g_TblNPC.m_ppVALUE[ I ][ 13 ].GetINT()
#define NPC_ATK_SPEED(I)			g_TblNPC.m_ppVALUE[ I ][ 14 ].GetINT()
#define NPC_IS_MAGIC_DAMAGE(I)		g_TblNPC.m_ppVALUE[ I ][ 15 ].GetINT()
#define NPC_AI_TYPE(I)				g_TblNPC.m_ppVALUE[ I ][ 16 ].GetINT()
#define NPC_GIVE_EXP(I)				g_TblNPC.m_ppVALUE[ I ][ 17 ].GetINT()
#define NPC_DROP_TYPE(I)			g_TblNPC.m_ppVALUE[ I ][ 18 ].GetINT()
#define NPC_MARK_NO(I)				NPC_DROP_TYPE(I)			///�̴ϸʿ� ǥ�õǴ� NPC IMAGE NO

#define NPC_DROP_MONEY(I)			g_TblNPC.m_ppVALUE[ I ][ 19 ].GetINT()
#define NPC_DROP_ITEM(I)			g_TblNPC.m_ppVALUE[ I ][ 20 ].GetINT()

#define NPC_UNION_NO(I)				NPC_DROP_ITEM(I)			///���ջ������� �ش� ������ ��ȣ

#define	NPC_NEED_SUMMON_CNT(I)		g_TblNPC.m_ppVALUE[ I ][ 21 ].GetINT()		// ��ȯ�� �ʿ��� ��ȯ�ɷ�ġ
#define NPC_SELL_TAB(I,T)			g_TblNPC.m_ppVALUE[ I ][ 21+T ].GetINT()
#define NPC_SELL_TAB0(I)			g_TblNPC.m_ppVALUE[ I ][ 21 ].GetINT()
#define NPC_SELL_TAB1(I)			g_TblNPC.m_ppVALUE[ I ][ 22 ].GetINT()
#define NPC_SELL_TAB2(I)			g_TblNPC.m_ppVALUE[ I ][ 23 ].GetINT()
#define NPC_SELL_TAB3(I)			g_TblNPC.m_ppVALUE[ I ][ 24 ].GetINT()
#define NPC_CAN_TARGET(I)			g_TblNPC.m_ppVALUE[ I ][ 25 ].GetINT()
#define	NPC_ATK_RANGE(I)			g_TblNPC.m_ppVALUE[ I ][ 26 ].GetINT()
#define	NPC_TYPE(I)					g_TblNPC.m_ppVALUE[ I ][ 27 ].GetINT()
#define	NPC_HIT_MATERIAL_TYPE(I)	g_TblNPC.m_ppVALUE[ I ][ 28 ].GetINT()
#define	NPC_FACE_ICON(I)			g_TblNPC.m_ppVALUE[ I ][ 29 ].GetINT()
#define	NPC_NORMAL_EFFECT_SOUND(I)	g_TblNPC.m_ppVALUE[ I ][ 30 ].GetINT()
#define	NPC_ATTACK_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 31 ].GetINT()		// ���� ���� ����
#define	NPC_HITTED_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 32 ].GetINT()		// �¾����� ���� �Ҹ�
#define	NPC_HAND_HIT_EFFECT(I)		g_TblNPC.m_ppVALUE[ I ][ 33 ].GetINT()		//	�Ǽ� Ÿ��ȿ��
#define	NPC_DEAD_EFFECT(I)			g_TblNPC.m_ppVALUE[ I ][ 34 ].GetINT()
#define NPC_DIE_SOUND(I)			g_TblNPC.m_ppVALUE[ I ][ 35 ].GetINT()		//	������ ȿ����

#define NPC_QUEST_TYPE(I)			g_TblNPC.m_ppVALUE[ I ][ 38 ].GetINT()		//	����Ʈ Ÿ��
#define NPC_GLOW_COLOR(I)			g_TblNPC.m_ppVALUE[ I ][ 39 ].GetINT()		//	����Ʈ Ÿ��
#define NPC_GLOW_COLOR(I)			g_TblNPC.m_ppVALUE[ I ][ 39 ].GetINT()		//	����Ʈ Ÿ��

#define	NPC_STRING_ID_COLOUM		40
#define NPC_STRING_ID(I)			g_TblNPC.m_ppVALUE[ I ][ NPC_STRING_ID_COLOUM ].GetSTR()		//	��Ʈ�� ���̵�

#define	NPC_ATTRIBUTE(I)			g_TblNPC.m_ppVALUE[ I ][ 43 ].GetINT()

//-------------------------------------------------------------------------------------------------

#define	ATTRIBUTE_FORM(SA,NA)		g_TblATTR.GetData( NA , SA )				// SA:��ų�Ӽ�, NA:npc�Ӽ�

//-------------------------------------------------------------------------------------------------

#define	FILE_MOTION(WEAPON,ACTION)	g_TblAniTYPE.GetData( ACTION , WEAPON )

//-------------------------------------------------------------------------------------------------
#define EFFECT_NAME(I)				(NULL)		// g_TblEFFECT.GetData( I , 0 ]
#define	EFFECT_TYPE(I)				g_TblEFFECT.GetData( I , 1 )
#define	EFFECT_POINT_CNT			4
#define	EFFECT_POINT(I,PNO)			g_TblEFFECT.GetData( I , 2+PNO )
#define	EFFECT_POINT1(I)			g_TblEFFECT.GetData( I , 2 )
#define	EFFECT_POINT2(I)			g_TblEFFECT.GetData( I , 3 )
#define	EFFECT_POINT3(I)			g_TblEFFECT.GetData( I , 4 )
#define	EFFECT_POINT4(I)			g_TblEFFECT.GetData( I , 5 )
#define EFFECT_TRAIL_NORMAL(I)		g_TblEFFECT.GetData( I , 6 )
#define EFFECT_TRAIL_CRITICAL(I)	g_TblEFFECT.GetData( I , 7 )
#define EFFECT_TRAIL_DURATION(I)	g_TblEFFECT.GetData( I , 8 )
#define EFFECT_HITTED_NORMAL(I)		g_TblEFFECT.GetData( I , 9 )
#define EFFECT_HITTED_CRITICAL(I)	g_TblEFFECT.GetData( I , 10 )
#define EFFECT_BULLET_NORMAL(I)		g_TblEFFECT.GetData( I , 11 )
#define EFFECT_BULLET_CRITICAL(I)	g_TblEFFECT.GetData( I , 12 )
#define	EFFECT_BULLET_MOVETYPE(I)	g_TblEFFECT.GetData( I , 13 )
#define	EFFECT_BULLET_TYPE(I)		g_TblEFFECT.GetData( I , 14 )
#define EFFECT_BULLET_SPEED(I)		g_TblEFFECT.GetData( I , 15 )
#define	EFFECT_FIRE_SND_IDX(I)		g_TblEFFECT.GetData( I , 16 )
#define	EFFFCT_HIT_SND_IDX(I)		g_TblEFFECT.GetData( I , 17 )

//-------------------------------------------------------------------------------------------------

#define	DROPITEM_ITEMNO(I,NO)		g_TblDropITEM.GetData( I , 1+NO )	// NO >= 1
#define	DROPITEM_ITEM1(I)			g_TblDropITEM.GetData( I , 1 )

#define	SET_DROPITEM_ITEMNO(I,NO,N)		g_TblDropITEM.SetData( I , 1+NO,N )	// NO >= 1
#define	SET_DROPITEM_ITEM1(I,N)			g_TblDropITEM.SetData( I , 1 ,N)


//////////�ѱ� ������ ��� ������ �������� �߰�(���翵 2005.11.15) ////////////////////////////////////
#define  DROP_TYPE_PER(I,T)    g_TblDropITEM.GetData( I ,T+1)       //���Ÿ�Ժ���.. Ȯ�� ��������..(��,������,����)
#define  DROP_ITEM_PER(I,T)    g_TblDropITEM.GetData( I ,(T*2)+5)
#define  DROP_ITEM_NO(I,T)      g_TblDropITEM.GetData( I ,(T*2)+4)
#define  SET_DROP_ITEM_NO(I,T,N) g_TblDropITEM.SetData( I ,(T*2)+4,N)

enum EN_DROP_TYPE
{
	EN_DROP_MONEY =0,EN_DROP_ITEM,EN_DROP_ZONE,EN_DROP_TYPE_END,
};

#define DEF_MAX_DROP_ENTRY		20
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	Hit sound
*/
#define	HIT_SOUND(T, I)				g_TblHitSound.GetData( T , I )

//-------------------------------------------------------------------------------------------------
// LIST_PRODUCT.STB			: ������ ���� ���
#define	PRODUCT_TYPE(I)					g_TblPRODUCT.GetData( I , 0 )
#define	PRODUCT_RAW_MATERIAL(I)			g_TblPRODUCT.GetData( I , 1 )			// ���� ���� ��ȣ

#define	PRODUCT_NEED_ITEM_NO(P,C)		g_TblPRODUCT.GetData( P , 2+(C)*2 )	// ��� ������ ��ȣ
#define	PRODUCT_NEED_ITEM_CNT(P,C)		g_TblPRODUCT.GetData( P , 3+(C)*2 )	// �ʿ� ����

#define	PRODUCT_NEED_ITEM_TYPE_REN(P,C)		g_TblPRODUCT.GetData( P , 4+(C)*3 )	// ��� ������ ��ȣ
#define	PRODUCT_NEED_ITEM_NO_REN(P,C)		g_TblPRODUCT.GetData( P , 5+(C)*3 )	// ��� ������ ��ȣ
#define	PRODUCT_NEED_ITEM_CNT_REN(P,C)		g_TblPRODUCT.GetData( P , 6+(C)*2 )	// �ʿ� ����

/*
	..
	..
	..
*/
#define JEWEL_ADD_JOB
#define	JEWEL_ICON_NO(I)					g_TblJEWELITEM.GetData( I , 9 )
#define	JEWEL_QUALITY(I)					g_TblJEWELITEM.GetData( I , 8 )

// �����ۿ��� ���� �ε����������´�...
#define ITEM_BREAK_IDX(T,I)			ItemSTBGetData(T,I,47)

//�ѱ� ������ �����ۺ��� (���翵 2005.11.1)
#define BREAK_ITEM_IDX(I,C)		g_TblBreak.GetData(I,2+(C)*3)			//���ؾ����� �ε���
#define BREAK_ITEM_CNT(I,C)		g_TblBreak.GetData(I,3+(C)*3)			//���ؾ����� ����
#define BREAK_ITEM_PER(I,C)		 g_TblBreak.GetData(I,4+(C)*3)			//���ؾ����� Ȯ��
#define BREAK_MAX_CNT(I)			 g_TblBreak.GetData(I,62)						//�����ִ�Ƚ��
#define BREAK_ENTRY_ITEM_CNT(I)  g_TblBreak.GetData(I,63)						//���ص�Ͼ����ۼ�..

#define DEF_MAX_BREAK_CNT		4

// ����Ʈ������ 

// ���� 
#ifdef __SERVER
	#define STORE_NAME(I)					g_TblStore.m_ppNAME[ I )
	#define STORE_TAB_ICON(I)				g_TblStore.GetData( I , 1 )
	#define STORE_ITEM(I,T)					g_TblStore.GetData( I , 2+T )
#else
	#define STORE_NAME(I)					CStringManager::GetSingleton().GetStoreTabName( I )
	#define STORE_ITEM(I,T)					g_TblStore.m_ppVALUE[ I ][ 2+T ].GetINT()
#endif

// ����
#define	TELEPORT_NAME(I)					g_TblWARP.m_ppVALUE[ I ][ 0 ].GetSTR()
#define	TELEPORT_ZONE(I)					g_TblWARP.m_ppVALUE[ I ][ 1 ].GetINT()
#define TELEPORT_EVENT_POS(I)				g_TblWARP.m_ppVALUE[ I ][ 2 ].GetSTR()

// EVENT
#define	EVENT_NAME(I)						g_TblEVENT.m_ppVALUE[ I ][ 0 ].GetSTR()
#define EVENT_TYPE(I)						g_TblEVENT.m_ppVALUE[ I ][ 1 ].GetSTR()
#define	EVENT_DESC(I)						g_TblEVENT.m_ppVALUE[ I ][ 2 ].GetSTR()
#define EVENT_FILENAME(I)					g_TblEVENT.m_ppVALUE[ I ][ 3 ].GetSTR()


#define	SKY_MESH(S)							g_TblSKY.m_ppVALUE[ S ][ 0 ].GetSTR()
#define SKY_TEXTURE_CNT						4
#define	SKY_TEXTURE(S,T)					g_TblSKY.m_ppVALUE[ S ][ 1 + T ].GetSTR()
#define SKY_LIGHTMAP_BLEND(I)				g_TblSKY.m_ppVALUE[ I ][ 5 ].GetINT()
#define SKY_AMBIENT_CNT						4
#define SKY_AMBIENT( S, T )					g_TblSKY.m_ppVALUE[ S ][ 6 + T ].GetINT()
#define SKY_CHAR_LIGHT_CNT					4
#define SKY_CHAR_AMBIENT( S, T )			g_TblSKY.m_ppVALUE[ S ][ 10 + (2*T) ].GetINT()
#define SKY_CHAR_DIFFUSE( S, T )			g_TblSKY.m_ppVALUE[ S ][ 10 + (2*T) + 1 ].GetINT()

#define SKY_CHANGE_DELAY_CNT				4
#define SKY_CHANGE_DELAY(I,T)				g_TblSKY.GetData( I , 18 + T ]



// ZONE LIST
#ifdef __SERVER
#define	ZONE_NAME(I)						g_TblZONE.m_ppVALUE[ I ][ 0 ].GetSTR()
#define ZONE_DESC(I)
#else
#define	ZONE_NAME(I)						CStringManager::GetSingleton().GetZoneName( I )	// Zone �̸�
#define ZONE_DESC(I)						CStringManager::GetSingleton().GetZoneDesc( I )	// Zone ����
#endif

#define	ZONE_FILE(I)						g_TblZONE.m_ppVALUE[ I ][ 1 ].GetSTR()
#define	ZONE_START_POS(I)					g_TblZONE.m_ppVALUE[ I ][ 2 ].GetSTR()
#define	ZONE_REVIVE_POS(I)					g_TblZONE.m_ppVALUE[ I ][ 3 ].GetSTR()
#define	ZONE_IS_UNDERGROUND(I)				g_TblZONE.m_ppVALUE[ I ][ 4 ].GetINT()
#define	ZONE_BG_MUSIC_DAY(I)				g_TblZONE.m_ppVALUE[ I ][ 5 ].GetSTR()
#define	ZONE_BG_MUSIC_NIGHT(I)				g_TblZONE.m_ppVALUE[ I ][ 6 ].GetSTR()

#define	ZONE_BG_IMAGE(I)					g_TblZONE.m_ppVALUE[ I ][ 7 ].GetINT()
#define ZONE_MINIMAP_NAME(I)				g_TblZONE.m_ppVALUE[ I ][ 8 ].GetSTR()
#define ZONE_MINIMAP_STARTX(I)				g_TblZONE.m_ppVALUE[ I ][ 9 ].GetINT()
#define ZONE_MINIMAP_STARTY(I)				g_TblZONE.m_ppVALUE[ I ][ 10 ].GetINT()
#define ZONE_OBJECT_TABLE(I)				g_TblZONE.m_ppVALUE[ I ][ 11 ].GetSTR()
#define ZONE_CNST_TABLE(I)					g_TblZONE.m_ppVALUE[ I ][ 12 ].GetSTR()
#define ZONE_DAY_CYCLE(I)					g_TblZONE.m_ppVALUE[ I ][ 13 ].GetINT()
#define ZONE_MORNING_TIME(I)				g_TblZONE.m_ppVALUE[ I ][ 14 ].GetINT()
#define ZONE_DAY_TIME(I)					g_TblZONE.m_ppVALUE[ I ][ 15 ].GetINT()
#define ZONE_EVENING_TIME(I)				g_TblZONE.m_ppVALUE[ I ][ 16 ].GetINT()
#define ZONE_NIGHT_TIME(I)					g_TblZONE.m_ppVALUE[ I ][ 17 ].GetINT()
#define ZONE_PVP_STATE(I)					g_TblZONE.m_ppVALUE[ I ][ 18 ].GetINT()
#define ZONE_PLANET_NO(I)					g_TblZONE.m_ppVALUE[ I ][ 19 ].GetINT()
#define ZONE_TYPE(I)						g_TblZONE.m_ppVALUE[ I ][ 20 ].GetINT()
#define ZONE_CAMERA_TYPE(I)					g_TblZONE.m_ppVALUE[ I ][ 21 ].GetINT()			/// ī�޶󿡼��� ������ġ.
#define	ZONE_JOIN_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 22 ].GetSTR()			/// ���� ���ν� ������ Ʈ����
#define	ZONE_KILL_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 23 ].GetSTR()			/// PK ��� on�� ����ڸ� �׿����� ����� Ʈ����
#define	ZONE_DEAD_TRIGGER(I)				g_TblZONE.m_ppVALUE[ I ][ 24 ].GetSTR()			/// PK ��� on�� �׾����� ����� Ʈ����
#define	ZONE_SECTOR_SIZE(I)					g_TblZONE.m_ppVALUE[ I ][ 25 ].GetINT()			/// �������� ����ϴ� ��
#define ZONE_STRING_ID(I)					g_TblZONE.m_ppVALUE[ I ][ 26 ].GetSTR()			/// ��Ʈ�� ���̵�
#define ZONE_WEATHER_TYPE(I)				g_TblZONE.m_ppVALUE[ I ][ 27 ].GetINT()			/// ����Ÿ��

#define	ZONE_PARTY_EXP_A(I)					g_TblZONE.m_ppVALUE[ I ][ 28 ].GetINT()
#define	ZONE_PARTY_EXP_B(I)					g_TblZONE.m_ppVALUE[ I ][ 29 ].GetINT()

#define	ZONE_RIDING_REFUSE_FLAG(I)			g_TblZONE.m_ppVALUE[ I ][ 30 ].GetINT()			// ž�� �ź� �÷���
#define	ZONE_REVIVE_ZONENO(I)				g_TblZONE.m_ppVALUE[ I ][ 31 ].GetINT()			// ����� ��Ȱ�� ��ȣ
#define	ZONE_REVIVE_X_POS(I)				g_TblZONE.m_ppVALUE[ I ][ 32 ].GetINT()			// ����� ��Ȱ�� ��ǥ
#define	ZONE_REVIVE_Y_POS(I)				g_TblZONE.m_ppVALUE[ I ][ 33 ].GetINT()			// ����� ��Ȱ�� ��ǥ


// INIT AVATAR
#define	AVATAR_STR(I)						g_TblAVATAR.GetData( I , 0 )
#define	AVATAR_DEX(I)						g_TblAVATAR.GetData( I , 1 )
#define	AVATAR_INT(I)						g_TblAVATAR.GetData( I , 2 )
#define	AVATAR_CON(I)						g_TblAVATAR.GetData( I , 3 )
#define	AVATAR_CHARM(I)						g_TblAVATAR.GetData( I , 4 )
#define	AVATAR_SENSE(I)						g_TblAVATAR.GetData( I , 5 )

#define AVATAR_FACEITEM(I)					g_TblAVATAR.GetData( I , 6 )
#define AVATAR_HELMET(I)					g_TblAVATAR.GetData( I , 7 )
#define AVATAR_ARMOR(I)						g_TblAVATAR.GetData( I , 8 )
#define AVATAR_GAUNTLET(I)					g_TblAVATAR.GetData( I , 9 )
#define AVATAR_BOOTS(I)						g_TblAVATAR.GetData( I , 10 )
#define AVATAR_BACKITEM(I)					g_TblAVATAR.GetData( I , 11 )
#define AVATAR_WEAPON(I)					g_TblAVATAR.GetData( I , 12 )
#define AVATAR_SUBWPN(I)					g_TblAVATAR.GetData( I , 13 )

#define AVATAR_ITEM_WEAPON(I,J)				g_TblAVATAR.GetData( I , 14+J )
#define AVATAR_ITEM_USE(I,J)				g_TblAVATAR.GetData( I , 24+(J)*2 )
#define AVATAR_ITEM_USECNT(I,J)				g_TblAVATAR.GetData( I , 25+(J)*2 )
#define AVATAR_ITEM_ETC(I,J)				g_TblAVATAR.GetData( I , 34+(J)*2 )
#define AVATAR_ITEM_ETCCNT(I,J)				g_TblAVATAR.GetData( I , 35+(J)*2 )

#define	AVATAR_MONEY(I)						g_TblAVATAR.GetData( I , 44 )
#define	AVATAR_ZONE(I)						g_TblAVATAR.GetData( I , 45 )

#define AVATAR_START_X(I)					g_TblAVATAR.GetData( I , 46 )
#define AVATAR_START_Y(I)					g_TblAVATAR.GetData( I , 47 )
#define AVATAR_REVIVE_ZONE(I)			g_TblAVATAR.GetData( I , 48 )


/// Option - �ػ�
#define RESOLUTION_DEFAULT					0
#define RESOLUTION_NAME(I)					g_TblResolution.m_ppVALUE[ I ][ 0 ].GetSTR();
#define RESOLUTION_WIDTH(I)					g_TblResolution.m_ppVALUE[ I ][ 1 ].GetINT();
#define RESOLUTION_HEIGHT(I)				g_TblResolution.m_ppVALUE[ I ][ 2 ].GetINT();
#define RESOLUTION_DEPTH(I)					g_TblResolution.m_ppVALUE[ I ][ 3 ].GetINT();

#define FILTER_FOULWORD(I)					g_TblFoulWord.m_ppVALUE[ i ][ 0 ].GetSTR();
#define FILTER_CHANGEWORD(I)				g_TblFoulWord.m_ppVALUE[ i ][ 1 ].GetSTR();



/// ĳ���� ���º��� ����
#ifdef __SERVER
	#define STATE_NAME(I)					g_TblSTATE.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define STATE_NAME(I)					CStringManager::GetSingleton().GetStatusName( I )
#endif
//#define STATE_DESC(I)						g_TblSTATE.m_ppVALUE[ I ][ 18 ].GetSTR()

#define STATE_TYPE(I)						g_TblSTATE.m_ppVALUE[ I ][ 1 ].GetINT()		/// ����
#define STATE_CAN_DUPLICATED(I)				g_TblSTATE.m_ppVALUE[ I ][ 2 ].GetINT()		/// �ߺ� ����
#define STATE_PRIFITS_LOSSES(I)				g_TblSTATE.m_ppVALUE[ I ][ 3 ].GetINT()		/// ���� �Ҹ�
#define STATE_APPLY_ARG(I)					g_TblSTATE.m_ppVALUE[ I ][ 4 ].GetINT()		/// ���� ����

/// ���� �ɷ�
#define STATE_APPLY_ABILITY_CNT				2
#define STATE_APPLY_ING_STB(I,T)			g_TblSTATE.m_ppVALUE[ I ][ 5 + (T)*2 ].GetINT()			/// ���� �ɷ�
#define STATE_APPLY_ABILITY_VALUE(I,T)		g_TblSTATE.m_ppVALUE[ I ][ 5 + (T)*2 + 1 ].GetINT()		/// ���� �ɷ� ��ġ

#define STATE_SYMBOL(I)						g_TblSTATE.m_ppVALUE[ I ][ 9  ].GetINT()	/// ���� �ɺ�
#define STATE_STEP_EFFECT(I)				g_TblSTATE.m_ppVALUE[ I ][ 10 ].GetINT()	/// �ܰ� ȿ��
#define STATE_STEP_SOUND(I)					g_TblSTATE.m_ppVALUE[ I ][ 11 ].GetINT()	/// �ܰ� ����

/// ���ͷ�
#define STATE_CONTROL_CNT					3
#define STATE_CONTROL(I,T)					g_TblSTATE.m_ppVALUE[ I ][ 12 +(T) ].GetINT()	/// ���ͷ�

#define STATE_SHOTDOWN_EFFECT(I)			g_TblSTATE.m_ppVALUE[ I ][ 15 ].GetINT()	/// ���� ȿ��
#define STATE_SHOTDOWN_SOUND(I)				g_TblSTATE.m_ppVALUE[ I ][ 16 ].GetINT()	/// ���� ����

/// STB �ε����� ���� ��ȣ�� ������ ����, �Ҹ� ����
#define STATE_PRIFITS_LOSSES_BY_STATE(I)	g_TblSTATE.m_ppVALUE[ I ][ 17 ].GetINT()	/// ���� ����

/// ���� ������ ����� �޼���
#ifdef __SERVER
	#define STATE_SETTING_STRING(I)				g_TblSTATE.m_ppVALUE[ I ][ 18 ].GetSTR()	/// ���� ��Ʈ��
	#define STATE_DELETE_STRING(I)				g_TblSTATE.m_ppVALUE[ I ][ 19 ].GetSTR()	/// ���� ��Ʈ��
#else
	#define STATE_SETTING_STRING(I)				CStringManager::GetSingleton().GetStatusStartMsg( I )
	#define STATE_DELETE_STRING(I)				CStringManager::GetSingleton().GetStatusEndMsg( I )
#endif

#define STATE_STRING_ID(I)					g_TblSTATE.m_ppVALUE[ I ][ 20 ].GetSTR()	/// ���� ��Ʈ��


///
///	�þ�����...
///

#define CAMERA_NAME(I)						g_TblCamera.m_ppVALUE[ I ][ 0 ].GetSTR()
#define CAMERA_ZSC_FILE(I)					g_TblCamera.m_ppVALUE[ I ][ 1 ].GetSTR()
#define CAMERA_FOV(I)						g_TblCamera.m_ppVALUE[ I ][ 2 ].GetINT()
#define CAMERA_ASPECT_RATIO(I)				g_TblCamera.m_ppVALUE[ I ][ 3 ].GetSTR()
#define CAMERA_MAX_RANGE(I)					g_TblCamera.m_ppVALUE[ I ][ 4 ].GetINT()
#define CAMERA_NEAR_PLANE(I)				g_TblCamera.m_ppVALUE[ I ][ 5 ].GetINT()
#define CAMERA_FAR_PLANE(I)					g_TblCamera.m_ppVALUE[ I ][ 6 ].GetINT()
#define CAMERA_NEAR_ALPHA_FOG(I)			g_TblCamera.m_ppVALUE[ I ][ 7 ].GetINT()
#define CAMERA_FAR_ALPHA_FOG(I)				g_TblCamera.m_ppVALUE[ I ][ 8 ].GetINT()
#define CAMERA_NEAR_FOG(I)					g_TblCamera.m_ppVALUE[ I ][ 9 ].GetINT()
#define CAMERA_FAR_FOG(I)					g_TblCamera.m_ppVALUE[ I ][ 10 ].GetINT()


///
/// Lod ���� �Ÿ� ����
///
#define LOD_APPEAR_MIN(I)					g_TblRangeSet.GetData( I , 1 )
#define LOD_APPEAR_MAX(I)					g_TblRangeSet.GetData( I , 2 )


///
/// ����...( LIST_UNION.STB )
///
#ifdef __SERVER
	#define UNION_NAME(I)						g_TblUnion.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define UNION_NAME(I)						CStringManager::GetSingleton().GetUnionName( I )
#endif
#define UNION_COLOR(I)						g_TblUnion.m_ppVALUE[ I ][ 1 ].GetINT()
#define UNION_MARK(I)						g_TblUnion.m_ppVALUE[ I ][ 2 ].GetINT()
#define UNION_HOSTILE_CNT					6
#define UNION_HOSTILE(I,C)					g_TblUnion.m_ppVALUE[ I ][ 3 + C ].GetINT();
#define UNION_STRING_ID(I)					g_TblUnion.m_ppVALUE[ I ][ 11 ].GetSTR()


///
/// ������( LIST_CLASS.STB )
///
#ifdef __SERVER
	#define CLASS_NAME(I)						g_TblClass.m_ppVALUE[ I ][ 0 ].GetSTR()
#else
	#define CLASS_NAME(I)						CStringManager::GetSingleton().GetClassName(I)
#endif
#define CLASS_INCLUDE_JOB_CNT				8///�ش� Class�� ���ԵǴ� �������� �� ����
#define CLASS_INCLUDE_JOB(I,C)				g_TblClass.m_ppVALUE[ I ][ 1 + C ].GetINT()	
#define CLASS_STRING_ID(L)					g_TblClass.m_ppVALUE[ L ][ 11 ].GetSTR()

// *--------------------------------------------------------------------------------* //


///
/// ���ڱ� �Ҹ�( LIST_STEPSOUND.STB )
///
#define STEPSOUND( ZONETYPE, TILENO )		g_TblStepSound.GetData( TILENO,[ ZONETYPE )





///
/// ��� ������ ��޺� ���� ��ġ...( LIST_GRADE.STB )
///
#define	ITEMGRADE_ATK(G)					g_TblItemGRADE.GetData( G , 0 )
#define	ITEMGRADE_HIT(G)					g_TblItemGRADE.GetData( G , 1 )
#define	ITEMGRADE_DEF(G)					g_TblItemGRADE.GetData( G , 2 )
#define	ITEMGRADE_RES(G)					g_TblItemGRADE.GetData( G , 3 )
#define	ITEMGRADE_AVOID(G)					g_TblItemGRADE.GetData( G , 4 )
#define	ITEMGRADE_GLOW_COLOR(G)				g_TblItemGRADE.GetData( G , 5 )

#define	ITEMGRADE_ATK_KRNEW(G)				g_TblItemGRADE.GetData( G , 0 ) 
#define	ITEMGRADE_ATKRATE_KRNEW(G)			( g_TblItemGRADE.GetData( G , 1 ) / 100.0f )
#define	ITEMGRADE_HIT_KRNEW(G)				g_TblItemGRADE.GetData( G , 2 ) 
#define	ITEMGRADE_HITRATE_KRNEW(G)			( g_TblItemGRADE.GetData( G , 3 ) / 100.0f )
#define	ITEMGRADE_DEF_KRNEW(G)				g_TblItemGRADE.GetData( G , 4 )
#define	ITEMGRADE_DEFRATE_KRNEW(G)			( g_TblItemGRADE.GetData( G , 5 ) / 100.0f )
#define	ITEMGRADE_RES_KRNEW(G)				g_TblItemGRADE.GetData( G , 6 )
#define	ITEMGRADE_RESRATE_KRNEW(G)			( g_TblItemGRADE.GetData( G , 7 ) / 100.0f )
#define	ITEMGRADE_AVOID_KRNEW(G)			g_TblItemGRADE.GetData( G , 8 )
#define	ITEMGRADE_AVOIDRATE_KRNEW(G)		( g_TblItemGRADE.GetData( G , 9 ) / 100.0f )
#define	ITEMGRADE_GLOW_COLOR_KRNEW(G)		g_TblItemGRADE.GetData( G , 10 )







extern STBDATA	g_TblHAIR;			//�Ӹ�
extern STBDATA	g_TblFACE;			//��

extern STBDATA	g_TblARMOR;			//���� ������
extern STBDATA	g_TblGAUNTLET;		//�尩 ������ 
extern STBDATA	g_TblBOOTS;			//�Ź� ������ 
extern STBDATA	g_TblHELMET;		//��� ������ 
extern STBDATA	g_TblWEAPON;		//���� ������
extern STBDATA	g_TblSUBWPN;		//�������� ������
extern STBDATA  g_TblFACEITEM;		//�� ������ 
extern STBDATA  g_TblBACKITEM;		//� �ٴ� ������ 
extern STBDATA  g_TblJEWELITEM;		//��ű� ������ 
extern STBDATA  g_TblGEMITEM;		//���� ������ 
extern STBDATA  g_TblNATUAL;		//����� 
extern STBDATA	g_TblUSEITEM;		//�Ҹ� ������ 


extern STBDATA  g_TblEFFECT;		//ȿ�� 
extern STBDATA	g_TblNPC;			//NPC
extern STBDATA	g_TblDropITEM;		//��� ������
extern STBDATA	g_TblAniTYPE;		//anitype
extern STBDATA  g_TblPRODUCT;		//���깰	

extern STBDATA  g_TblQUESTITEM;		//����Ʈ ������ 
extern STBDATA  g_TblStore;			//���� 

extern STBDATA	g_TblWARP;			// ���� !
extern STBDATA	g_TblEVENT;			// �̺�Ʈ

extern STBDATA	g_TblSKY;

extern STBDATA	g_TblZONE;

extern STBDATA *g_pTblSTBs[ ITEM_TYPE_RIDE_PART+1 ];



extern STBDATA	g_TblString;		// String table
extern STBDATA	g_TblHitSound;		// ������ ���� Ÿ�ݽ��� ����

extern STBDATA	g_TblAVATAR;		/// �ƹ�Ÿ �ʱ� ���� ����Ÿ..
extern STBDATA  g_TblResolution;	/// �ɼ�-�ػ�
extern STBDATA	g_TblFoulWord;		/// ä�ý� Filtering�� �ܾ���� ����Ʈ 


extern STBDATA  g_TblSTATE;			/// ĳ������ ���¸� �����ϴ� ����.

extern STBDATA  g_TblCamera;		/// �þ߰��� ����..
extern STBDATA  g_TblRangeSet;		/// LOD �Ÿ� ���� ����..

extern STBDATA  g_TblUnion;
extern STBDATA	g_TblClass;
extern STBDATA  g_TblStepSound;		/// ���ڱ��Ҹ�..

extern STBDATA	g_TblItemGRADE;		/// ��� �����ۺ� ��� ���� ��ġ

extern STBDATA	g_TblATTR;
extern STBDATA g_TblBreak;					//�ѱ������� ������ ����..

extern STBDATA	g_TblUPGRADE;		/// ��� - ������
extern STBDATA		g_TblBadNames;


#ifndef __SERVER
	extern STBDATA      g_TblHELP; 
//	extern STBDATA		g_TblBadNames;
#endif
//-------------------------------------------------------------------------------------------------
#endif
