#include "SoundPross.h"
#include "Debug.h"
void SoundProcess::Load(int loadFile, SOUNDNAME_SE name)
{
	// loadFileに格納		
	se_sound[static_cast<int>(name)] = loadFile;	// 種類別にサウンドを分ける
	se_loadFlag[static_cast<int>(name)] = true;		// フラグをtrueにする
}

void SoundProcess::Load(int loadFile, SOUNDNAME_BGM name)
{
	// loadFileに格納
	bgm_sound[static_cast<int>(name)] = loadFile;	// 種類別にサウンドを分ける
	bgm_loadFlag[static_cast<int>(name)] = true;	// フラグをtrueにする
}

void SoundProcess::Init(SceneID nowScene)
{
	if (nowScene == SceneID::Title)
	{
		// BGMのロード
		Load(LoadSoundMem(L"Resource/resource/Sound/wave.mp3"), SOUNDNAME_BGM::TitleSceneBGM);
		Load(LoadSoundMem(L"Resource/resource/Sound/Select.wav"), SOUNDNAME_SE::select);
		Load(LoadSoundMem(L"Resource/resource/Sound/click.mp3"), SOUNDNAME_SE::enter);
	}

	if (nowScene == SceneID::Select)
	{
		Load(LoadSoundMem(L"Resource/resource/Sound/openMenu.mp3"), SOUNDNAME_SE::openMenu);
		Load(LoadSoundMem(L"Resource/resource/Sound/cursorMove.mp3"), SOUNDNAME_SE::cursorMove);
		Load(LoadSoundMem(L"Resource/resource/Sound/click.mp3"), SOUNDNAME_SE::click);
	}

	if (nowScene == SceneID::Game)
	{
		// 3Dサウンド用にする
		SetCreate3DSoundFlag(true);
		Load(LoadSoundMem(L"Resource/resource/Sound/Rocket.wav"), SOUNDNAME_SE::enemyAttack);
		SetCreate3DSoundFlag(false);
		// ここまで3Dサウンドとする

		// ここから2Dサウンド
		Load(LoadSoundMem(L"Resource/resource/Sound/PlayerMoveSound.wav"), SOUNDNAME_SE::playerMove);
		Load(LoadSoundMem(L"Resource/resource/Sound/dash.wav"), SOUNDNAME_SE::playerDash);
		Load(LoadSoundMem(L"Resource/resource/Sound/afuteburner.wav"), SOUNDNAME_SE::playerAB);
		Load(LoadSoundMem(L"Resource/resource/Sound/playerHit.wav"), SOUNDNAME_SE::playerHit);
		Load(LoadSoundMem(L"Resource/resource/Sound/playerAttack.wav"), SOUNDNAME_SE::playerAttack);
		Load(LoadSoundMem(L"Resource/resource/Sound/beam.wav"), SOUNDNAME_SE::playerSpetial);
		Load(LoadSoundMem(L"Resource/resource/Sound/playerAttackHit.wav"), SOUNDNAME_SE::playerAttackHit);
		Load(LoadSoundMem(L"Resource/resource/Sound/playerJump.wav"), SOUNDNAME_SE::playerJump);
		Load(LoadSoundMem(L"Resource/resource/Sound/playerDestroy.wav"), SOUNDNAME_SE::playerDestory);
		Load(LoadSoundMem(L"Resource/resource/Sound/enemyDestroy.wav"), SOUNDNAME_SE::enemyDestroy);


		// BGMのロード
		Load(LoadSoundMem(L"Resource/resource/Sound/TutorialSceneBGM.mp3"), SOUNDNAME_BGM::TutorialSceneBGM);
		Load(LoadSoundMem(L"Resource/resource/Sound/GameSceneBGM_1.wav"), SOUNDNAME_BGM::GameSceneStage1BGM);
		Load(LoadSoundMem(L"Resource/resource/Sound/GameSceneBGM_2.mp3"), SOUNDNAME_BGM::GameSceneStage2BGM);
		// 本来BGMではないがループ処理を行うためBGMとする
	}

	if (nowScene == SceneID::Result)
	{
		Load(LoadSoundMem(L"Resource/resource/Sound/Clear.mp3"), SOUNDNAME_BGM::ResultClearBGM);
		Load(LoadSoundMem(L"Resource/resource/Sound/Fail.wav"), SOUNDNAME_BGM::ResultFailBGM);
		Load(LoadSoundMem(L"Resource/resource/Sound/Select.wav"), SOUNDNAME_SE::select);
	}
}

