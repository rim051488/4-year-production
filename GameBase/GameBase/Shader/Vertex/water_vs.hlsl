struct VS_INPUT
{
	float3 pos : POSITION;//���W
	float3 norm : NORMAL;//�@��
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 tangent:TANGENT;//�ڃx�N�g��
	float3 binormal:BINORMAL;//�]�@���x�N�g��
};

struct VS_OUTPUT
{
	float4 svpos:SV_POSITION;
	float3 pos:POSITION0;
	float4 diffuse:COLOR;
	float3 normal:NORMAL;
	float2 uv:TECOORD0;
	float3 viewVec : TEXCOORD1;
};

cbuffer BaseCBuffer : register(b1)
{
	matrix AntiViewportM;//4x4�r���[�|�[�g�t�s��
	matrix ProjectionM;//4x4�v���W�F�N�V�����s��
	float4x3 ViewM;//4x3(�r���[�s��)
	float4x3 LocalM;//4x3(��]�g�k���s�ړ�)
	float4		ToonOutLineSize;						// �g�D�[���̗֊s���̑傫��
	float		DiffuseSource;							// �f�B�t���[�Y�J���[( 0.0f:�}�e���A��  1.0f:���_ )
	float		SpecularSource;						// �X�y�L�����J���[(   0.0f:�}�e���A��  1.0f:���_ )
	float		MulSpecularColor;						// �X�y�L�����J���[�l�ɏ�Z����l( �X�y�L�������������Ŏg�p )
	float		Padding;//�l�ߕ�(����)
}

cbuffer cBuff : register(b7)
{
	float cnt;			//�^�C��
	float gravity;		//�d��
	float pi;			//��
}

cbuffer cBuffFloat4 : register(b8)
{
	float4 cPos;	//�J�����ʒu(���[���h)
}


//�Q���X�g�i�[�g
// ���[�J���ʒu, ����, �g�̒��_����, ����, �����Q, �g�̕���
void GerstnerWave(in float3 localPos,float t,float waveLength,float Q,float R,float3 waveDir,inout float3 outPos,inout float3 norm)
{
	//�g�̕���
	float2 dir = waveDir.xz;
	dir = normalize(dir);
	//�U��(�g���ɑ΂��ĐU���͍ő�ł�1/14)
	float ampRate = waveLength / 14.0f;
	//n��/waveLength(�l���傫���قǔg�������Ȃ�)
	float PIperL = 3.0f * pi / waveLength;
	//�ړ����x
	float velocity = sqrt(gravity / PIperL) * t;
	//���_�ʒu�Ɣg�̕����̓���
	float d = dot(dir, localPos.xz);
	//�V�[�^
	float theta = PIperL * d + velocity;
	float3 gPos = float3(0.0f, 0.0f, 0.0f);
	//Q��R�͂��ꂼ��g�̋����Ɋ֌W
	gPos.xz =  Q * ampRate * dir * cos(theta);
	gPos.y =  R * ampRate * sin(theta);

	//�@��
	float3 normal = float3(0.0f, 1.0f, 0.0f);
	normal.xz  = (-dir * R * cos(theta)) /
		(7.0f / pi - Q * dir * dir * sin(theta));
	normal = saturate(normal);

	outPos.xyz += gPos.xyz;
	norm += normalize(normal);
}

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT ret;
	float4 pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 norm = float3(0.0f, 0.0f, 0.0f);
	// ���[�J���ʒu, ����, �g�̒��_����, ����, �����Q, �g�̕���
    GerstnerWave(input.pos, cnt + 2.0f, 0.6f, 0.4f, 0.3f, float3(0.2f, 0.0f, 0.3f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt, 1.2f, 0.3f, 0.5f, float3(-0.4f, 0.0f, 0.7f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt + 3.0f, 1.8f, 0.3f, 0.5f, float3(0.4f, 0.0f, 0.4f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt, 2.2f, 0.4f, 0.4f, float3(-0.3f, 0.0f, 0.6f), pos.xyz, norm);

	pos.xyz += input.pos;

	pos.xyz = mul(pos, LocalM);//���[���h
	ret.pos = pos;
	pos.xyz = mul(pos, ViewM);//�r���[
	pos = mul(pos, ProjectionM);//�v���W�F�N�V����

	//�@�����v�Z
	norm = mul(norm, LocalM);

	ret.svpos = pos;

	ret.uv = input.uv0;
	ret.normal = normalize(norm);
	ret.diffuse = input.diffuse;
	ret.viewVec = cPos - ret.pos.xyz;

	return ret;
}