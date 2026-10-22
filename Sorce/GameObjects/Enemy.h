#pragma once
#include "../Systems/Vector3.h"
#include "../Systems/Animation.h"
class Enemy
{
public:
	Enemy();
	~Enemy();
	void Init();
	void Update(float timeScale);
	void Draw();
	Vector3 GetPos() const;

	void SetPos(const Vector3& pos);
	void Kill(const Vector3& attackPos);
	bool IsAlive()const;
	bool IsSmash()const { return m_isSmash; }

private:

	int m_modelHandle;
	Animation m_animation;
	// アニメーションのインデックス
	int m_idleAnim;
	int m_deathAnim;

	// SE系
	int m_hitSeHandle;

	bool m_isAlive;
	bool m_isDead;
	bool m_isSmash;

	Vector3 m_pos;// 位置
	Vector3 m_velocity;// 吹っ飛び速度

};

