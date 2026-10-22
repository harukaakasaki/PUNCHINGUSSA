#pragma once
#include <vector>
#include <memory>
#include "Scene.h"

// プロトタイプ宣言
class Player;
class Enemy;
class Camera;

class SceneMain : public Scene
{
public:
	SceneMain();
	~SceneMain();

	void Init();
	void Update();
	void Draw();

	bool IsEnd() const override;
	Scene* GetNextScene() override;

private:
	void DrawGrid();

private:
	int m_stageModelHandle;// ステージのモデルハンドル
	int m_fontHandle;      // フォントのハンドル
	int m_kOLogoHandle;    // KOロゴのハンドル
	int m_aliveCount;
	int m_frameCount;
	int m_clearTimer;
	int m_bgmHandle;
	int m_koSeHandle;
	int m_shineSeHandle;
	float m_koScale;
	float m_timeScale;
	bool m_isClear = false;
	bool m_isKOAnim;

	// 各クラスのポインタを取得
	Player* m_pPlayer;
	Camera* m_pCamera;

	// エネミーのリスト
	std::vector<Enemy*>m_enemies;

};

