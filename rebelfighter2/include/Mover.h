﻿#pragma once
#include "Object3D.h"
#include "Input.h"

#define MAX_SPAWNS 20
#define MAX_HISTS 40
#define MAX_LOCKON_LASERS 10
#define MAX_STOCK 10

class CMover;
class CMoverGroup;
class CMoverGroupManager;
class CBgMover;
class CJointMover;


// 그룹
typedef enum {
	MYSHIP_GROUP, WEAPON_GROUP, ENEMY_GROUP, BULLET_GROUP, STAGE_GROUP,
	GROUND_GROUP, ITEM_GROUP, WATER_GROUP, BARRIER_GROUP, GROUND_ENEMY_GROUP,
	BG_GROUP, JOINT_GROUP,
	NUM_GROUPS
} GROUP_ID;

// 무기
typedef enum {
	WP_BASIC_SHOT, WP_AUTO_SHOT, 
	WP_POWER_SHOT, WP_SEMI_AUTO_SHOT, WP_POWER_SHOT_RELEASED,
	WP_SHOT_AND_LASER, WP_LOCK_SHOT, WP_COMMAND_SHOT,
	WP_GROUND_MISSILES, WP_REFLECT_SHOT, WP_REFLECT_LASER
} WEAPON_ID;

// 목표물
typedef enum {
	TG_RANDOM_MYSHIP, TG_LOCKED_ENEMY, TG_MOVER, TG_RANDOM_ENEMY
} TARGET_TYPE;

// 보행
typedef enum {
	WS_WALKING, WS_JUMP_UP, WS_JUMP_DOWN
} WALK_STATE;

// 버서크
typedef enum {
	BS_NORMAL, BS_BERSERK, BS_WEAK
} BERSERK_STATE;

// 집게
typedef enum {
	AS_READY, AS_EXTENDING, AS_EXTENDED, AS_SHRINKING
} ARM_STATE;

// 자유롭게 움직일 수 있는 조준
typedef enum {
	MSS_NORMAL, MSS_SIGHT, MSS_ATTACK
} MOVABLE_SIGHT_STATE;

// 메인 캐릭터를 사로잡을 수 있는 적기
typedef enum {
	CPT_NORMAL, CPT_CAPTURED
} CAPTOR_STATE;

// 궤적
typedef struct {
	float VX, VY;
	int Time;
} ORBIT;


//=============================================================
// 커맨드

// 스틱과 버튼 상태를 나타내는 상수:
#define CMD_NONE      0  // 초기 상태
#define CMD_NEUTRAL   1  // 중립
#define CMD_UP        2  // 상
#define CMD_DOWN      4  // 하
#define CMD_LEFT      8  // 좌
#define CMD_RIGHT    16  // 우
#define CMD_BUTTON0  32  // 버튼0
#define CMD_BUTTON1  64  // 버튼1
#define CMD_BUTTON2 128  // 버튼2
#define CMD_BUTTON3 256  // 버튼3

// 입력 이력의 숫자，커맨드의 최대 개수
#define NUM_CMD_HIST 60
#define MAX_CMD 10

// 커맨드:
// 커맨드 길이，입력 제한 시간，입력 내용
// 발사되는 무기
typedef struct {
	int Length, Limit, Input[NUM_CMD_HIST];
	CMover* Model;
} COMMAND;

// 커맨드 리스트
typedef struct {
	int NumCommand;
	COMMAND Command[MAX_CMD];
} COMMAND_LIST;

// 지형과의 충돌
typedef enum {
	GR_HIT, GR_ALONG, GR_REFLECT
} GROUND_HIT;

// Spawn/Next에서 계승할 속성
#define IH_YAW 1


//=============================================================
// 이동 물체 관련

// 이동 물체
class CMover {
friend CMoverGroup;
friend CBgMover;
friend CJointMover;

protected:

	CMoverGroup* Group;
	CMover* Parent;

	bool Used;
	bool Dead;

	float X, Y, Z, XRand, YRand, RX, RY;
	float VX, VY, Speed, SpeedRnd;
	float Angle, AngleRnd;
	float AX, AY, Accel;
	CObject3D* Obj3D;
	int Color;

