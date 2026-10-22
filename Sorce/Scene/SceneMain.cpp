#include "SceneMain.h"
#include "ResultScene.h"
#include "DxLib.h"
#include "../Systems/Pad.h"
#include "../GameObjects/Player.h"
#include "../GameObjects/Enemy.h"
#include "../Systems/Camera.h"
#include "../Systems/EffectManager.h"

namespace
{
	constexpr float kPlayerRadius = 80.0f;// プレイヤーの当たり判定の半径
	constexpr float kEnemyRadius = 80.0f; // エネミーの当たり判定の半径
	constexpr float kFadeTime  = 240.0f;  // シーン切り替えまでの時間(60.0f = 1秒)
	constexpr float kKOTime    = 120.0f;  // KOロゴ表示までの時間(60.0f = 1秒)
	constexpr float kEnemyCount = 20;     // エネミーの数
	constexpr float keffectSize = 60;     // エフェクトのサイズ
	constexpr float kKOSize = 100;        // KOロゴのサイズ

	// ステージの幅
	constexpr float kStageMinX = -1550.0f;
	constexpr float kStageMaxX = 1550.0f;
	constexpr float kStageMinZ = -1550.0f;
	constexpr float kStageMaxZ = 1550.0f;
}

SceneMain::SceneMain() :
	m_frameCount(0),
	m_aliveCount(0),
	m_clearTimer(0),
	m_stageModelHandle(-1),
	m_kOLogoHandle(-1),
	m_koScale(1.0f),
	m_isKOAnim(false),
	m_fontHandle(-1),
	m_timeScale(0),
	m_bgmHandle(0),
	m_koSeHandle(0),
	m_shineSeHandle(0)
{
	m_pPlayer = new Player();

	m_pCamera  = new Camera();
}

SceneMain::~SceneMain()
{
	// 再生中のエフェクトを全て消去
	EffectManager::GetInstance().ClearEffects();

	delete m_pPlayer;

	delete m_pCamera;

	MV1DeleteModel(m_stageModelHandle);

	StopSoundMem(m_bgmHandle);
	DeleteSoundMem(m_bgmHandle);
}

