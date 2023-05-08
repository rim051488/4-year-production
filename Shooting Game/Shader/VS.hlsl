struct Input
{
    float4 pos : POSITION;
    float4 norm : NORMAL;
    float2 uv : TEXCOORD;
    min16uint2 boneno : BONE_NO;
    min16uint weight : WEIGHT;
};

struct Output
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float4 svpos : SV_POSITION;
};

struct Wave
{
    float2 dir;
    float amplitude;
    float waveLength;
};

cbuffer cbuff : register(b0)
{
    // ïœä∑çsóÒ
    matrix worldMat;
    matrix viewMat;
    Wave waves[100];
}

Output BasicVS(Input input)
{
    Output output;    
    output.pos = mul(mul(worldMat,viewMat),input.pos);
    output.uv = input.uv;
    output.svpos = mul(mul(worldMat, viewMat), input.pos);
    return output;
}