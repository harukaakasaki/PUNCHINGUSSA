#include "ResultScene.h"
#include "../Systems/Pad.h"
#include "TitleScene.h"
#include <DxLib.h>
#include <cmath>

namespace
{
	// アニメーションのインデックス
	const char* const kPlayerAnim = "CharacterArmature|Idle";// プレイヤー待機アニメーション
}

ResultScene::ResultScene():
	m_isEnd(false),
	m_skyModelHandle(-1),
	m_fontHandle(-1),
	m_stageModelHandle(-1),
	m_playerModelHandle(-1),
	m_playerIdleAnim(-1),
	m_bgmHandle(0),
	m_selectSeHandle(0)
{
}

ResultScene::~ResultScene()
{
	MV1DeleteModel(m_skyModelHandle);
	MV1DeleteModel(m_playerModelHandle);
	MV1DeleteModel(m_stageModelHandle);

	StopSoundMem(m_bgmHandle);
	DeleteSoundMem(m_bgmHandle);
	StopSoundMem(m_selectSeHandle);
	DeleteSoundMem(m_selectSeHandle);
}

void ResultScene::Init()
{
	// モデルを読み込む
	m_skyModelHandle = MV1LoadModel("data/BoxingResult.mv1");
	m_playerModelHandle = MV1LoadModel("data/player.mv1");
	m_stageModelHandle = MV1LoadModel("data/WinStage.mv1");

	// BGM
	m_bgmHandle = LoadSoundMem("bgm/result_bgm.mp3");
	m_selectSeHandle = LoadSoundMem("bgm/select_se.mp3");

	ChangeVolumeSoundMem(140, m_bgmHandle);
	PlaySoundMem(m_bgmHandle, DX_PLAYTYPE_LOOP);

	// フォントを作る
	m_fontHandle = CreateFontToHandle("Nikkyou Sans", 70, -1, DX_FONTTYPE_NORMAL);
	// カメラ位置
	SetCameraPositionAndTarget_UpVecY(
		VGet(500, 300, -1000),// カメラ位置
		VGet(500, 300, -200));// 注視点

	// 各モデルの位置
	MV1SetPosition(m_skyModelHandle, VGet(500, -600, 0));
	MV1SetPosition(m_playerModelHandle, VGet(500, 150, -200));
	// ステージの位置とスケールを設定
	MV1SetPosition(m_stageModelHandle, VGet(3270, -205, 0));
	MV1SetScale(m_stageModelHandle, VGet(1.2f, 1.2f, 1.2f));

	
}

void ResultScene::Update()
{
	// パッドクラスの更新
	Pad::Update();

	// Aボタンが押されたらシーンを終了する
	if (Pad::IsTrigger(PAD_INPUT_1))
	{
		// SEの再生
		ChangeVolumeSoundMem(200, m_selectSeHandle);
		PlaySoundMem(m_selectSeHandle, DX_PLAYTYPE_BACK);
		m_isEnd = true;
	}

	// 空の回転
	m_skyAngle += 0.008f;
	// プレイヤーの回転
	m_playerAngle += 0.008f;
	m_blinkAngle += 0.08f;
}

void ResultScene::Draw()
{
	// モデルを描画
	MV1DrawModel(m_skyModelHandle);
	MV1DrawModel(m_playerModelHandle);
	MV1DrawModel(m_stageModelHandle);

	// スカイドームの回転
	MV1SetRotationXYZ(m_skyModelHandle, VGet(0, m_skyAngle, 0));
	MV1SetRotationXYZ(m_playerModelHandle, VGet(0, m_playerAngle, 0));

	// 文字を描画
	DrawStringToHandle(465, 55, "You Win!", GetColor(0, 0, 0), m_fontHandle);
	DrawStringToHandle(460, 50, "You Win!", GetColor(0, 255, 255), m_fontHandle);
	
	// スタートボタンを点滅させる
	int alpha = static_cast<int>((sinf(m_blinkAngle) * 0.5f + 0.5f) * 255);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, alpha);
	DrawStringToHandle(365, 560, "Aボタンでタイトルにもどる", GetColor(0, 0, 0), m_fontHandle);
	DrawStringToHandle(360, 555, "Aボタンでタイトルにもどる", GetColor(255, 255, 255), m_fontHandle);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	
}

// シーン切り替え
Scene* ResultScene::GetNextScene()
{
	return new TitleScene;
}