	int Inherit;

	typedef struct { 
		bool 
			FPN, DDA, INTG, Aiming, Directed, Looping,
			MyShip, Roll, Yaw, Pos, Homing, Scale,
			Lock, Target,
			Throw, Arm, RFShot, ChangeColor, Reflect, Collision, Graze,
			FollowParent, Slow, Quick, MovableSightB, MovableSightS,
			Captee, Captor, Orbit, Tentacle, Joint, Bg, 
			LimitedScrollX, FreeScrollY, Gravity, ControlScrollY,
			BgRotate, Star,
			Berserk, LockOnLaser, Laser, Weapon, Warp, SCButton, SCItem,
			Barrier, Next, GroundHit, SightedBomb, 
			Docking, TurnMarker, Option, Walk, SAItem, Punch,
			Underwater, Transform;
	} VALID;
	VALID Valid;

	// 변신
	typedef struct {
		CMover *Model, *Effect;
		int ButtonID;
		bool PrevButton;
	} TRANSFORM;
	TRANSFORM Transform;

	// 스크롤 속도 조절
	typedef struct {
		float YTop, YBottom, SVYTop, SVYBottom;
	} CONTROL_SCROLL_Y;
	CONTROL_SCROLL_Y ControlScrollY;

	// 중력
	struct {
		float VX, VY;
	} Gravity;

	// 배경
	struct {
		CObject3D** Chip;
		int* Map;
		int MW, MH;
		float SX, SY, SW, SH, CW, CH, SVX, SVY;
	} Bg;

	struct {
		float Angle;
	} BgRotate;

	// 궤적
	struct {
		int Index, Count, Time;
		ORBIT* Data;
		float XMul, YMul;
		bool Repeat;
	} Orbit;

	// 메인 캐릭터를 사로잡을 수 있는 적기
	struct {
		CMover *Attack;
		CAPTOR_STATE State;
		CObject3D *MyShip;
	} Captor;

	// 적기에 사로잡힌 메인 캐릭터
	struct {
		bool Doubled;
	} Captee;

	// 자유롭게 움직일 수 있는 조준(스틱)
	typedef struct {
		float X, Y, Speed;
		CMover *SightMover, *SightModel;
	} MOVABLE_SIGHT_S;
	MOVABLE_SIGHT_S MovableSightS;

	// 자유롭게 움직일 수 있는 조준(버튼)
	typedef struct {
		int ButtonID, State;
		float X, Y, Speed;
		CMover *Mover, *SightModel, *AttackModel;
	} MOVABLE_SIGHT_B;
	MOVABLE_SIGHT_B MovableSightB;

	// 슬로우, 퀵
	struct {
		int ButtonID, Speed;
	} Slow;
	struct {
		int ButtonID, Speed;
	} Quick;

	// 스치기
	struct {
		bool Grazing;
		CMover* Effect;
	} Graze;

	// 충돌
	struct {
		int MinEnergy, Energy;
		CMover* Crash;
	} Collision;

	// 반사
	struct {
		int ButtonID;
		CMover *FieldModel, *FieldMover, *ShotModel;
	} Reflect;

	// 색 바꾸기
	struct {
		int ButtonID, NumColor;
		CObject3D** Obj3D;
		CMover** Shot;
		CMover** Effect;
		CHit* Hit;
		bool PrevButton;
	} ChangeColor;

	// 샷의 강화
	struct {
		int Mode;
		CMover* Shot;
		CHit* Hit;
	} RFShot;

	// 적기를 집어서 던지기
	struct {
		int ButtonID, Length, MaxLength, State, Time, MaxTime,
			NumStock, MaxStock;
		CObject3D *Head, *Tail, *Stock[MAX_STOCK];
		float Speed;
		bool Captured;
	} Arm;

	// 아군을 집어서 던지기
	struct {
		CHit* Hit;
		int ButtonID, Time, MaxTime;
		float VX, VY, VYaw;
		bool Thrown;
	} Throw;

