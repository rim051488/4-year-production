#include<DxLib.h>
#include "MonoPE.h"

MonoPE::MonoPE(int postPS, Vector2 pos, Vector2 rate):PEBase(postPS,pos,rate)
{
}

MonoPE::~MonoPE()
{
}

void MonoPE::Update(float delta)
{
}

int MonoPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	SetPostEffect(beforeScr,-1,-1,-1, postPS_, pos_, rate_);
	return ownScr_;
}
