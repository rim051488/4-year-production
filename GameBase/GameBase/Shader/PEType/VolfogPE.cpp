#include <DxLib.h>
#include "VolfogPE.h"

#include "../../Common/Debug.h"
VolfogPE::VolfogPE(int postPS, Vector2 pos, Vector2 rate) :PEBase(postPS, pos, rate)
{
	DebugLog("VolumeFog生成開始");
	ps_ = LoadPixelShader(L"Resource/resource/Shader/PostEffect/FtR.pso");
	int x, y;
	GetDrawScreenSize(&x, &y);
	stageScr_ = MakeScreen(x, y,false);
	fogScr1_ = MakeScreen(x, y, false);
	fogScr2_ = MakeScreen(x, y, false);
	int shrinkX = static_cast<int>(x / REDUCTION);
	int shrinkY = static_cast<int>(y / REDUCTION);
	shrinkScr_ = MakeScreen(shrinkX, shrinkY, false);
	DebugLog("VolumeFog生成終了");
}

VolfogPE::~VolfogPE()
{
}

void VolfogPE::Update(float delta)
{
}

int VolfogPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	// ステージのみにかける(フォグのみ)
	SetDrawScreen(stageScr_);
	ClsDrawScreen();
	SetPostEffect(redScr, skyScr, -1, depth, postPS_, pos_, rate_);
	// 縮小バッファを作成
	// 縮小させる
	SetDrawScreen(shrinkScr_);
	ClsDrawScreen();
	shrink_->Shrink(stageScr_, pos_, REDUCTION);
	// 縮小したものを元の大きさに戻す
	SetDrawScreen(fogScr1_);
	ClsDrawScreen();
	shrink_->Shrink(shrinkScr_,pos_,1);

	// メインのスクリーンにフォグをかける(フォグ＋縮小バッファ)
	SetDrawScreen(fogScr2_);
	ClsDrawScreen();
	SetPostEffect(beforeScr, skyScr, fogScr1_, depth, ps_, pos_, rate_);

	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	DrawGraph(0, 0, fogScr2_, false);
	return ownScr_;
}
