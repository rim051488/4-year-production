#include <DxLib.h>
#include "AveblurPE.h"

AveblurPE::AveblurPE(int postPS, Vector2 pos, Vector2 rate) :PEBase(postPS, pos, rate)
{
}

AveblurPE::~AveblurPE()
{
}

void AveblurPE::Update(float delta)
{
}

int AveblurPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	SetPostEffect(beforeScr,-1,-1,-1, postPS_, pos_, rate_);
	return ownScr_;
}
