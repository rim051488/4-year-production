#pragma once
#include "../PEBase.h"
class AveblurPE :
    public PEBase
{
public:
    AveblurPE(int postPS, Vector2 pos, Vector2 rate);
    ~AveblurPE();
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)override;

    PEID GetPEID(void)override { return PEID::Aveblur; }

};

