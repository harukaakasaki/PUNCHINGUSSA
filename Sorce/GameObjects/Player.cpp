#include "Player.h"
#include <DxLib.h>
#include "../Systems/Pad.h"
#include "../Systems/Game.h"
#include "../Systems/Animation.h"

namespace
{
	constexpr float kSpeed = 20.0f;// 移動速度
	// アニメーションのインデックス
	const char* const kIdleAnim = "CharacterArmature|Idle";    // 待機アニメーション
	const char* const kAttackAnim = "CharacterArmature|Punch"; // 攻撃アニメーション
	const char* const kRunAnim = "CharacterArmature|Run";	   // 走るアニメーション

	// まだ使っていないアニメーション
	const char* const kHitAnim = "CharacterArmature|Hit";	   // ヒット時アニメーション
}

Player::Player():
	m_pos(-100.0f, 100.0f, -50.0f),
	m_modelHandle(-1),
	m_idleAnim(-1),
	m_attackAnim(-1),
	m_runAnim(-1),
	m_attackSeHandle(0)
{
	m_modelHandle = MV1LoadModel("data/player.mv1");
	m_attackSeHandle = LoadSoundMem("bgm/punchMiss_se.mp3");// 攻撃SE
}

Player::~Player()
{
	MV1DeleteModel(m_modelHandle);
	StopSoundMem(m_attackSeHandle);
	DeleteSoundMem(m_attackSeHandle);
}

void Player::Init()
{
	m_animation.Init(m_modelHandle);
	m_idleAnim = MV1GetAnimIndex(m_modelHandle, kIdleAnim);
	m_attackAnim = MV1GetAnimIndex(m_modelHandle, kAttackAnim);
	m_runAnim = MV1GetAnimIndex(m_modelHandle, kRunAnim);
}

void Player::Update(float cameraAngle, float timeScale)
{
	int x, y;
	GetJoypadAnalogInput(&x, &y, DX_INPUT_PAD1);

	float stickX = x / 1000.0f;
	float stickY = y / 1000.0f;

	// デッドゾーン（微妙な入力を無視する）
	float deadZone = 0.2f;
	if (fabs(stickX) < deadZone)stickX = 0.0f;
	if (fabs(stickY) < deadZone)stickY = 0.0f;

	// 移動
	Vector3 move = { 0.0f, 0.0f, 0.0f };

	// 前後
	move.x += cosf(cameraAngle) * stickY;
	move.z += sinf(cameraAngle) * stickY;
	// 左右
	move.x += -sinf(cameraAngle) * stickX;
	move.z += cosf(cameraAngle) * stickX;

	float len = sqrtf(move.x * move .x + move.z * move.z);
	if (len > 0)
	{
		// 移動方向から角度を作る
		m_angle = atan2f(-move.x, -move.z);

		// 正規化
		move.x /= len;
		move.z /= len;

		move.x = move.x * kSpeed * timeScale;
		move.z = move.z * kSpeed * timeScale;
	}

	m_pos.x += move.x;
	m_pos.z += move.z;

	// 攻撃
	if (Pad::IsTrigger(PAD_INPUT_3)&& !isAttack)
	{
		isAttack = true;
		// 攻撃アニメーションを再生
		m_animation.Play(m_attackAnim, false, 2.0f);
		// SEの再生
		ChangeVolumeSoundMem(200, m_attackSeHandle);
		PlaySoundMem(m_attackSeHandle, DX_PLAYTYPE_BACK);
	}
	// 攻撃状態じゃないときは待機アニメーションを再生
	if (!isAttack)
	{
		if (len > 0)
		{
			// 走るアニメーション
			m_animation.Play(m_runAnim, true, 1.0f);
		}
		else
		{
			// 待機アニメーション
			m_animation.Play(m_idleAnim, true, 1.0f);
		}
	}
	// 攻撃アニメーションが終了したら攻撃状態を解除
	if (isAttack && m_animation.IsEnd())
	{
		isAttack = false;
	}
	// アニメーションの再生速度
	m_animation.SetGlobalSpeed(0.7f * timeScale);
	m_animation.Update();

}

void Player::Draw()
{
	MATRIX rot = MGetRotY(m_angle);
	MATRIX trans = MGetTranslate(VGet(m_pos.x, m_pos.y - 80, m_pos.z));

	MATRIX mtx = MMult(rot, trans);

	MV1SetMatrix(m_modelHandle, mtx);

	MV1DrawModel(m_modelHandle);

#ifdef _DEBUG
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 80, 16, GetColor(0, 255, 0), GetColor(0, 0, 0), FALSE);
#endif // _DEBUG

	if (isAttack)
	{ 
		// 前方向
		float dirX = -sinf(m_angle);
		float dirZ = -cosf(m_angle);

		// 前に出す距離
		float attackRange = 150.0f;

		//// 攻撃時に前に踏み込む
		//float attackMove = 3.0f;

		//m_pos.x += dirX * attackMove;
		//m_pos.z += dirZ * attackMove;

		// 攻撃位置
		VECTOR attackPos = VGet(
			m_pos.x + dirX * attackRange,
			m_pos.y,
			m_pos.z + dirZ * attackRange);

#ifdef _DEBUG
		// 球体を描画
		
		DrawSphere3D(attackPos, 150, 16, GetColor(255, 255, 0), GetColor(0, 0, 0), FALSE);
#endif // DEBUG

	}
}

Vector3 Player::GetPos() const
{
	return m_pos;
}

void Player::SetPos(const Vector3& pos)
{
	m_pos = pos;
}

VECTOR Player::GetAttackPos() const
{
	float dirX = -sinf(m_angle);
	float dirZ = -cosf(m_angle);

	float attackRange = 150.0f;

	return VGet(
		m_pos.x + dirX * attackRange,
		m_pos.y,
		m_pos.z + dirZ * attackRange);

}

bool Player::IsAttack() const
{
	return isAttack;
}