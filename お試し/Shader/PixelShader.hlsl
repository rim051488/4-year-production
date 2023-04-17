float4 main() : SV_TARGET
{
    float4 col = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float M = 0.299f * col.x + 0.587f * col.y + 0.114f * col.z;
    col.x = M;
    col.y = M;
    col.z = M;

    return col;
}