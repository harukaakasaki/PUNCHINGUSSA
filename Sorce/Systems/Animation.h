#pragma once
class Animation
{
public:
	Animation();
	~Animation();
	void Init(int modelHandle);
	void Update();
	void Play(int animIndex,bool loop,float speed);
	void Stop();
	void SetGlobalSpeed(float speed);

	bool IsPlaying() const;
	bool IsEnd() const;

private:

	int m_modelHandle;// アニメーションモデルのハンドル

	int m_currentAnimIndex;// 現在再生中のアニメーションのインデックス

	int m_currentAnimHandle;// 現在再生中のアニメーションのインデックス
	int m_lastAnimHandle;// 最後に再生したアニメーションのインデックス

	float m_currentAnimCount;// アニメーションの再生時間
	float m_lastAnimCount;// 最後に再生したアニメーションの再生時間
	float m_globalSpeed;// アニメーションの再生速度

	int m_changeAnimFrame;// アニメーションを切り替えるフレーム数

	int m_animSpeed;// アニメーションの再生速度

	bool m_isLoop;// アニメーションをループさせるかどうか

	bool m_isEnd;// アニメーションが終了したかどうか

};

