#ifndef TBASIS_TPROTOCOL
#define TBASIS_TPROTOCOL
#include <windows.h>
#pragma  pack(push, 1 )

typedef struct {
	WORD	len;	// msg 바이트 크기
	WORD	type;	// 어떤 패킷이다.
}PACKET_HEADER;

typedef struct {
	PACKET_HEADER	ph;
	char			msg[1024];
}UPACKET, *P_UPACKET;

// 서버
#define PACKET_CHAT_NAME_REQ 1000 // 대화명 입력해라.
// 클라이언트
#define PACKET_CHAT_NAME_ACK 1001 // 대화명 입력 전송.
// 보통 채팅 메세지
#define PACKET_CHAT_MSG 1002 // 대화명 입력 전송.

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_POSITION;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_PARTNER_MAKE;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_PARTNER_POSITION;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_LOGIN;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_MAKEROOM;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_ENTERTHEROOM;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_GAMESTART;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_GETROOMLISTINFO;

typedef struct {
	WORD	user_idx;
	WORD	posX;
	WORD	posY;
	WORD	direction; //0 ~7 8방향
}TPACKET_USER_GETTHEROOMINFO;

#define PACKET_USER_POSITION				3000
#define PACKET_USER_PARTNER_MAKE			3001
#define PACKET_USER_PARTNER_POSITION		3002
#define PACKET_USER_LOGIN					4000
#define PACKET_USER_MAKEROOM				5000
#define PACKET_USER_ENTERTHEROOM			6000
#define PACKET_USER_GAMESTART				7000
#define PACKET_USER_GETROOMLISTINFO			8000
#define PACKET_USER_GETTHEROOMINFO			9000


#pragma pack(pop)

#endif