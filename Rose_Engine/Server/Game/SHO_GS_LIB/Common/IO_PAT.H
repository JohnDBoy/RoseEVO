/**
 * \ingroup SHO_GS
 * \file	IO_pat.h
 * \brief	private addon technology ?? 데이타(list_pat.stb) 보관
 */
#ifndef	__CRIDEITEM_H
#define	__CRIDEITEM_H
#include "IO_STB.h"
//-------------------------------------------------------------------------------------------------

#define	PAT_ITEM_PART_IDX(I)				g_PatITEM.GetValue(I,  2)
#define	PAT_ITEM_USE_RIGHT(I)				g_PatITEM.GetValue(I,  3)

//	4~15번 컬럼은 공통 아이템에서...
#define	PAT_ITEM_TYPE(I)					g_PatITEM.GetValue(I,  4)

/*
#define	PAT_ITEM_BASE_PRICE(I)				g_PatITEM.GetValue(I,  5)
#define	PAT_ITEM_PRICE_RATE(I)				g_PatITEM.GetValue(I,  6)
#define	PAT_ITEM_WEIGHT(I)					g_PatITEM.GetValue(I,  7)
#define	PAT_ITEM_QUALITY(I)					g_PatITEM.GetValue(I,  8)

#define	PAT_ITEM_ICON_NO(I)					g_PatITEM.GetValue(I,  9)
#define	PAT_ITEM_FIELD_IMG(I)				g_PatITEM.GetValue(I, 10)
#define	PAT_ITEM_EQUIP_SOUND(I)				g_PatITEM.GetValue(I, 11)
*/

#define	PAT_ITEM_PART_TYPE(I)				g_PatITEM.GetValue(I, 16)
#define	PAT_ITEM_PART_VER(I)				g_PatITEM.GetValue(I, 17)

#define	PAT_ITEM_NEED_SKILL_IDX(I)			g_PatITEM.GetValue(I, 19)
#define	PAT_ITEM_NEED_SKILL_LEV(I)			g_PatITEM.GetValue(I, 20)

#define	PAT_ITEM_NEED_ABILITY_IDX(I)		g_PatITEM.GetValue(I, 21)
#define	PAT_ITEM_NEED_ABILITY_VALUE(I)		g_PatITEM.GetValue(I, 22)

#define PAT_ITEM_ADD_ABILITY_CNT			2
#define PAT_ITEM_ADD_ABILITY_IDX(I,C)		g_PatITEM.GetValue(I, 24 + C * 3 )
#define PAT_ITEM_ADD_ABILITY_VALUE(I,C)		g_PatITEM.GetValue(I, 25 + C * 3 )

#define	PAT_ITEM_MAX_FUEL(I)				g_PatITEM.GetValue(I, 31)
#define PAT_ITEM_USE_FUEL_RATE(I)			g_PatITEM.GetValue(I, 32)

#define	PAT_ITEM_MOV_SPD(I)					g_PatITEM.GetValue(I, 33)

#define PAT_ITEM_ATK_RANGE(I)				g_PatITEM.GetValue(I, 35)
#define	PAT_ITEM_ATK_POW(I)					g_PatITEM.GetValue(I, 36)
#define	PAT_ITEM_ATK_SPD(I)					g_PatITEM.GetValue(I, 37)

#define	PAT_IS_MAGIC_DAMAGE(I)				g_PatITEM.GetValue(I, 69)		// 일반/마법 데미지.
#define	PAT_ITEM_SEAT_CNT(I)				g_PatITEM.GetValue(I, 39)		// 추가 좌석수


//----------------------------------------------------------------------------------------
/// 팻, 팻 탑승 아바타의 동작 상태위치( 시작 위치 )
//----------------------------------------------------------------------------------------
#define PAT_RELATIVE_MOTION_POS(I)			g_PatITEM.GetValue(I, 40)
#define PAT_RELATIVE_MOTION_POS_AVATAR(I)	g_PatITEM.GetValue(I, 41)
	

