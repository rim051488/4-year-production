#pragma once
#include "../PEBase.h"
class NoPE :
    public PEBase
{
public:
    NoPE(int postPS, Vector2 pos, Vector2 rate);
    ~NoPE();
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)override;

    PEID GetPEID(void)override { return PEID::Default; }
};