	// 버서크
	struct {
		CMover *EffectModel, *EffectMover;
		int Energy, ReqEnergy, Time, MaxTime, WarnTime, WeakTime;
		BERSERK_STATE State;
	} Berserk;

	// 아이템에 의한 특수공격
	struct {
		CMover *ShotModel, *EffectModel, *EffectMover;
		int Time, MaxTime, WarnTime;
	} SAItem;

	// 근접 공격
	struct {
		CMover *Model, *Mover;
		int ButtonID, Time, MaxTime;
	} Punch;

	// 보호막
	struct {
		CMover *Model, *Mover;
		float WarnVit;
	} Barrier;

	// 수중
	struct {
		float Speed, AirH, WaterH;
		CMover* Wave;
	} Underwater;

	// 보행
	struct {
		float GVX, GVY, JVX, JVY;
		int Time, JumpTime, ButtonID;
	} Walk;

	// 합체
	struct {
		bool Docked;
		CObject3D* Obj3D;
		CMover* Shot;
	} Docking;

	// 아이템의 효과
	struct {
		float Accel;
		bool Docking, SpecialAttack;
	} ItemEffect;

	// 옵션
	struct {
		int Count, Interval;
		CObject3D* Obj3D;
	} Option;

	// 워프
	struct {
		bool Released;
		int Input, Time, TimeLimit;
		float Distance;
		CMover* Effect;
	} Warp;

	// 아이템에 의한 스피드 조절
	struct {
		float Accel, MinSpeed, MaxSpeed;
	} SCItem;

	// 버튼에 의한 스피드 조절
	struct {
		float Accel, MinSpeed, MaxSpeed;
		bool PrevButton;
		int ButtonID;
	} SCButton;

	// 지면과의 충돌처리(미사일 등)
	struct {
		GROUND_HIT ModeX, ModeY;
		bool HitX, HitY;
	} GroundHit;

	// 목표물
	struct {
		TARGET_TYPE Type;
		int ID;
		CMover* AlterModel;
		CMover* Mover;
	} Target;

	// 알파
	struct {
		float X, V;
		bool Add;
	} Alpha;

	// 확대, 축소
	struct {
		float X, Y, VX, VY;
	} Scale;

	// 다음 상태
	struct {
		CMover* Model;
		int Timer;
	} Next;
	void GoNext(CMover* model);

	// 고정 소수
	struct {
		int X, Y, VX, VY;
	} Fpn;
	void InitAimingFPN(float ftx, float fty, float fspeed);

	// DDA
	struct {
		int X, Y, VX, VY, DX, DY, Speed, Diff;
	} Dda;
	void InitAimingDDA(float ftx, float fty, float fspeed);
	void InitDirectedDDA(float angle, float speed);
	void MoveDDA();

	// 16방향 속도 3
	struct {
		int X, Y, VX, VY;
	} Intg;
	void InitDirected16_3(float angle);

	// 조준탄，방향탄
	void InitAiming(float tx, float ty, float speed, float angle);
	void InitDirected(float speed, float angle);

	// 회전
	struct {
		float VAngle, AAngle;
		float Radius, VRadius, ARadius;
		CMover* Target;
	} Looping;
	void MoveLooping();

	// 이전 좌표
	int HistIndex;
	typedef struct {
		float X, Y, Yaw, Roll;
		bool Valid;
	} HIST;
	HIST Hists[MAX_HISTS];

	// 꼬리
	CMover* Prec;
	int PrecDelay;
	HIST PrecHist;
	int RefCount;

	// 레이저
	typedef struct {
		int Length;
		float VAlpha;
		bool AddAlpha;
		CObject3D* TailObj3D;
		bool TailAddAlpha;
		bool TailDamageTransparent;
	} LASER;
	LASER Laser;
	void InitLaser(CMover* model, CMover* parent);

	// 생성
	int NumSpawns;
	typedef struct {
		CMover* Model;
		float RX, RY;
		int Timer, Cycle, Count;
		int NWayCount;
		float NWayAngle;
		int CircleCount;
		bool CircleOdd;
	} SPAWN;
	SPAWN Spawns[MAX_SPAWNS];
	void SpawnNWay(SPAWN* spawn);
	void SpawnCircle(SPAWN* spawn);

