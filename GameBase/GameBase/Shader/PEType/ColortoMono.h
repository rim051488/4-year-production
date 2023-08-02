#pragma once
#include "../PEBase.h"
class ColortoMono :
    public PEBase
{
public:
    ColortoMono(int postPS, Vector2 pos, Vector2 rate);
    ~ColortoMono();
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)override;
    int timeBuff_;
    float* time_;
    float time;

    PEID GetPEID(void)override { return PEID::CtoM; }
};

