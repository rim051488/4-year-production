//struct PSInput
//{
//    float4 dif : COLOR0; // ディフューズカラー
//    float2 uv : TEXCOORD0; // テクスチャ座標
//    float4 svpos : SV_POSITION; // 座標( プロジェクション空間 )
//};

struct PSInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float4 tex0 : TEXCOORD1;
    float4 tex1 : TEXCOORD2;
    float4 tex2 : TEXCOORD3;
    float4 tex3 : TEXCOORD4;
    float4 tex4 : TEXCOORD5;
    float4 tex5 : TEXCOORD6;
    float4 tex6 : TEXCOORD7;
    float4 tex7 : TEXCOORD8;
    float4 svpos : SV_POSITION;
};

struct PSOutput
{
    float4 color : SV_TARGET;
};

cbuffer Blur : register(b5)
{
    float4 weight[2];//重み
}

SamplerState texsam : register(s0);
Texture2D<float4> tex : register(t0);

PSOutput main(PSInput input)
{
    PSOutput output;
    float4 color;
    //output.color = tex.Sample(texsam, input.uv);
    color = weight[0].x * tex.Sample(texsam, input.tex0.xy);
    color += weight[0].y * tex.Sample(texsam, input.tex1.xy);
    color += weight[0].z * tex.Sample(texsam, input.tex2.xy);
    color += weight[0].w * tex.Sample(texsam, input.tex3.xy);
    color += weight[1].x * tex.Sample(texsam, input.tex4.xy);
    color += weight[1].y * tex.Sample(texsam, input.tex5.xy);
    color += weight[1].z * tex.Sample(texsam, input.tex6.xy);
    color += weight[1].w * tex.Sample(texsam, input.tex7.xy);
    ////output.color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    
    // 基準テクセルにマイナス方向に8テクセル、重み付きでサンプリングする
    color += weight[0].x * tex.Sample(texsam, input.tex0.zw);
    color += weight[0].y * tex.Sample(texsam, input.tex0.zw);
    color += weight[0].z * tex.Sample(texsam, input.tex0.zw);
    color += weight[0].w * tex.Sample(texsam, input.tex0.zw);
    color += weight[1].x * tex.Sample(texsam, input.tex0.zw);
    color += weight[1].y * tex.Sample(texsam, input.tex0.xw);
    color += weight[1].z * tex.Sample(texsam, input.tex0.zw);
    color += weight[1].w * tex.Sample(texsam, input.tex0.zw);
    
    output.color = float4(color.xyz, 1.0f);
    return output;

}