	// 피해
	typedef struct {
		float Str, Vit;
		CMover* SpawnModel;
		CMover* NextModel;
		bool Invincible;
		bool Transparent;
		bool TargetOnly;
		CHit Hit;
	} DAMAGE;
	DAMAGE Damage;

	struct {
		int ID;
	} Lock;

	// 메인 캐릭터
	struct {
		int ID;
		float X, Y;
	} MyShip;
	void MoveMyShip();
	CMover* GetRandomMyShip();

	// 무기
	typedef struct {
		WEAPON_ID ID;
		bool PrevButton;
		int Power, Gauge, MinPower, MaxPower, MaxGauge;
		CMover *Shot, *BigShot, *MaxShot, *Laser;
		CObject3D *PowerEffect, *MaxEffect;
		struct {
			int HistIndex, Hist[NUM_CMD_HIST];
			COMMAND_LIST* List;
		} Cmd;
	} WEAPON;
	WEAPON Weapon;
	CMover* GetLockedEnemy(int id);
	void ClearLockedEnemy();

	// 조준에 의한 폭격
	typedef struct {
		float SX, SY, TX, TY;
		CObject3D *Ready, *Lock, *Target, *Busy;
		bool Locking, Bombing;
		CMover *Model, *Mover;
	} SIGHTED_BOMB;
	SIGHTED_BOMB SightedBomb;

	// 록 온 레이저
	typedef struct {
		float SX, SY;
		CObject3D *Ready, *Lock, *Target, *Busy;
		int MaxAvail, NumAvail;
		CMover *LaserModel, *LaserMover[MAX_LOCKON_LASERS], 
			*TargetMover[MAX_LOCKON_LASERS];
		bool Locked;
	} LOCKON_LASER;
	LOCKON_LASER LockOnLaser;

	// 턴 마커
	typedef struct {
		bool Turning;
		int ButtonID;
		float MX, MY, Yaw, Dist;
		CObject3D* Obj3D;
	} TURN_MARKER;
	TURN_MARKER TurnMarker;

	// 롤
	struct {
		float Pos, Max, Speed;
		bool Auto, OnVX, Plus;
	} Roll;

	// 요
	struct {
		float Pos, Speed;
		bool Auto;
	} Yaw;

	// 표시
	struct {
		float X, Y;
	} Pos;

	// 유도
	struct {
		float Angle;
		CMover* Target;
		bool Simple;
	} Homing;
	void MoveHoming();
	void MoveSimpleHoming();

	void Shot();
	void MaxShot();
	void BigShot(int power);

	void BasicShot(bool button, bool& prev_button);
	void AutoShot(bool button);
	void PowerShot(bool button, int min_power, int max_power, int& power);
	void SemiAutoShot(bool button, 
		int min_power, int max_power, int max_gauge, 
		int& power, int& gauge);
	void PowerShotReleased(bool button, 
		int min_power, int max_power, int& power);
	void ShotAndLaser(bool button, 
		int min_power, int max_gauge, 
		int& power, int& gauge);
	void LockShot(bool button, 
		int min_power, int max_gauge, 
		int& power, int& gauge);
	void CommandShot(const CInputState* is);

public:

	CMover* New();
	CMover* New(CMover* model);
	CMoverGroup* GetGroup(GROUP_ID id);

	// 생성자
	CMover(CMoverGroup* group);
	virtual ~CMover();
	CMover(CMover* mover);

	virtual void Init(CMover* model, CMover* parent=NULL);
	virtual void Check();
	virtual void Move();
	virtual void PostMove();
	virtual void Draw();

	float Rand05() { return (float)rand()/RAND_MAX-0.5f; }
	float Rand1() { return (float)rand()/RAND_MAX; }

	CMover* SetNotInherit(int ih);

