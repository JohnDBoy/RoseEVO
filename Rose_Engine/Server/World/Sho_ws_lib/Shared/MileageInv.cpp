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
// iItemNO == 03005 (������ú�) 
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

// ���ϸ��� �������� �ߺ��ɼ� ����..
/*
	if ( sITEM.IsEnableDupCNT() ) 
	{
		for (short nI=0; nI<INVENTORY_PAGE_SIZE; nI++) 
		{
			// ���� ��Ÿ �������̶� ������ Ÿ���� Ʋ���͵�� ������...
			if ( this->m_ItemPAGE[ nI ].GetHEADER() == sITEM.GetHEADER() ) 
			{
				if ( this->m_ItemPAGE[ nI ].GetQuantity() + sITEM.GetQuantity() <= MAX_DUP_ITEM_QUANTITY ) 
				{
					// ������ ��� �ִ� ������ �Ѿ� ���� �� ���Կ��� �Ҵ�.
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
	// �ߺ��ɼ� ���� ��������, PAT������ �̳� ������ �������߿� ���� �������� ���� ���...�󽽷� �Ҵ�...
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

// ���ϸ��� �������� ���԰�����...


	// ���� ������ �������� �������� ���� ���������� ��ü...
	if ( m_ItemLIST[ nListRealNO ].m_dwITEM ) {
	}

	CopyMemory(&m_ItemLIST[ nListRealNO ],&sITEM,sizeof(MileageItem));
	//m_ItemLIST[ nListRealNO ] = sITEM;



#ifndef	__SERVER	// �������� ��� ����.
	/// �߰��� �����ۿ� ���� Loopup table ����
	MakeItemIndexList();	
	g_pAVATAR->m_HotICONS.UpdateHotICON();
#endif

	return nListRealNO;
}


//-------------------------------------------------------------------------------------------------
/// Real Index �� �������� ���Կ��� ����.
void CMileageInv::DeleteITEM(WORD wListRealNO)
{
	m_ItemLIST[ wListRealNO ].Clear();

#ifndef	__SERVER	// �������� ��� ����.
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
// �κ��丮���� sITEM�� ���� nCurWEIGHT�� �����Ѵ�.
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
/// @bug m_dwITEM == 0 ���θ� �� ������ ����ٰ� �Ҽ� �ִ°�?
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