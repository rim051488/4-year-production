#include <DxLib.h>
#include "PEManager.h"
#include "PEBase.h"
#include "PEType/NoPE.h"
#include "PEType/MonoPE.h"
#include "PEType/AveblurPE.h"
#include "PEType/GaussPE.h"
#include "PEType/DofPE.h"
#include "PEType/VolfogPE.h"
#include "PEType/ColortoMono.h"

#include "../Common/Debug.h"

PEManager::PEManager()
{
	// 必要なシェーダを読み込む
	ShaderLoad();
	// ポストエフェクトのフラグをすべてfalseにする
	FlagSetFalse();
	// 非同期処理を一度きる(非同期時にまわすとフリーズすることがある)
	SetUseASyncLoadFlag(false);
	// ポストエフェクトの初期設定を行う
	PostEffect_[0].first = std::make_shared<NoPE>(-1, pos_, rate_);
	PostEffect_[1].first = std::make_shared<MonoPE>(postPS_[0], pos_, rate_);
	PostEffect_[2].first = std::make_shared<AveblurPE>(postPS_[1], pos_, rate_);
	PostEffect_[3].first = std::make_shared<GaussPE>(postPS_[2], pos_, rate_);
	PostEffect_[4].first = std::make_shared<DofPE>(postPS_[3], pos_, rate_);
	PostEffect_[5].first = std::make_shared<VolfogPE>(postPS_[4], pos_, rate_);
	PostEffect_[6].first = std::make_shared<ColortoMono>(postPS_[5], pos_, rate_);
	// フラグ管理用の変数を初期化
	usePE_.fill(false);
	// すべての処理が終わったので、非同期処理をtrueにする
	SetUseASyncLoadFlag(true);
}

PEManager::~PEManager()
{
	// 使ったシェーダを初期化する
	for (auto& ps : postPS_)
	{
		DeleteShader(ps);
	}
}

void PEManager::Update(float delta)
{
#ifdef _DEBUG
	// デバッグ用ポストエフェクトがちゃんと使用できるか確認する用
	CheckPostEffect(PEID::Default,KEY_INPUT_0);
	CheckPostEffect(PEID::Mono, KEY_INPUT_4);
	CheckPostEffect(PEID::Aveblur, KEY_INPUT_5);
	CheckPostEffect(PEID::Gaussian, KEY_INPUT_6);
	CheckPostEffect(PEID::Dof, KEY_INPUT_7);
	CheckPostEffect(PEID::VolFog, KEY_INPUT_8);
	CheckPostEffect(PEID::CtoM, KEY_INPUT_9);
#endif
	// 一部のポストエフェクトはUpdateが必要なのでそのポストエフェクトがtrueならまわす
	for (const auto& post : PostEffect_)
	{
		if (post.second)
		{
			post.first->Update(delta);
		}
	}
}

void PEManager::Draw(int beforeScr, int afterScr, int depth,int skyScr, int redScr)
{
	// ポストエフェクトを使用しているかを調べる
	for (const auto& post : PostEffect_)
	{
		// そのフラグがtrueかを調べる
		if (post.second)
		{
			// 複数ある場合を考えDrawをしてその情報をbeforeScrに格納する
			beforeScr = post.first->Draw(beforeScr,afterScr,depth,skyScr,redScr);
		}
	}
	// メインのスクリーンに切り替え
	SetDrawScreen(afterScr);
	ClsDrawScreen();
	// ポストエフェクトを適用したものを描画する
	DrawGraph(0, 0, beforeScr, true);
}

void PEManager::SetBuffer(int buff)
{
	// シェーダで使うバッファを格納する
	for (auto post : PostEffect_)
	{
		post.first->SetBuffer(buff);
	}
}

void PEManager::SetFlag(PEID id, bool flag)
{
	for (auto& post : PostEffect_)
	{
		// post.firstとidが一致するか調べる
		if (post.first->GetPEID() == id)
		{
			// tureだったらflagを格納する
			post.second = flag;
		}
	}
	/*auto itr = std::find_if(PostEffect_.begin(), PostEffect_.end(), [id](std::pair<std::shared_ptr<PEBase>, bool>& p) { return id == p.first->GetPEID(); });
	if (itr != PostEffect_.end())
	{
		itr->second = flag;
	}*/
}

bool PEManager::GetFlag(PEID id)
{
	// PostEffect_の中身の数だけまわす
	for (const auto& post : PostEffect_)
	{
		// idとpost.firstが同じPEIDかを調べる
		if (post.first->GetPEID() == id)
		{
			// 同じ場合はセットされているFlagを渡す
			return post.second;
		}
	}
	return false;
}

void PEManager::CheckPostEffect(PEID id, int hitkey)
{
	// キーが押されたかを取得する
	if (CheckHitKey(hitkey))
	{
		// idに対応するフラグがfalseかどうか調べる
		if (!usePE_[static_cast<size_t>(id)])
		{
			// falseだったのでtrueにする
			usePE_[static_cast<size_t>(id)] = true;
			// そのidのフラグを取得する
			bool ownFlag = GetFlag(id);
			if (ownFlag)
			{
				// tureだった場合はfalseに
				SetFlag(id, false);
			}
			else
			{
				// falseだった場合はtrueにする
				SetFlag(id, true);
			}
		}
	}
	// キーが押されていないでフラグがtrueだった場合は
	else if(usePE_[static_cast<size_t>(id)])
	{
		// falseに変える
		usePE_[static_cast<size_t>(id)] = false;
	}
}

void PEManager::ShaderLoad(void)
{
	// 使用するシェーダを格納する
	postPS_[0] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/mono.pso");
	postPS_[1] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/aveblur.pso");
	postPS_[2] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/gauss.pso");
	postPS_[3] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/Dof/dof.pso");
	postPS_[4] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/volFog.pso");
	postPS_[5] = LoadPixelShader(L"Resource/resource/Shader/PostEffect/baw.pso");
}

void PEManager::FlagSetFalse(void)
{
	// すべてのフラグをfalseにする
	for (auto& post : PostEffect_)
	{
		post.second = false;
	}
}
