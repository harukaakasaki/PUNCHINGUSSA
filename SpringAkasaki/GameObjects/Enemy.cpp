#include "Enemy.h"
#include <DxLib.h>
#include "../Systems/Animation.h"

namespace
{
	// アニメーションのインデックス
	const char* const kIdleAnim = "MonsterArmature|Yes";    // 待機アニメーション
	const char* const kDeathAnim = "MonsterArmature|Death"; // 死亡アニメーション
}


Enemy::Enemy():
	m_pos(100.0f,100.0f,0.0f),
	m_modelHandle(-1),
	m_isAlive(true),
	m_isDead(false),
	m_isSmash(false),
	m_idleAnim(-1),
	m_deathAnim(-1),
	m_hitSeHandle(0)
{
	m_modelHandle = MV1LoadModel("data/enemy_Crab.mv1"); // エネミーモデル
	m_hitSeHandle = LoadSoundMem("bgm/select_se.mp3");    // ヒットSE
}

Enemy::~Enemy()
{
	MV1DeleteModel(m_modelHandle);

	StopSoundMem(m_hitSeHandle);
	DeleteSoundMem(m_hitSeHandle);
}

Vector3 Enemy::GetPos() const
{
	return m_pos;
}


bool Enemy::IsAlive() const
{
	return m_isAlive;
}

void Enemy::Init()
{
	m_animation.Init(m_modelHandle);
	m_idleAnim = MV1GetAnimIndex(m_modelHandle, kIdleAnim);
	m_deathAnim = MV1GetAnimIndex(m_modelHandle, kDeathAnim);
}
void Enemy::Update(float timeScale)
{
	// アニメーションの再生速度
	m_animation.SetGlobalSpeed(0.25f * timeScale);

	if (m_isDead)
	{
		m_animation.Update();
		
		if(m_animation.IsEnd())
		{
			m_isDead = false;
		}
		m_pos += m_velocity;

		return;
	}
	
	if (m_isAlive)
	{
		// 通常アニメーションを再生
		m_animation.Play(m_idleAnim, true, 1.0f);

		m_animation.Update();

	}
	

}
void Enemy::Draw()
{

	if (!m_isAlive && !m_isDead)return;

	MV1SetPosition(m_modelHandle, VGet(m_pos.x, m_pos.y-70, m_pos.z));

	// エネミーの向きを変える
	MV1SetRotationXYZ(m_modelHandle, VGet(0.0f, DX_PI_F, 0.0f));

	MV1DrawModel(m_modelHandle);
#ifdef _DEBUG
	// 球体を描画
	DrawSphere3D(VGet(m_pos.x, m_pos.y, m_pos.z), 80, 16, GetColor(255, 0, 0), GetColor(0, 0, 0), FALSE);
#endif // _DEBUG
}

void Enemy::SetPos(const Vector3& pos)
{
	m_pos = pos;
}

void Enemy::Kill(const Vector3& attackPos)
{
    if(!m_isAlive) return;
	if (m_isSmash) return;

	Vector3 dir = m_pos - attackPos;

	dir.y += 1.0f;

	dir.Normalize();// 正規化

	m_velocity = dir * 0.15f;// 吹っ飛ぶ勢い
	m_velocity.y += 20.0f;

	

	m_isAlive = false;
	m_isDead = true;
	m_isSmash = true;

	// SEの再生
	ChangeVolumeSoundMem(200, m_hitSeHandle);
	PlaySoundMem(m_hitSeHandle, DX_PLAYTYPE_BACK);
	m_animation.Play(m_deathAnim, false, 1.0f);
}
