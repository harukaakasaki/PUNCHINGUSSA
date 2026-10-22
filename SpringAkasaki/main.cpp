#include "DxLib.h"
#include "Scene/SceneController.h"
#include "../Systems/Game.h"
#include <memory>
#include "Scene/SceneMain.h"
#include "../Systems/EffectManager.h"
#include "EffekseerForDXLib.h"

EffectManager effectManager;

// プログラムは WinMain から始まります
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Effekseerを使用する際に必ず使う。
	SetUseDirect3DVersion(DX_DIRECT3D_11);

	// ウィンドウモード設定
	ChangeWindowMode(true);
	// ウィンドウのタイトル変更
	SetMainWindowText("SpringGame");
	// 画面のサイズ変更
	SetGraphMode(Game::kScreenWidth, Game::kScreenHeight, Game::kColorBitNum);

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}



	SetDrawScreen(DX_SCREEN_BACK);

	AddFontResourceEx("data/fonts/NikkyouSans-mLKax.ttf", FR_PRIVATE, NULL);

	//シーンの作成
	SceneController controller;
	controller.Init();


	while (ProcessMessage() != -1)
	{
		LONGLONG start = GetNowHiPerformanceCount();

		// 前のフレーム描画を消す
		ClearDrawScreen();
		//（ゲーム内容）

		controller.Update();
		controller.Draw();

		if (CheckHitKey(KEY_INPUT_ESCAPE))
		{
			break;
		}

		// 内容を画面に反映
		ScreenFlip();

		// ゲーム速度がどのPCでも同じスピードになる
		while (GetNowHiPerformanceCount() - start < 16667)
		{

		}
	}

	// メモリの開放
	// shared_ptrはメモリの開放を自動で行ってくれる

	RemoveFontResourceEx("data/fonts/NikkyouSans-mLKax.ttf", FR_PRIVATE, NULL);

	effectManager.End();
	Effkseer_End(); // エフェクシアの終了処理
	
	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}