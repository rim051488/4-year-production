struct PSInput
{
    float4 diff : COLOR0;
    float4 spec : COLOR1;
    float2 uv : TEXCOORD0;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

SamplerState texsam : register(s0);
Texture2D<float4> tex : register(t0);
PSOutput main(PSInput input)
{
    PSOutput output;
    float4 color;
    color = float4(tex.Sample(texsam, input.uv));
    // 平均ブラー
    // 基準テクセル+近傍8テクセルの平均を計算する
    // 2.5テクセルずらすためのUV値を求める
    float offsetU = 1.5f / 1280;
    float offsetV = 1.5f / 720;
    
    // 基準テクセルから右のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(offsetU, 0.0f)));
    
    // 基準テクセルから左のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(-offsetU, 0.0f)));
    
    // 基準テクセルから下のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(0.0f, offsetV)));
    
    // 基準テクセルから上のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(0.0f, -offsetV)));
    
    // 基準テクセルから右下のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(offsetU, offsetV)));
    
    // 基準テクセルから右上のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(offsetU, -offsetV)));
    
    // 基準テクセルから左下のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(-offsetU, offsetV)));
    
    // 基準テクセルから左上のテクセルのカラーをサンプリングする
    color += float4(tex.Sample(texsam, input.uv + float2(-offsetU, -offsetV)));
    
    // 基準テクセルと近傍8テクセルの平均なので9で除算する
    color /= 9.0f;
    
    output.color = color;
    return output;
}