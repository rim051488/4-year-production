struct Input
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float4 svpos : SV_POSITION;
};

// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex : register(t0);
// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerState smp : register(s0);

float4 BasicPS(Input input) : SV_TARGET
{
    float4 col = float4((float2(0, 1) + input.pos.xy) * 0.5f, 1.0f, 1.0f);
    return float4(tex.Sample(smp,input.uv));
}