// SEの再生
void SoundProcess::PlayBackSound(SOUNDNAME_SE name,bool loopflag)
{
	// 音量の調整
	volume_ = se_setUserVol_ * defaultSEVol_;

	// SEのフラグをtrueにする
	se_playFlag[static_cast<int>(name)] = true;

	// 再生可能だったら再生する
	if (!loopflag)
	{
		if (se_playFlag[static_cast<int>(name)])
		{
			// 音量を設定する
			ChangeVolumeSoundMem(volume_, se_sound[static_cast<int>(name)]);
			// 再生処理
			PlaySoundMem(se_sound[static_cast<int>(name)], DX_PLAYTYPE_BACK);
		}
	}
	else
	{
		// 再生可能だったら再生する
		if (se_playFlag[static_cast<int>(name)])
		{
			// 音量を設定する
			ChangeVolumeSoundMem(volume_, se_sound[static_cast<int>(name)]);
			// 再生処理
			PlaySoundMem(se_sound[static_cast<int>(name)], DX_PLAYTYPE_LOOP);

		}
	}
}

// BGMの再生
void SoundProcess::PlayBackSound(SOUNDNAME_BGM name, bool loopflag, bool topPotisionflag)
{
	// volumeの調整
	DebugLog("PlayBgm=", bgm_seUserVol_);
	volume_ = bgm_seUserVol_ * defaultBGMVol_;

	// BGMのフラグをtrueにする
	bgm_playFlag[static_cast<int>(name)] = true;

	// ループするかどうか
	if (!loopflag)
	{
		// 再生可能だったら再生する
		if (bgm_playFlag[static_cast<int>(name)])
		{
			// 音量をセット
			ChangeVolumeSoundMem(volume_, bgm_sound[static_cast<int>(name)]);
			PlaySoundMem(bgm_sound[static_cast<int>(name)], DX_PLAYTYPE_BACK);
		}
	}
	else
	{
		// 再生可能だったら再生する
		if (bgm_playFlag[static_cast<int>(name)])
		{
			// 音量をセット
			ChangeVolumeSoundMem(volume_, bgm_sound[static_cast<int>(name)]);
			// 再生処理
			PlaySoundMem(bgm_sound[static_cast<int>(name)], DX_PLAYTYPE_LOOP);

		}
	}
}

void SoundProcess::SoundStop(SOUNDNAME_SE name)
{
	// SEを止める
	StopSoundMem(se_sound[static_cast<int>(name)]);
}

void SoundProcess::SoundStop(SOUNDNAME_BGM name)
{
	// BGMを止める
	StopSoundMem(bgm_sound[static_cast<int>(name)]);
}

void SoundProcess::Release()
{
	// SEを消す
	for (int i = 0; i != seNum; ++i)
	{
		if (!se_loadFlag[i])
		{
			continue;
		}
		DeleteSoundMem(se_sound[i]);
		se_sound[i] = -1;
	}

	// BGMを消す
	for (int i = 0; i != bgmNum; ++i)
	{
		if (!bgm_loadFlag[i])
		{
			continue;
		}
#ifndef DEBUG
		StopSoundMem(bgm_sound[i]);
#endif
		DeleteSoundMem(bgm_sound[i]);
		bgm_sound[i] = -1;
	}
}

void SoundProcess::Release(SOUNDNAME_SE name)
{
	DeleteSoundMem(se_sound[static_cast<int>(name)]);
}

void SoundProcess::Release(SOUNDNAME_BGM name)
{
	DeleteSoundMem(bgm_sound[static_cast<int>(name)]);
}

void SoundProcess::SetSEVolumeUserSet(float volumeEntire)
{
	se_setUserVol_ = volumeEntire;
}

float SoundProcess::GetSEVolumeUserSet()
{
	return se_setUserVol_;
}

void SoundProcess::SetBGMVolumeUserSet(float volumeEntire)
{
	DebugLog("SetBGM=", volumeEntire);
	bgm_seUserVol_ = volumeEntire;
}

float SoundProcess::GetBGMVolumeUserSet()
{
	DebugLog("GetBGM=", bgm_seUserVol_);
	return bgm_seUserVol_;
}

int SoundProcess::GetDefaultSEVolume()
{
	return defaultSEVol_;
}

int SoundProcess::GetDefaultBGMVolume()
{
	return defaultBGMVol_;
}

SoundProcess::SoundProcess()
{
	// SEのデフォルト音量
	defaultSEVol_ = 255.0f;

	// BGMのデフォルト音量
	defaultBGMVol_ = 255.0f;

}

SoundProcess::~SoundProcess()
{
}
