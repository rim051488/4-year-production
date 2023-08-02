#pragma once
#include "ModelRender.h"

// ライティングとかしない描画をするクラス(スカイドームとか)
class NonLightingRender :
    public ModelRender
{
public:
private:
    void Draw(int shadowMap = -1, int buff = -1, int cubeTex = -1) final;
    void SetUpDepthTex(int ps = -1,int buff = -1) final;
};

