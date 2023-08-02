	#pragma once
#include <DxLib.h>
#include <array>
#include "../Scene/SceneID.h"

#define lpSooundPross SoundProcess::GetInstance()

//SEの種類
enum class SOUNDNAME_SE
{
	playerMove,
	playerDash,
	playerAB,
	playerHit,
	playerAttack,
	playerSpetial,
	playerAttackHit,
	playerJump,
	playerDestory,
	enemyAttack,
	enemyDestroy,
	select,
	openMenu,
	cursorMove,
	click,
	enter,
	Max
};

// BGMの種類
enum class SOUNDNAME_BGM
{
	TitleSceneBGM,
	TutorialSceneBGM,
	GameSceneStage1BGM,
	GameSceneStage2BGM,
	ResultClearBGM,
	ResultFailBGM,
	Max
};

constexpr int seNum = static_cast<int>(SOUNDNAME_SE::Max) + 1;				// SEの数
constexpr int bgmNum = static_cast<int>(SOUNDNAME_BGM::Max) + 1;			// BGMの数

class SoundProcess
{
public:

	static SoundProcess& GetInstance(void)
	{
		static SoundProcess s_Instance;
		return s_Instance;
	}

	// 初期化
	void Init(SceneID nowScene);

	// ロード
	void Load(int loadFile, SOUNDNAME_SE name);		// SEのロード
	void Load(int loadFile, SOUNDNAME_BGM name);	// BGMのロード

	/// <summary>
	/// SEの再生
	/// </summary>
	/// <param name="name">SEの種類</param>
	/// <param name="loopflag">ループするかどうか</param>
	void PlayBackSound(SOUNDNAME_SE name, bool loopflag);

	/// <summary>
	/// BGMの再生
	/// </summary>
	/// <param name="name">BGMの種類</param>
	/// <param name="loopflag">ループするかどうか</param>
	/// <param name="topPotisionflag">途中から再生するかどうか</param>
	void PlayBackSound(SOUNDNAME_BGM name,bool loopflag, bool topPotisionflag);

	/// <summary>
	/// SEを止める
	/// </summary>
	/// <param name="name">名前</param>
	void SoundStop(SOUNDNAME_SE name);

	/// <summary>
	/// BGMを止める
	/// </summary>
	/// <param name="name">名前</param>
	void SoundStop(SOUNDNAME_BGM name);

	// 解放
	void Release();
	void Release(SOUNDNAME_SE name);
	void Release(SOUNDNAME_BGM name);

	// SEのユーザー音量調整を設定
	void SetSEVolumeUserSet(float UserSetVolume);

	// SEのユーザー音量調整を渡す
	float GetSEVolumeUserSet();

	// BGMのユーザー音量調整を設定
	void SetBGMVolumeUserSet(float UserSetVolume);

	// BGMのユーザー音量調整を渡す
	float GetBGMVolumeUserSet();

	// SEのデフォルト音量を渡す
	int GetDefaultSEVolume();

	// BGMのデフォルト音量を渡す
	int GetDefaultBGMVolume();

private:
	SoundProcess();
	~SoundProcess();

	// ボリューム関係
	int volume_;									// ボリューム
	int defaultSEVol_;								// デフォルトのSEのボリューム
	int defaultBGMVol_ ;							// デフォルトのBGMのボリューム

	// SEの変数
	std::array<int, seNum> se_sound;				// seを数だけ保存
	std::array<bool, seNum> se_loadFlag;			// ロードのチェック
	std::array<bool, seNum> se_playFlag;			// 再生チェック
	float se_setUserVol_;							// SEのユーザー音量調整

	// BGMの変数
	std::array<int, bgmNum> bgm_sound;				// BGMを数だけ保存する
	std::array<bool, seNum> bgm_loadFlag;			// ロードのチェック
	std::array<bool, seNum> bgm_playFlag;			// 再生チェック
	float bgm_seUserVol_;							// BGMのユーザー音量調整
};