struct PSInput
{
    float4 pos : POSITION0;
    float4 vpos : POSITION1;
    float3 norm : NORMAL;
    float4 uv : TEXCOORD;
    float4 diff : COLOR0;
    float4 spec : COLOR1;
    float4 svpos : SV_POSITION;
};

struct PSOutput
{
    float4 color : SV_TARGET0;
};

cbuffer DofArea : register(b6)
{
    float DepthStart;
    float DepthEnd;
    float DepthScope;
}

PSOutput main(PSInput input)
{
    PSOutput output;
    // ”íÊŠE[“x‚Ì”ÍˆÍ“à‚ğ0.0f`1.0f‚É•ÏŠ·
    if (input.pos.z < DepthStart)
    {
        output.color.x = 0.0f;
        output.color.y = 0.0f;
        output.color.z = 0.0f;
    }
    else if (input.pos.z > DepthEnd)
    {
        output.color.x = 1.0f;
        output.color.y = 1.0f;
        output.color.z = 1.0f;
    }
    else
    {
        output.color.x = (input.pos.z - DepthStart) * DepthScope;
        output.color.y = (input.pos.z - DepthStart) * DepthScope;
        output.color.z = (input.pos.z - DepthStart) * DepthScope;
    }
    output.color.w = 1.0f;
    return output;
}