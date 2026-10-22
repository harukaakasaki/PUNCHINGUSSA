#include "TitleScene.h"
#include "../Systems/Pad.h"
#include "../Systems/Game.h"
#include "SceneMain.h"
#include <DxLib.h>

namespace
{
	// アニメーションのインデックス
	const char* const kPlayerAnim = "CharacterArmature|Idle";// プレイヤー待機アニメーション
	const char* const kEnemyAnim = "MonsterArmature|Jump";     // エネミー待機アニメーション
}

TitleScene::TitleScene():
	m_skyModelHandle(-1),
	m_playerModelHandle(-1),
	m_enemyModelHandle(-1),
	m_fontHandle(-1),
	m_titleLogoHandle(-1),
	m_skyAngle(0.0f),
	m_playerIdleAnim(-1),
	m_enemyIdleAnim(-1),
	m_isEnd(false),
	m_bgmHandle(0),
	m_selectSeHandle(0)
{
}

TitleScene::~TitleScene()
{
	MV1DeleteModel(m_skyModelHandle);
	MV1DeleteModel(m_playerModelHandle);
	MV1DeleteModel(m_enemyModelHandle);
	DeleteGraph(m_titleLogoHandle);

	StopSoundMem(m_bgmHandle);
	DeleteSoundMem(m_bgmHandle);
	StopSoundMem(m_selectSeHandle);
	DeleteSoundMem(m_selectSeHandle);
}

void TitleScene::Init()
{
	// モデルを読み込む
	m_skyModelHandle = MV1LoadModel("data/Sky_Night02.mv1");
	m_playerModelHandle = MV1LoadModel("data/player.mv1");
	m_enemyModelHandle = MV1LoadModel("data/enemy_Crab.mv1");
	m_titleLogoHandle = LoadGraph("data/TitleLogo_3.png");

	// BGM
	m_bgmHandle = LoadSoundMem("bgm/title_bgm.mp3");
	m_selectSeHandle = LoadSoundMem("bgm/select_se.mp3");

	ChangeVolumeSoundMem(180, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	m_animation.Init(m_playerModelHandle);
	m_animation.Init(m_enemyModelHandle);

	m_playerIdleAnim = MV1GetAnimIndex(m_playerModelHandle, kPlayerAnim);
	m_enemyIdleAnim = MV1GetAnimIndex(m_enemyModelHandle, kEnemyAnim);
	

	// フォントを作る
	m_fontHandle = CreateFontToHandle("Nikkyou Sans", 70, -1, DX_FONTTYPE_NORMAL);

	MV1SetPosition(m_skyModelHandle, VGet(500, -600, 0));
	MV1SetPosition(m_playerModelHandle, VGet(700, 0, -200));
	MV1SetPosition(m_enemyModelHandle, VGet(300, 0, -200));

	// カメラ位置
	SetCameraPositionAndTarget_UpVecY(
		VGet(500, 300, -1000),// カメラ位置
		VGet(500, 300, -200));// 注視点

	SetupCamera_Perspective(DX_PI_F / 3.0f);
	SetCameraNearFar(100.0f, 15000.0f);
}

void TitleScene::Update()
{
	Pad::Update();

	// 待機アニメーション
	m_animation.Play(m_playerIdleAnim, true, 1.0f);
	m_animation.Play(m_enemyIdleAnim, true, 1.0f);
	// アニメーションの再生速度
	m_animation.SetGlobalSpeed(0.7f);
	m_animation.Update();

	// 空の回転
	m_skyAngle += 0.002f;
	// 点滅
	m_blinkAngle += 0.08f;

	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// SEの再生
		ChangeVolumeSoundMem(200, m_selectSeHandle);
		PlaySoundMem(m_selectSeHandle, DX_PLAYTYPE_BACK);
		m_isEnd = true;
	}
}

void TitleScene::Draw()
{
	// スカイドームの回転
	MV1SetRotationXYZ(m_skyModelHandle, VGet(0, m_skyAngle, 0));

	// モデルを描画
	MV1DrawModel(m_skyModelHandle);
	/*MV1DrawModel(m_playerModelHandle);
	MV1DrawModel(m_enemyModelHandle);*/

	// 2D描画の設定
	SetUseZBufferFlag(false);
	// タイトル画面を描画
	DrawGraph(0, 50,m_titleLogoHandle, TRUE);
	

	SetUseZBufferFlag(true);

	// スタートボタンを点滅させる
	int alpha = static_cast<int>((sinf(m_blinkAngle) * 0.5f + 0.5f) * 255);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(460, 505, "Aボタンでスタート！", GetColor(0, 0, 0), m_fontHandle);
	DrawStringToHandle(450, 500, "Aボタンでスタート！", GetColor(255, 255, 255), m_fontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

Scene* TitleScene::GetNextScene()
{
	return new SceneMain();// 次のシーンへ移行
}