	// 기본
	CMover* SetZ(float z) { Z=z; return this; }
	CMover* SetScale(float sx, float sy, float svx, float svy);
	CMover* SetAlpha(float alpha, float v_alpha);
	CMover* SetDamage(
		float str, float vit, 
		CMover* spawn_model, CMover* next_model,
		bool invincible, bool transparent,
		bool target_only=false
	);
	CMover* SetLock(int id);
	CMover* SetNext(CMover* model, int timer);
	CMover* SetObj3D(CObject3D* obj3d);
	CMover* SetAccel(float accel);
	CMover* SetAccelXY(float ax, float ay);
	CMover* SetRand(float speed_rnd, float angle_rnd);
	CMover* SetXYRand(float xrand, float yrand);
	CMover* SetItemEffect_Accel(float accel);
	CMover* SetItemEffect_Docking(bool docking);
	CMover* SetItemEffect_SA(bool special_attack);
	CMover* SetFollowParent(float rx=0, float ry=0);

	// 배경
	CMover* SetBg(CObject3D** chip, int* map, 
		int map_w, int map_h, 
		float sx, float sy, float scrn_w, float scrn_h, 
		float chip_w, float chip_h, float svx, float svy);
	CMover* SetBgRotate(float angle);
	CMover* SetLimitedScrollX() { Valid.LimitedScrollX=true; return this; }
	CMover* SetFreeScrollY() { Valid.FreeScrollY=true; return this; }
	CMover* SetGravity(float vx, float vy);
	CMover* SetControlScrollY(float ytop, float ybottom, float svytop, float svybottom);

	// 탄
	CMover* SetTarget(TARGET_TYPE target_type, int target_id, CMover* alter_model);
	CMover* SetAiming(float speed, float angle);
	CMover* SetAimingDDA(float speed, float angle);
	CMover* SetAimingFPN(float speed, float angle);
	CMover* SetDirected(float speed, float angle);
	CMover* SetDirectedDDA(float speed, float angle);
	CMover* SetDirected16_3(float speed, float angle);
	CMover* SetLooping(float angle, float va, float aa, float radius, float vr, float ar);
	CMover* AddSpawn(CMover* model, int timer, int cycle, int count, float rx=0, float ry=0);
	CMover* SetLaser(
		int length, float valpha, bool add_alpha, 
		CObject3D* tail_obj3d, bool tail_add_alpha, 
		bool tail_damage_transparent
	);
	CMover* SetLaser(int count, float valpha, bool add_alpha);
	CMover* AddSpawnNWay(CMover* model, int timer, int cycle, int count, int nway_count, float nway_angle);
	CMover* AddSpawnCircle(CMover* model, int timer, int cycle, int count, int circle_count, bool circle_odd);
	CMover* SetHoming(float speed, float angle);
	CMover* SetSimpleHoming(float speed);
	CMover* SetHoming(float angle);

	// 적기
	CMover* SetOrbit(ORBIT* data, int count, float xmul, float ymul, bool repeat);

	// 메인 캐릭터
	CMover* SetMyShip(int id, float x, float y, float speed);
	CMover* SetWarp(int time_limit, float distance, CMover* effect);
	CMover* SetOption(int count, int interval, CObject3D* obj3d);
	CMover* SetSCButton(float accel, float min_speed, float max_speed, int button_id);
	CMover* SetSCItem(float min_speed, float max_speed);
	CMover* SetDocking(CObject3D* obj3d, CMover* shot);
	CMover* SetUnderwater(float speed, float air_h, float water_h, CMover* wave);
	CMover* SetBarrier(CMover* model, float warn_vit);
	CMover* SetTransform(CMover* model, CMover* effect, int button_id);
	CMover* SetWalk(float vx, float vy, float jvx, float jvy, int jtime, int button_id);
	
