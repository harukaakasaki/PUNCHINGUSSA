#pragma once
#include <string>
#include <DxLib.h>
#include <vector>
#include <map>

// 現在再生中のエフェクトを管理する構造体
struct ActiveEffect
{
	int playHandle = -1; // 再生中のエフェクトのハンドル
};

class EffectManager
{
public:
	static EffectManager& GetInstance()
	{
		static EffectManager instance;
		return instance;
	}

	EffectManager();
	~EffectManager();
	void Init();
	void Update();
	void Draw();
	void End();

	// エフェクトの読み込み
	void Load(const std::string& effectName, const char* filePath);
	// エフェクトの再生
	void Play(const std::string& effectName, VECTOR position, float scale = 1.0f);
	// 再生中のエフェクトを全て消去
	void ClearEffects();

private:

	// コピー禁止
	EffectManager(const EffectManager&) = delete;
	EffectManager& operator=(const EffectManager&) = delete;

	// エフェクトの保管庫
	std::map<std::string, int> m_effects;

	// 現在再生中のエフェクト
	std::vector<ActiveEffect>m_activeEffects;
};

