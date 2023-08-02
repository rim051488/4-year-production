struct PSInput
{
    float4 diff : COLOR0; // ディフューズ
    float4 spec : COLOR1; // スペキュラカラー
    float2 uv : TEXCOORD0; // テクスチャ座標
};

struct PSOutput
{
    float4 color : SV_TARGET0; // 色
};

cbuffer DofArea : register(b6)
{
    float DepthStart;
    float DepthEnd;
    float DepthScope;
}


SamplerState sam : register(s0);
SamplerState depthsam : register(s1);
SamplerState skysam : register(s2);
SamplerState bokesam : register(s3);

Texture2D<float4> tex : register(t0);
Texture2D<float4> depth : register(t1);
Texture2D<float4> sky : register(t2);
Texture2D<float4> boke : register(t3);

// フォグをかける範囲を設定
float fadeCreate(float d)
{
    float fade;
    // フォーカス情報から算出
    if (d < DepthStart)
    {
        fade = 1.0f - d / DepthStart;
    }
    else if (d < DepthEnd)
    {
        fade = 0.0f;
    }
    else
    {
        fade = (d - DepthEnd) / (1.0f - DepthEnd);
    }
    return fade;
}

// ぼやけ率から表示する色を設定
float4 depthOfColor(float fade,float4 color1,float4 color2)
{
    float4 color1_,color2_;
    float blend;
    // 取得した範囲から色をとる
    if (fade < 0.5f)
    {
        color1_ = color1;
        color2_ = float4(color2.xyz, fade);
        blend = fade / 0.5f;
    }
    else
    {
        blend = (fade - 0.5f) / 0.5f;
        // この2つの処理が空の色にする
        color1_ = lerp(color1, float4(color2.xyz, 0.5f), 1);
        color2_ = float4(color2.xyz, fade);
    }

    return lerp(color1_, color2_, blend);
}
// メインのスクリーンと縮小バッファとのラープ
float4 lerpColor(float fade, float4 color1,float4 color2)
{
    
    return lerp(color1, color2, 0);;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    float fade;
    float4 color1, texColor, skyColor;
    // 深度値を取得する
    float d = depth.Sample(sam, input.uv);
    fade = fadeCreate(d);
    
    texColor = tex.Sample(sam, input.uv);
    skyColor = sky.Sample(skysam, input.uv);
    color1 = depthOfColor(fade, texColor, skyColor);
    output.color = color1;
    return output;
}
