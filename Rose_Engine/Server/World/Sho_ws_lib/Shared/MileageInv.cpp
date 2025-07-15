#include "IO_STB.h"
#include <crtdbg.h>
#include ".\mileageinv.h"


short CMileageInv::GetWEIGHT (short nListNO)
{
	MileageItem *pITEM = &this->m_ItemLIST[ nListNO ];

	if ( 0 == pITEM->GetTYPE() ) {
		return 0;
	}

	if ( pITEM->IsEnableDupCNT() ) {
		return ITEM_WEIGHT( pITEM->m_cType, pITEM->m_nItemNo ) * pITEM->GetQuantity();
	}

	return ITEM_WEIGHT( pITEM->m_cType, pITEM->m_nItemNo );
}

//-------------------------------------------------------------------------------------------------
// iItemNO == 03005 (무사수련복) 
void CMileageInv::SetInventory (short nListNO, int iItem, int iQuantity)
{
	if ( 0 == iItem )
		return;

	MileageItem sITEM;
	sITEM.Init( iItem, iQuantity );
	
	//m_ItemLIST[ nListNO ] = sITEM;
	CopyMemory(&m_ItemLIST[nListNO],&sITEM,sizeof(MileageItem));
}


//-------------------------------------------------------------------------------------------------
/// Server function
short CMileageInv::AppendITEM (MileageItem &sITEM, short &nCurWeight)
{
	_ASSERT( sITEM.GetTYPE() );

	if ( sITEM.IsEmpty()||!sITEM.GetMileageType() ) 
	{
		return -1;
	}

// 마일리지 아이템은 중복될수 없다..
/*
	if ( sITEM.IsEnableDupCNT() ) 
	{
		for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++) 
		{
			// 같은 기타 아이템이라도 아이템 타입이 틀린것들로 구성됨...
			if ( this->m_ItemPAGE[ nI ].GetHEADER() == sITEM.GetHEADER() ) 
			{
				if ( this->m_ItemPAGE[ nI ].GetQuantity() + sITEM.GetQuantity() <= MAX_DUP_ITEM_QUANTITY ) 
				{
					// 더했을 경우 최대 갯수가 넘어 가면 새 스롯에다 할당.
					nCurWeight += ( ITEM_WEIGHT( sITEM.m_cType, sITEM.m_nItemNo ) * sITEM.GetQuantity() );

					this->m_ItemPAGE[ nI ].m_uiQuantity += sITEM.GetQuantity();
					sITEM.m_uiQuantity = this->m_ItemPAGE[ nI ].GetQuantity();

					_ASSERT( MAX_MILEAGE_INV_SIZE > nI+MAX_MILEAGE_EQUIP+INVENTORY_PAGE_SIZE);

					return nI+MAX_MILEAGE_EQUIP+INVENTORY_PAGE_SIZE;
				}
			}
		}
	}
*/
	// 중복될수 없는 장비아이템, PAT아이템 이나 보유한 아이템중에 같은 아이템이 없는 경우...빈슬롯 할당...
	short nInvIDX = GetEmptyInventory(  );
	if ( nInvIDX >= 0 ) {
		AppendITEM ( nInvIDX, sITEM, nCurWeight );
		return nInvIDX;
	}

	return -1;
}

short CMileageInv::AppendITEM (short nListRealNO, MileageItem &sITEM, short &nCurWeight)
{
	_ASSERT( sITEM.GetTYPE() );

	if ( sITEM.IsEmpty() ) {
		return -1;
	}

	if ( nListRealNO >= MAX_MILEAGE_INV_SIZE )
	{
		return -1;
	}

// 마일리지 아이템은 무게가없다...


	// 돈을 제외한 아이템은 서버에서 받은 아이템으로 교체...
	if ( m_ItemLIST[ nListRealNO ].m_dwITEM ) {
	}

	CopyMemory(&m_ItemLIST[ nListRealNO ],&sITEM,sizeof(MileageItem));
	//m_ItemLIST[ nListRealNO ] = sITEM;



#ifndef	__SERVER	// 서버에선 사용 안함.
	/// 추가된 아이템에 대한 Loopup table 갱신
	MakeItemIndexList();	
	g_pAVATAR->m_HotICONS.UpdateHotICON();
#endif

	return nListRealNO;
}


//-------------------------------------------------------------------------------------------------
/// Real Index 로 아이템을 슬롯에서 비운다.
void CMileageInv::DeleteITEM(WORD wListRealNO)
{
	m_ItemLIST[ wListRealNO ].Clear();

#ifndef	__SERVER	// 서버에선 사용 안함.
	short nLookUpIndex = GetLookupIndexFromRealIndex( wListRealNO );
	if( nLookUpIndex < 0 )
	{
		//_ASSERT( 0 && "nLookUpIndex < 0" );
		return;
	}

	m_btIndexLIST[ nLookUpIndex ] = 0; 

#endif	
}

//-------------------------------------------------------------------------------------------------
// 인벤토리에서 sITEM을 뺀후 nCurWEIGHT를 갱신한다.
void CMileageInv::SubtractITEM (short nListNO, MileageItem &sITEM, short &nCurWEIGHT)
{

	if ( m_ItemLIST[ nListNO ].GetTYPE() ) {
		nCurWEIGHT -= m_ItemLIST[ nListNO ].Subtract( sITEM );
	}
}


short CMileageInv::FindITEM (MileageItem &sITEM)
{

	for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++) {
		if ( this->m_ItemPAGE[ nI ].GetHEADER() == sITEM.GetHEADER() ) {
			return nI+MAX_MILEAGE_EQUIP+(INVENTORY_PAGE_SIZE);
		}
	}
	return -1;
}

//-------------------------------------------------------------------------------------------------
/// @bug m_dwITEM == 0 으로만 이 슬롯이 비었다고 할수 있는가?
short CMileageInv::GetEmptyInventory()
{
	for (short nI=0 ; nI < INVENTORY_PAGE_SIZE ; nI++ ) 
	{	
		if( m_ItemPAGE[ nI ].m_cType == 0 ) 
		{
			return nI + MAX_EQUIP_IDX ;
		}
	}

	return -1;
}


//-------------------------------------------------------------------------------------------------
short CMileageInv::GetEmptyInvenSlotCount( )
{
	short nCount = 0;


	for( short nl =  0; nl < INVENTORY_PAGE_SIZE; ++nl )
	{
		if( m_ItemPAGE[ nl ].GetTYPE() == 0 )
			++nCount;
	}
	return nCount;
}