#pragma once
#include <vector>
#include "../Systems/Vector3.h"
#include "../Systems/Camera.h"
#include "../Systems/Animation.h"


class Player
{
public:
	Player();
	~Player();
	void Init();
	void Update(float cameraAngle,float timeScale);
	void Draw();
	void SetPos(const Vector3& pos);

	// プレイヤー情報の取得
	Vector3 GetPos() const;// 位置
	VECTOR GetAttackPos() const;// 攻撃位置
	bool IsAttack() const;// 攻撃中かどうか
	

private:
	int m_modelHandle;// モデル
	Animation m_animation;// アニメーション
	// アニメーションのインデックス
	int m_idleAnim;
	int m_attackAnim;
	int m_runAnim;

	// SE系
	int m_attackSeHandle;

	float m_angle = 0.0f;// 角度

	Vector3 m_pos;// 位置
	bool isAttack = false;// 攻撃しているかどうか
	
};

