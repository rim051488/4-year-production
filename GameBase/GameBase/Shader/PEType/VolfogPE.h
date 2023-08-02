#pragma once
#include <array>
#include "../PEBase.h"
class VolfogPE :
    public PEBase
{
public:
    VolfogPE(int postPS, Vector2 pos, Vector2 rate);
    ~VolfogPE();
    // 縮小させる倍率
    static constexpr float REDUCTION = 2.00f;
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr) override;

    PEID GetPEID(void)override { return PEID::VolFog; }
    // ステージのみを描画
    int stageScr_;
    // 縮小バッファ
    int shrinkScr_;
    // フォグをかけたスクリーン
    int fogScr1_;
    int fogScr2_;
    // フォグのみのシェーダ
    int ps_;
};

