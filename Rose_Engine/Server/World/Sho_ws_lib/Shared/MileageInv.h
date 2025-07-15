#pragma once

#include "CItem.h"
#pragma warning (disable:4201)

#pragma pack (push, 1)

class CMileageInv
{
public:
	union
	{
		MileageItem		m_ItemLIST[ MAX_MILEAGE_INV_SIZE ];
		struct 
		{
			MileageItem	m_ItemEQUIP[ MAX_MILEAGE_EQUIP	];		// ���� ���..
			MileageItem	m_ItemPAGE [ INVENTORY_PAGE_SIZE ];    //������ ���� ����..
		} ;
	} ;	
public:
	CMileageInv(void){};
	~CMileageInv(void){};

	short	AppendITEM (MileageItem &sITEM, short &nCurWEIGHT);
	short	AppendITEM (short nListNO, MileageItem &sITEM, short &nCurWEIGHT);
	void	DeleteITEM  (WORD wListRealNO);
	short	FindITEM (MileageItem &sITEM);

	void	SubtractITEM (short nLitNO, MileageItem &sITEM, short &nCurWEIGHT);

	short GetWEIGHT (short nListNO);
	void  SetInventory (short nListNO, int iItemNO, int iQuantity=1);

	short GetEmptyInventory();	
	short GetEmptyInvenSlotCount( );
};

class CPatsEquipInv
{
	MileageItem		m_ItemLIST[ MAX_RIDING_PART ];
};
#pragma pack (pop)

//-------------------------------------------------------------------------------------------------
