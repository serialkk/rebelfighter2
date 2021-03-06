#pragma once

typedef struct _raknet {
	RakNet::RakNetStatistics *rss;
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakNet::RakPeerInterface *client;
	//	client->InitializeSecurity(0,0,0,0);
	//RakNet::PacketLogger packetLogger;
	//client->AttachPlugin(&packetLogger);


	// Holds packets
	RakNet::Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Just so we can remember where the packet came from
	bool isServer;

	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID;

	// Crude interface

	// Holds user data
	char ip[64], serverPort[30], clientPort[30];
}st_RAKNET;

class CGameMulti : public CGameScene
{
public:
/////////////////////////////////////////////////////////////
// for Multi play


	HANDLE hThread;

	st_RAKNET m_raknet;

	int		m_iMultiPlayer;		//멀티플레이어 유저 번호. 1: 1p, 2: 2p,  0: 접속되지 않음. 3: 접속자수 2명이상이라 서버로 접속 끊음.
	int     m_iMultiPlayerCount;//멀티플레이 총 유저 수
	bool	m_bMultiPlaying;	//멀티플레이 실행중인지?
	void	Send_ID_USER_LASER_FIRE(float fPosX, float fPosY);
	void	Send_ID_USER_LASER_MOVE(unsigned short id, float fPosX, float fPosY);
	void	Send_ID_USER_MOVE(float fPosX, float fPosY, int iDirection);

	void	PrintNetworkConnectStatus();
	void	Send_ID_DISCONNECTION_NOTIFICATION();
/////////////////////////////////////////////////////////////
	INT Init();
	void Frame();
	void Render(LPDIRECT3DDEVICE9& dxdevice, LPD3DXSPRITE& dxsprite);
	void Destroy();




public:
	void MultiInit();						//멀티플레이관련 init
	void NextStageCreate();						//다음스테이지 생성관련 처리
	void EnemyBullet();							//적 총알 발사 처리
	void ProcessSideBar();						//옆에 상태바 출력처리용

	std::vector<CLaserData*>		pvLaser0;	//주인공 총알 벡터 컨테이너
	std::vector<CLaserData*>		pvLaser1;	//적 총알 벡터 컨테이너
	std::vector<CCharacterData*>	pvTie0;	    //적 케릭터 벡터 컨테이너


	RECT				m_ImgRc2;				// RECT 애니 이미지

	DWORD				m_dTimeBegin;			// 시작 타임
	DWORD				m_dTimeEnd;				// 끝 타임



	char phpBuf[80], scoreBuf[80], stageBuf[80], lifeBuf[80]
		, playerabilBuf[80], vectorsize[80], killcount[80], timeBuf[80]
		, ioncntBuf[80], lasercntBuf[80], missilecntBuf[80]
		, curioncntBuf[80], curlasercntBuf[80], curmissilecntBuf[80], enemyBuf[80]
		, speedBuf[80], powerBuf[80], enemylevelBuf[80];

	TCHAR	fpsBuf[128];

	int    ioncnt;
	int		lasercnt;
	int		missilecnt;
	int		curioncnt;
	int		curlasercnt;
	int		curmissilecnt;

	int		stage;			//처음 시작시 스테이지 1
	int		curstage;
	int		score;			//점수
	int		enemylevel;	//적 레벨 계산용 변수

						//시간계산용
	int		hh;	//시
	int		mm;	//분
	int		ss;	//초



	CPlayerData m_xwing_1p;
	CPlayerData m_xwing_2p;


	D3DXVECTOR3 vcPos1;						//배경위치
	D3DXVECTOR3 vcPos2;						//배경위치
	D3DXVECTOR3 vcBar;						//상태바위치
	D3DXVECTOR3 vcLpos0;					//주인공총알위치0
	D3DXVECTOR3 vcLpos1;					//적총알위치0
	
	D3DXVECTOR3 boom;						//폭발위치



	INT ColCheck();		//충돌체크 함수1: 적총알이 주인공에 맞았는지 체크
	INT ColCheck2();	//충돌체크 함수2: 주인공총알이 적에 맞았는지 체크
	INT ColCheck3();	//충돌체크 함수3: 적과 주인공의 충돌 체크

						////////////////////////////////////////////////////////////////////////////////
						//메뉴별로 게임 메뉴, 게임 실행, 스코어 


	void	CharacterMovePath(std::vector<CCharacterData*>::iterator* _FT);
	void	CharacterMoveBasic(std::vector<CCharacterData*>::iterator* _FT);

	void	InputMove();
	void	CharacterMove();


public:
	CGameMulti();
	virtual ~CGameMulti();
};

extern CGameMulti*	g_pGameMultiplay; //외부 연결 범위 선언