#include "EffectManager.h"
#include "EffekseerForDXLib.h"
#include "DxLib.h"

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
}

void EffectManager::Init()
{
	static bool isInitialized = false;
	if (isInitialized)return;

	if (Effekseer_Init(8000) == -1)
	{
		// エフェクトの初期化に失敗
		printfDx("エフェクトの初期化に失敗しました。\n");
	}
	// DxLibの3D表示を合わせる
	Effekseer_InitDistortion();
	SetUse3DFlag(TRUE);
	SetWriteZBuffer3D(TRUE);

	isInitialized = true;

}

void EffectManager::Update()
{
	// Effekseerのエフェクトを更新
	UpdateEffekseer3D();

	// 再生終了したものをリストから消去
	for (auto it = m_activeEffects.begin(); it != m_activeEffects.end(); )
	{
		if (IsEffekseer3DEffectPlaying(it->playHandle) == -1)
		{
			it = m_activeEffects.erase(it);
		}
		else
		{
			// 次に進む
			++it;
		}
	}
}

void EffectManager::Draw()
{
	DrawEffekseer3D();
}

void EffectManager::End()
{
	// 読み込んだエフェクトを全て解放
	for (auto& effect : m_effects)
		if (effect.second != -1)
		{
			DeleteEffekseerEffect(effect.second);
			effect.second = -1;
		}
	m_effects.clear();

	m_activeEffects.clear();

	//Effkseer_End();
}

void EffectManager::Load(const std::string& effectName, const char* filePath)
{
#ifdef _DEBUG
	printfDx("現在のエフェクト: %s\n", filePath);
#endif // _DEBUG

	int effectHandle = LoadEffekseerEffect((const char*)filePath);

	if (effectHandle != -1)
	{
		m_effects[effectName] = effectHandle;
	}
	else
	{
		printfDx("エフェクトの読み込みに失敗しました: %s\n", filePath);
	}
}

void EffectManager::Play(const std::string& effectName, VECTOR position, float scale)
{
	// 登録されていない場合は再生しない
	if (m_effects.find(effectName) == m_effects.end())return;

	// 再生
	int handle = PlayEffekseer3DEffect(m_effects[effectName]);
	if (handle != -1)
	{
		SetPosPlayingEffekseer3DEffect(handle, position.x, position.y, position.z);
		SetScalePlayingEffekseer3DEffect(handle, scale, scale, scale);

		// リストに追加して管理
		m_activeEffects.push_back({ handle });
	}
}

void EffectManager::ClearEffects()
{
	m_activeEffects.clear();
}
