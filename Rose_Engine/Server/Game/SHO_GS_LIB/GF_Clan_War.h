
/*
	작성자 김영환
*/

//#ifdef __CLAN_WAR_SET			// 클랜전 추가 정의

/*
	클랜전 관리/처리 함수
*/

////	클랜전 정보 초기화
BOOL	GF_Clan_War_Init();

////	클랜전 정보 ini 읽기
BOOL	GF_Clan_War_Ini_Read();

////	클랜전 필드 보스 몹 초기화
BOOL	GF_CW_MOB_Init();

////	클랜전 필드 확인
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N);

////	클랜전 필드 팀 확인.
DWORD	GF_Clan_War_Zone_Is(DWORD p_Zone_N,DWORD p_Clan_N);

////	클랜전 존 게임중인 확인 [2차 클랜전 보강]
BOOL	GF_Clan_War_Zone_Play_Is(DWORD p_Zone_N);

////	클랜전 만들어진 방 찾기
DWORD	GF_Find_CWar_List(LPVOID p_Npc,DWORD p_Type);

////	클랜전 만들어진 방 찾고 : 해당 방에 이긴 팀 설정. : 사용안함.
DWORD	GF_Find_CWar_List2(DWORD p_Clan_ID);

////	클랜전 방의 타입 얻기..
DWORD	GF_Get_CWar_Type(DWORD	p_Type);

////	클랜전 방의 배팅 금액 설정
__int64 GF_Set_CWar_Bat(DWORD p_Clan_Inx,DWORD p_Type);

////	클랜전 신청 취소에 대한 보상 처리...
BOOL	GF_CWar_Bat_Return(LPVOID p_User);

////	마스터 유저의 금액을 뺀다.
BOOL	GF_Clan_Master_Money_Sub(DWORD p_Clan_Inx,__int64 p_Money);

////	마스터 유저의 금액을 추가.
BOOL	GF_Clan_Master_Money_Add(DWORD p_Clan_Inx,__int64 p_Money);

////	마스터 유저의 클랜전 변수 변경.
BOOL	GF_Clan_Master_ClanWar_IDX(DWORD p_Clan_Inx,WORD Num,bool b_WinType,DWORD p_WarType);

////	지정 유저의 클랜전 보상변수 확인
BOOL	GF_Clan_Master_ClanWar_Gift(LPVOID p_User);

////	승리한 클랜윈 처리
BOOL	GF_Win_CWar_Set(LPVOID p_Room,DWORD p_Type,bool b_WinType);

////	클랜전 드롭 처리
BOOL	GF_Clan_War_Drop(DWORD p_List_N,DWORD p_Team);

////	클랜전 빈방 찾기
DWORD	GF_Find_CWar_Void(LPVOID p_Npc);

////	클랜전 방 지우기
BOOL	GF_Del_CWar_List(LPVOID p_Npc);
BOOL	GF_Del_CWar_List2(DWORD	p_Num);

////	클랜전에 신청 취소 처리
BOOL	GF_Clan_War_Cancel(DWORD	p_Num);

////	클랜전 신청 처리
BOOL	GF_Clan_War_Input(LPVOID p_Npc,DWORD p_Clan_Inx,DWORD p_LV, DWORD p_Type, LPVOID pClanUser);

////	클랜전 참여 LV 확인.
BOOL	GF_Clan_War_Check_LV(DWORD p_LV,DWORD p_Type);

////	클랜 리스트에서 찾기
DWORD	GF_Clan_Fine_List(DWORD p_Clan_ID);

////	클랜의 마스터를 찾는다.
DWORD	GF_Clan_Master_Fine(DWORD p_Clan);

////	클랜 마스터 하위 유저를 찾는다.
DWORD	GF_Clan_Master_Sub_Fine(DWORD p_Clan);

////	클랜전 등록 클랜 확인
BOOL	GF_ClanWar_Check_myClan(DWORD p_List_N,DWORD p_Clan_N);

////	클랜 소속 게임 유저에게 입장 확인 메시지 처리
BOOL	GF_ClanWar_Join_OK_Send(DWORD p_List_N,DWORD p_Type,DWORD p_Sub_Type);



