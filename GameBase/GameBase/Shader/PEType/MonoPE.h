#pragma once
#include "../PEBase.h"

class MonoPE :
    public PEBase
{
public:
    MonoPE(int postPS, Vector2 pos, Vector2 rate);
    ~MonoPE();
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)override;

    PEID GetPEID(void)override { return PEID::Mono; }

};

