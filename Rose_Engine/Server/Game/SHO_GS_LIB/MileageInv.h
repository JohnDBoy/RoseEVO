#pragma once

#include "CItem.h"
#pragma warning (disable:4201)

#pragma pack (push, 1)

class CMileageInv
{
public:
	union
	{
		tagITEM		m_ItemLIST[ MAX_MILEAGE_INV_SIZE ];
		struct 
		{
			tagITEM	m_ItemEQUIP[ MAX_MILEAGE_EQUIP	];		// 착용 장비..
			tagITEM	m_ItemPAGE [ INVENTORY_PAGE_SIZE ];    //아이템 저장 영역..
		} ;
	} ;	
public:
	CMileageInv(void);
	~CMileageInv(void);

	short	AppendITEM (tagITEM &sITEM, short &nCurWEIGHT);
	short	AppendITEM (short nListNO, tagITEM &sITEM, short &nCurWEIGHT);
	void	DeleteITEM  (WORD wListRealNO);
	short	FindITEM (tagITEM &sITEM);

	void	SubtractITEM (short nLitNO, tagITEM &sITEM, short &nCurWEIGHT);

	short GetWEIGHT (short nListNO);
	void  SetInventory (short nListNO, int iItemNO, int iQuantity=1);

	short GetEmptyInventory();	
	short GetEmptyInvenSlotCount( );



};
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)