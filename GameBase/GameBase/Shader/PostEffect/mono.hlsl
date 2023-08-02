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

SamplerState texsam : register(s0);
Texture2D<float4> tex : register(t0);
PSOutput main(PSInput input)
{
    PSOutput output;
    output.color = float4(tex.Sample(texsam, input.uv.xy)); // ;
    //output.color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    // モノクロ化
    float M = 0.299f * output.color.x + 0.587f * output.color.z + 0.114f * output.color.z;
    output.color.x = M;
    output.color.y = M;
    output.color.z = M;
    return output;
}