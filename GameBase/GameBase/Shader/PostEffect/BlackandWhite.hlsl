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

cbuffer Magnification:register(b6)
{
    float mag;
}

SamplerState texsam : register(s0);
Texture2D<float4> tex : register(t0);
PSOutput main(PSInput input)
{
    PSOutput output;
    // テクスチャのカラーを取得
    output.color = float4(tex.Sample(texsam, input.uv.xy));
    float mag_ = 0.0f;
    float r, g, b;
    // だんだんモノクロになっていくように処理をする
    r = abs((1.2f- mag)) *  output.color.x + (0.587f * mag) * output.color.y + (0.114f * mag) * output.color.z;
    g = abs((1.5f- mag)) * output.color.y + (0.299f * mag) * output.color.x + (0.114f * mag) * output.color.z;
    b = abs((1.1f- mag)) * output.color.z + (0.299f * mag) * output.color.x + (0.587f * mag) * output.color.y;
    output.color.x = r;
    output.color.y = g;
    output.color.z = b;
    return output;
}