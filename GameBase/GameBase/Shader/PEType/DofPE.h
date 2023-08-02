#pragma once
#include <array>
#include "../PEBase.h"
class DofPE :
    public PEBase
{
public:
    DofPE(int postPS, Vector2 pos, Vector2 rate);
    ~DofPE();
private:
    void Update(float delta) override;
    int Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)override;

    /// <summary>
    /// 横ブラーの描画
    /// </summary>
    /// <param name="beforeScr">元の画像</param>
    void DrawXblur(int beforeScr);

    /// <summary>
    /// 縦ブラーの描画
    /// </summary>
    /// <param name="beforeScr">横ブラーの画像</param>
    void DrawYblur();

    PEID GetPEID(void)override { return PEID::Dof; }

    // 元のスクリーン画像に変換
    int scr1_;
    int scr2_;

    // 頂点シェーダ
    std::array<int, 2> VS_;
    int ps_;
};