//----------------------------------------------------------------------------------------
/// 상황에 따른 팻의 효과
//----------------------------------------------------------------------------------------
#define PAT_RIDE_EFFECT(I)					g_PatITEM.GetValue(I, 42)
#define PAT_RIDE_SOUND(I)					g_PatITEM.GetValue(I, 43)
#define PAT_GETOFF_EFFECT(I)				g_PatITEM.GetValue(I, 44)
#define PAT_GETOFF_SOUND(I)					g_PatITEM.GetValue(I, 45)
#define PAT_DEAD_EFFECT(I)					g_PatITEM.GetValue(I, 46)
#define PAT_DEAD_SOUND(I)					g_PatITEM.GetValue(I, 47)
#define PAT_STOP_SOUND(I)					g_PatITEM.GetValue(I, 48)		// 정지시 효과음
#define PAT_MOVE_EFFECT(I)					g_PatITEM.GetValue(I, 49)
#define PAT_MOVE_SOUND(I)					g_PatITEM.GetValue(I, 50)
	
#define PAT_ATTACK_EFFECT(I)				g_PatITEM.GetValue(I, 51)
#define PAT_ATTACK_SOUND(I)					g_PatITEM.GetValue(I, 52)
#define PAT_HIT_EFFECT(I)					g_PatITEM.GetValue(I, 53)
#define PAT_HIT_SOUND(I)					g_PatITEM.GetValue(I, 54)
	
#define PAT_BULLET(I)						g_PatITEM.GetValue(I, 55)	

#ifdef __KCHS_BATTLECART__
#define PAT_ITEM_HP(I)						g_PatITEM.GetValue(I, 67)		// PAT 체력
#define PAT_ITEM_EQUIP_REQUIRE_CLASS(I)		g_PatITEM.GetValue(I, 68)		// 사용 클래스
#define	PAT_DAMAGE_TYPE(I)					g_PatITEM.GetValue(I, 69)		// 일반/마법데미지 ???
#define	PAT_ABILITY_TYPE(I)					g_PatITEM.GetValue(I, 70)		// 의자 ???
#endif

//----------------------------------------------------------------------------------------
/// 이동, 정지 시 효과음, 효과
//----------------------------------------------------------------------------------------



//----------------------------------------------------------------------------------------
/// 카트의 더미본에 붙는 효과들
//----------------------------------------------------------------------------------------
#define PAT_ITEM_ATTACHED_EFFECT_CNT		8
#define PAT_ITEM_ATTACHED_EFFECT(I,T)		g_PatITEM.GetValue(I, 56 + T)		// 이동시 효과음

//----------------------------------------------------------------------------------------
///펫 아이템 분해 인덱스.
//----------------------------------------------------------------------------------------

	#define PAT_ITEM_BREAK_IDX(I)          g_PatITEM.GetValue(I,71)

//-------------------------------------
// 2006.10.23/김대성/추가 - 카트 정탄화 시스템, 펫 아이템 무기타입
#ifdef __BATTLECART2
#define PAT_ITEM_ARMS_TYPE(I)		g_PatITEM.GetValue(I, 74)		// 무기타입
#endif
//-------------------------------------


/**
 * \ingroup SHO_GS_LIB
 * \class	CPatITEM
 * \author	wookSang.Jo
 * \brief	personal addon technology item class
 */
class CPatITEM
{
private:

public :
	STBDATA		m_ItemDATA;

	CPatITEM ();
	~CPatITEM ();

	bool  LoadPatITEM(const char *szFileName);
	void  Free ();

	/// icarus:  : 2004. 6. 3 기준가격의 범위 초과 입력으로 short => int로 변환
	int GetValue(WORD wLine, WORD wCol)
	{
/*
		_ASSERT( wLine < m_ItemDATA.m_nDataCnt );

		return m_ItemDATA.m_ppDATA[ wLine ][ wCol ];
*/
		return m_ItemDATA.GetData(wLine,wCol);
	}
} ;
extern CPatITEM	g_PatITEM;

//-------------------------------------------------------------------------------------------------
#endif
