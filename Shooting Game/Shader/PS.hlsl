struct Input
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
    // �ϊ��s��
    matrix mat_;
    Wave waves[100];
}


// 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex : register(t0);
// 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[
SamplerState smp : register(s0);

static int numWaves = 20;
static float steepness = 0.8f;
static float speed = 15.0f;

float4 BasicPS(Input input) : SV_TARGET
{
    //// �Q���X�g�i�[�g
    //float4 col = float4(tex.Sample(smp, input.uv));
    ////for (int i = 0; i < numWaves; i++)
    ////{
    ////    Wave wave = waves[i];
    ////    float wi = 2 / wave.waveLength;
    ////    float Qi = steepness / (wave.amplitude * wi * numWaves);
    ////    float phi = speed * wi;
    ////    float rad = dot(wave.dir, input.pos.xz) * wi + phi;
    ////    col.y += sin(rad) * wave.amplitude;
    ////    col.xz += cos(rad) * wave.amplitude * Qi * wave.dir;
    ////}
    //float M = 0.299f * col.x + 0.587f * col.y + 0.114f * col.z;
    //col.x = M;
    //col.y = M;
    //col.z = M;

    return float4(0, 0, 0, 1);

}