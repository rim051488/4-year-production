struct Input
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float4 svpos : SV_POSITION;
};

float4 BasicPS(Input input) : SV_TARGET
{
    float4 col = float4((float2(0, 1) + input.pos.xy) * 0.5f, 1.0f, 1.0f);
    return float4(input.uv,1.0f,1.0f);
}