	// 특수공격
	CMover* SetPunch(CMover* model, int button_id, int max_time);
	CMover* SetSAItem(CMover* shot, CMover* effect, int max_time, int warn_time);
	CMover* SetBerserk(CMover* effect, int req_energy, int max_time, int warn_time, int weak_time);
	CMover* SetThrow(int button_id, CHit* hit, float vx, float vy, float vyaw, int max_time);
	CMover* SetArm(int button_id, CObject3D* head, CObject3D* tail, 
		float speed, int max_length, int max_time, int max_stock);
	CMover* SetRFShot(int mode, CMover* shot, CHit* hit);
	CMover* SetColor(int color);
	CMover* SetChangeColor(int button_id, int num_color, CObject3D** obj3d, CMover** shot, CMover** effect, CHit* hit);
	CMover* SetReflect(int button_id, CMover* field, CMover* shot);
	CMover* SetCollision(int min_energy, CMover* crash);
	CMover* SetGraze(CMover* effect);
	CMover* SetSlow(int button_id, int speed);
	CMover* SetQuick(int button_id, int speed);
	CMover* SetMovableSightB(
		int button_id, float x, float y, float speed,
		CMover* sight, CMover* attack);
	CMover* SetMovableSightS(float x, float y, float speed, CMover* sight);
	CMover* SetCaptee();
	CMover* SetCaptor(CMover* attack);

	// 무기
	CMover* SetGroundHit(GROUND_HIT mode_x, GROUND_HIT mode_y);
	CMover* SetWeapon_Common(WEAPON_ID id);
	CMover* SetWeapon_Shot(WEAPON_ID id, CMover* shot);
	CMover* SetWeapon_PowerShot(
		WEAPON_ID id, 
		CMover* shot, CMover* power_shot, CMover* max_shot, 
		CObject3D* power_effect, CObject3D* max_effect,
		int min_power, int max_power, int max_gauge
	);
	CMover* SetWeapon_Laser(
		WEAPON_ID id, 
		CMover* shot, CMover* laser, 
		int min_power, int max_gauge
	);
	CMover* SetWeapon_Command(WEAPON_ID id, COMMAND_LIST* list);
	CMover* SetSightedBomb(
		float sx, float sy,
		CObject3D* ready, CObject3D* lock, 
		CObject3D* target, CObject3D* busy,
		CMover* model
	);
	CMover* SetLockonLaser(
		int max_avail, float sx, float sy,
		CObject3D* ready, CObject3D* lock, 
		CObject3D* target, CObject3D* busy,
		CMover* laser_model
	);
	CMover* SetTurnMarker(int button_id, float dist, float yaw, CObject3D* obj3d);

	// 회전
	CMover* SetRoll(float speed);
	CMover* SetAutoRoll(float max, float speed, bool on_vx=true, bool plus=true);
	CMover* SetYaw(float speed);
	CMover* SetAutoYaw();
	CMover* SetPos(float x, float y);
};


#define MAX_LOCKS 100
typedef enum {
	DAMAGE_BOTH, DAMAGE_TAKE
} DAMAGE_MODE;


// 이동 물체 그룹
class CMoverGroup {
protected:
	CMoverGroupManager* Manager;

	int NumAllMovers, NumFreeMovers;
	CMover** AllMovers;
	CMover** FreeMovers;

	int NumUsedMovers;
	CMover** UsedMovers;

	CHit ScreenHit, DeadzoneHit;

	CInput* Input;

	CMover* LockedMovers[MAX_LOCKS];

	int HitIndex;
	int CollisionEnergy;

	int Speed, SpeedTime;

	LPDIRECT3DDEVICE9 Device;

public:
	LPDIRECT3DDEVICE9 GetDevice() { return Device; }

	int GetCollisionEnergy() { return CollisionEnergy; }
	void IncCollisionEnergy() { CollisionEnergy+=2; }
	int GetSpeed() { return Speed; }
	void SetSpeed(int speed) { Speed=speed; }

	CMoverGroup(CMoverGroupManager* manager, LPDIRECT3DDEVICE9 device, int num_all_movers);
	virtual ~CMoverGroup();

	CMover* New();
	void Delete(CMover* mover);
	void DeleteAll();

	void Check();
	void Move();
	void PostMove();

	void Draw();

	CMover* FirstUsed();
	CMover* NextUsed();
	CMover* FirstHit(CHit* hit);
	CMover* NextHit(CHit* hit);
	CMover* FirstInside(CHit* hit);
	CMover* NextInside(CHit* hit);

	CMover** GetUsed() { return UsedMovers; }
	int GetNumUsed() { return NumUsedMovers; }
	int GetNumFree() { return NumFreeMovers; }