////	클랜전 지역으로 유저 워프 시킴
BOOL	GF_ClanWar_Move(LPVOID p_User,DWORD p_List_N,DWORD p_Team_N);

////	클랜전 종료 위치로 워프
BOOL	GF_ClanWar_Out_Move(LPVOID p_User);

////	클랜전 공간에 몹 설정 하기
BOOL	GF_ClanWar_Mob_Set(DWORD p_List_N);

////	클랜전 공간에 몹 지우기
BOOL	GF_ClanWar_Mob_Del(DWORD p_List_N,DWORD p_Type);
BOOL	GF_ClanWar_Mob_Del2(LPVOID p_Room,DWORD p_Type);

////	해당 팀의 몬스터 체력을 회복한다.
BOOL	GF_ClanWar_MOB_HP_ReSet(LPVOID p_Room,DWORD p_Type);

////	클랜전 보스몹 잡음 메시지 처리 [이건 트리거 기준으로 처리하는 방식]
BOOL	GF_ClanWar_Boss_MOB_Kill(DWORD List_N,DWORD p_idx);

////	클랜전 보스몹이 죽었는지 체크 [2차 클랜전 보강 : 서버에서 보스 죽었는지 체크]
BOOL	GF_ClanWar_Boss_Kill_Check(LPVOID p_Boss,LPVOID p_User);

////	HP 전송을 위한 클랜전 보스 몹인지 확인한다. [2차 클랜전 보강]
DWORD	GF_ClanWar_Boss_MOB_Is(LPVOID p_Boss);

////	클랜전 보스몹 HP 상태 전송.	 [2차 클랜전 보강]
BOOL	GF_ClanWar_Boss_MOB_HP(DWORD p_List_N);

////	클랜전 퀘스트 삭제
BOOL	GF_ClanWar_Quest_Del(LPVOID p_User,DWORD p_Quest_Idx);

////	클랜전 퀘스트 등록
BOOL	GF_ClanWar_Quest_Add(LPVOID p_User,DWORD p_Quest_Idx);

////	클랜전 PK 모드 전투 확인
BOOL	GF_ClanWar_PK_Is(LPVOID p_User);

////	클랜전 참여 팀의 퀘스트 처리 함수
BOOL	GF_ClanWar_Quest_DropProc(DWORD p_List_N,DWORD p_Team);

////	클랜전 포기 처리 메시지
BOOL	GF_ClanWar_GiveUP(DWORD p_List_N,DWORD p_Team);

////	클랜전 참여중인 클랜 확인 함수
DWORD	GF_Get_CLANWAR_Check(DWORD p_Clan_N,DWORD p_User_LV);

////	클랜전 참여 유저에 진행 메시지 전송한다.
BOOL	GF_Send_CLANWAR_Progress(DWORD p_List_N,DWORD p_Type);


////	클랜전 부활 위치 얻기
BOOL	GF_Get_ClanWar_Revival_Pos(DWORD p_List_N,DWORD pTeam,float &Out_x,float &Out_Y);

////	클랜전 맵에 GATE NPC 조정 처리
BOOL	GF_Set_ClanWar_Gate_NPC(LPVOID p_User,DWORD p_List_N,BOOL p_Type);

////	클랜전 참여 인원 증가
BOOL	GF_Add_ClanWar_Number(DWORD p_List_N,DWORD pTeam);

////	클랜전 참여 인원 감소
BOOL	GF_Sub_ClanWar_Number(DWORD p_List_N,DWORD pTeam);

////	클랜전 포인트 처리 부분
BOOL	GF_Set_ClanWar_RankPoint(DWORD p_Type,LPVOID p_User_A,LPVOID p_User_B, void* vp_ClanWar_Set );//DWORD	WarType);

////	클랜 포인트 점수차이 얻기
float	GF_Get_Rank_Rate(int Ateam_P,int BTeam_P);

// (20070104:남병철) : 클랜 보상 포인트 적용
//BOOL	GF_Add_ClanWar_RewardPoint( int iClanID, int iRewardPoint );


//#endif