void SceneMain::Init()
{
	// カリングの設定（裏面のポリゴンは見えないようにする）
	SetUseBackCulling(true);

	// Zバッファの設定
	SetUseZBuffer3D(true);	// Zバッファを使います
	SetWriteZBuffer3D(true);	// 描画する物体はZバッファにも距離を書き込む

	SetCameraPositionAndTarget_UpVecY(VGet(0.0f, 300.0f, -700), VGet(0.0f, 0.0f, 0.0f));
	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(1.0f, 1500.0f);

	// 各クラスの初期化
	m_pPlayer->Init();

	m_pCamera ->Init();

	EffectManager::GetInstance().Init();

	auto& efk = EffectManager::GetInstance();

	/*エフェクトの読み込み*/
	efk.Load("HitEffect","data/effects/effect_hit02.efkefc");// エネミーヒット

	m_aliveCount = kEnemyCount;

	m_koScale = kKOSize; // KOロゴのスケール
	m_isKOAnim = false;

	// ステージのモデルを読み込む
	m_stageModelHandle = MV1LoadModel("data/BoxingStage.mv1");
	// フォントを作る
	m_fontHandle = CreateFontToHandle("Nikkyou Sans", 30, -1, DX_FONTTYPE_NORMAL);
	// KOロゴ
	m_kOLogoHandle = LoadGraph("data/KO_Logo.png");
	// ステージの位置とスケールを設定
	MV1SetPosition(m_stageModelHandle, VGet(-3500.0f, 2550.0f, 0.0f));
	MV1SetScale(m_stageModelHandle, VGet(0.5f, 1.0f, 0.5f));

	// BGM
	m_bgmHandle = LoadSoundMem("bgm/game_bgm.mp3");
	m_koSeHandle = LoadSoundMem("bgm/kankankan_se.mp3");
	m_shineSeHandle = LoadSoundMem("bgm/ko_se.mp3");

	ChangeVolumeSoundMem(180, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	// エネミーの複数生成
	for (int i = 0; i < kEnemyCount; i++)
	{
		Enemy* enemy = new Enemy();

		enemy->Init();

		float x = GetRand(1500)-850;
		float z = GetRand(-1000)-300;

		enemy->SetPos(Vector3(x, 100.0f, z));

		m_enemies.push_back(enemy);
	}

	// カメラの初期位置をプレイヤーに合わせる
	m_pCamera->Update(m_pPlayer->GetPos());

}

void SceneMain::Update()
{
	m_frameCount++;

	Pad::Update();

	EffectManager::GetInstance().Update();

	/*スローモーション制御*/ 
	// エネミーの数が0になると、0.5倍速になる
	if (m_aliveCount <= 0)
	{
		m_timeScale = 0.2f;
	}
	else
	{
		m_timeScale = 1.0f;
	}

	// 各クラスの更新
	m_pPlayer ->Update(m_pCamera -> GetAngle(),m_timeScale);
	m_pCamera ->Update(m_pPlayer->GetPos());

	Vector3 playerPos = m_pPlayer->GetPos();

	// ステージの範囲内にプレイヤーがいるように制限
	if (playerPos.x < kStageMinX)playerPos.x = kStageMinX;
	if (playerPos.x > kStageMaxX)playerPos.x = kStageMaxX;
	if (playerPos.z < kStageMinZ)playerPos.z = kStageMinZ;
	if (playerPos.z > kStageMaxZ)playerPos.z = kStageMaxZ;

	m_pPlayer->SetPos(playerPos);

	for(auto enemy : m_enemies)
	{
		enemy->Update(m_timeScale);

		if (!enemy->IsAlive())continue;
		if (enemy->IsSmash())continue;

		Vector3 enemyPos = enemy->GetPos();

		// 攻撃判定
		if (m_pPlayer->IsAttack())
		{
			VECTOR attackPos = m_pPlayer->GetAttackPos();

			float dx = attackPos.x - enemyPos.x;
			float dy = attackPos.y - enemyPos.y;
			float dz = attackPos.z - enemyPos.z;

			float distance = sqrtf(dx * dx + dy * dy + dz * dz);

			float attackRadius = 150.0f;

			VECTOR effectPos = attackPos;

			effectPos.y += 50.0f;// エフェクトの位置を敵の頭上に調整

			// 攻撃が当たったかの判定
			if (distance <= attackRadius + kEnemyRadius)
			{
				EffectManager::GetInstance().Play("HitEffect", effectPos, keffectSize);
				
#ifdef _DEBUG
				printfDx("敵にヒット！\n");
#endif // _DEBUG
				enemy->Kill(m_pPlayer->GetPos());
				
			}
		}

			// 距離計算
			float dx = playerPos.x - enemyPos.x;
			float dy = playerPos.y - enemyPos.y;
			float dz = playerPos.z - enemyPos.z;

			float distance = sqrtf(dx * dx + dy * dy + dz * dz);

#ifdef _DEBUG
			// 当たり判定
			if (distance <= kPlayerRadius + kEnemyRadius)
			{
				printfDx("あたってます\n");
			}
#endif // _DEBUG

	}
	m_aliveCount = 0;

	for (auto enemy : m_enemies)
	{
		if (enemy->IsAlive())
		{
			m_aliveCount++;
		}
	}

	// 全滅した場合、シーン切り替えまでの時間をカウント
	if (m_aliveCount <= 0)
	{
		m_clearTimer++;

		if (m_clearTimer == kKOTime)
		{
			m_isKOAnim = true;

			// SEの再生
			ChangeVolumeSoundMem(200, m_koSeHandle);
			PlaySoundMem(m_koSeHandle, DX_PLAYTYPE_BACK);
			
		}

		if (m_clearTimer >= kFadeTime)
		{
			// SEの再生
			ChangeVolumeSoundMem(200, m_shineSeHandle);
			PlaySoundMem(m_shineSeHandle, DX_PLAYTYPE_BACK);
			m_isClear = true;
		}
	}

	if (m_isKOAnim)
	{
		float targetScale = 1.0f;// 目標のスケール

		m_koScale += (targetScale - m_koScale) * 0.6f;// スムーズにスケールを変化させる
	}
}

void SceneMain::Draw()
{
	m_pCamera->Draw();

	MV1DrawModel(m_stageModelHandle);

	DrawGrid();

#ifdef _DEBUG
	DrawString(0, 0, "SceneMain", GetColor(255, 255, 255));

	DrawFormatString(0, 16, GetColor(255, 255, 255), "FRAME:%d", m_frameCount);
#endif // _DEBUG

	// 各クラスの描画
	m_pPlayer->Draw();
	for(auto enemy : m_enemies)
	{
		enemy->Draw();
	}

	EffectManager::GetInstance().Draw();

	if (m_aliveCount > 0)
	{
		DrawStringToHandle(25.5, 205, "X", GetColor(0, 0, 0), m_fontHandle);
		DrawStringToHandle(25, 200, "X", GetColor(0, 255, 0), m_fontHandle);
		DrawFormatStringToHandle(50.5, 205, GetColor(0, 0, 0), m_fontHandle, "ボタンでテキをたおす(のこり %dヒキ)", m_aliveCount);
		DrawFormatStringToHandle(50, 200, GetColor(255, 255, 255), m_fontHandle, "ボタンでテキをたおす(のこり %dヒキ)", m_aliveCount);
	}
	else
	{
		DrawStringToHandle(25.5, 205, "X", GetColor(0, 0, 0), m_fontHandle);
		DrawStringToHandle(25, 200, "X", GetColor(0, 255, 0), m_fontHandle);
		DrawStringToHandle(50.5, 205, "ボタンでテキをたおす クリア!", GetColor(0, 0, 0), m_fontHandle);
		DrawStringToHandle(50, 200, "ボタンでテキをたおす クリア!", GetColor(255, 255, 255), m_fontHandle);

		int x = 650;
		int y = 300;

		// KO画面を描画
		DrawRotaGraph(x, y,m_koScale,0.0f, m_kOLogoHandle, TRUE);
	}
}

bool SceneMain::IsEnd() const
{
	return m_isClear;
}

Scene* SceneMain::GetNextScene()
{
	return new ResultScene();// 次のシーンへ移行
}

// グリッドを描画する関数
void SceneMain::DrawGrid()
{
	// 直線の始点と終点
	VECTOR startPos;
	VECTOR endPos;

	for (int z = -1000; z <= 1000; z += 100)
	{
		startPos = VGet(-1000.0f, 0.0f, static_cast<float>(z));
		endPos = VGet(1000.0f, 0.0f, static_cast<float>(z));
		DrawLine3D(startPos, endPos, GetColor(0,255,0));
	}
	for (int x = -1000; x <= 1000; x += 100)
	{
		startPos = VGet(static_cast<float>(x), 0.0f, -1000.0f);
		endPos = VGet(static_cast<float>(x), 0.0f, 1000.0f);
		DrawLine3D(startPos, endPos, GetColor(0,255,255));
	}
}