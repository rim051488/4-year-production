struct Output
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float4 svpos : SV_POSITION;
};

Output BasicVS(float4 pos : POSITION,float2 uv : TEXCOORD)
{
    Output output;
    output.pos = pos;
    output.uv = uv;
    output.svpos = pos;
    return output;
}