	CMoverGroupManager* GetManager() { return Manager; }
	CMover* GetRandomUsedMover();
	CMover* GetLockedMover(int id) { return LockedMovers[id]; }
	void ClearLockedMover();

	void CheckDamage(CMoverGroup* group, DAMAGE_MODE mode);

	CHit GetScreenHit() { return ScreenHit; }
	void SetScreenHit(const CHit& hit) { ScreenHit=hit; }
	CHit GetDeadzoneHit() { return DeadzoneHit; }
	void SetDeadzoneHit(const CHit& hit) { DeadzoneHit=hit; }

	void SetInput(CInput* input) { Input=input; }
	CInput* GetInput() { return Input; }
};


// 이동 물체 그룹의 관리
class CMoverGroupManager {
protected:
	CMoverGroup* Groups[NUM_GROUPS];
public:

	CMoverGroupManager(LPDIRECT3DDEVICE9 device);
	~CMoverGroupManager();

	void Check();
	void Move();
	void PostMove();
	void Draw();

	void DeleteAll();
	void SetScreenHit(const CHit& hit);
	void SetDeadzoneHit(const CHit& hit);

	CMoverGroup* GetGroup(GROUP_ID group_id) { return Groups[group_id]; }
};


//==============================================================
// 촉수, 다관절 관련

#define MAX_JOINT 100

// 촉수, 다관절
class CJointMover : public CMover {
protected:

	// 촉수，다관절
	typedef struct {

		// 공통
		CMover *Mover[MAX_JOINT], *HeadModel, *BodyModel, *RootModel;
		int NumPart;

		// 촉수
		int NumLoop;
		float MaxDist;

		// 다관절
		CMover *Target;
		float Rad[MAX_JOINT], CRad, VRad, LRad, Dist;

	} JOINT;
	JOINT Joint;

public:

	CJointMover(CMoverGroup* group);
	void Init(CMover* model_joint, CMover* parent);
	void PostMove();

	CJointMover* SetTentacle(
		CMover* head, CMover* body, CMover* root, 
		int num_part, int num_loop, float max_dist);
	CJointMover* SetJoint(
		CMover* head, CMover* body, CMover* root, 
		int num_part, float center_angle, float vangle, float limit_angle, float dist);

};


// 촉수, 다관절 그룹
class CJointMoverGroup : public CMoverGroup {
public:
	CJointMoverGroup(CMoverGroupManager* manager, LPDIRECT3DDEVICE9 device, int num_all_movers);
	~CJointMoverGroup();
};


//==============================================================
// 배경 관련

#define MAX_STARS 100

// 별의 정점형식
typedef struct {
	FLOAT X, Y, Z, RHW;
	D3DCOLOR Color;
} STAR_VERTEX;

// 독자적인 정점형식을 나타내는 FVF
#define D3DFVF_STAR_VERTEX D3DFVF_XYZRHW|D3DFVF_DIFFUSE


// 배경
class CBgMover : public CMover {
protected:

	// 별
	struct {
		float X[MAX_STARS], Y[MAX_STARS];
		float X0, Y0, X1, Y1;
		CObject3D* Obj3D;
		int Num, MinARGB, MaxARGB;
		D3DCOLOR Color[MAX_STARS];
	} Star;

public:
	CBgMover(CMoverGroup* group);
	void Init(CMover* model_bg, CMover* parent);
	void Move();
	void Draw();

	CBgMover* SetStar(
		float vx, float vy, float z_0to1, int num, 
		int min_argb, int max_argb);
	CBgMover* SetStar(
		CObject3D* obj3d, 
		float x0, float y0, float x1, float y1, 
		float vx, float vy, float z, int num);
};


// 배경 그룹
class CBgMoverGroup : public CMoverGroup {
protected:
	LPDIRECT3DVERTEXBUFFER9 StarVertexBuffer;
public:
	CBgMoverGroup(CMoverGroupManager* manager, LPDIRECT3DDEVICE9 device, int num_all_movers);
	~CBgMoverGroup();
	LPDIRECT3DVERTEXBUFFER9 GetStarVertexBuffer() { return StarVertexBuffer; }
};


