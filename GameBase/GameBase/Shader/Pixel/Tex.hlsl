// �s�N�Z���V�F�[�_�[�̓���
struct PSInput
{
	float4 pos:POSITION0;
	float4 lpos:POSITION1;
    float4 worldpos:POSITION2;
    float3 eyepos:POSITION3;
	float3 norm:NORMAL0;
    float3 cnorm:NORMAL1;
	float4 uv:TEXCOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float4 svpos:SV_POSITION;
};

// �s�N�Z���V�F�[�_�[�̏o��
struct PSOutput
{
	float4 color0           : SV_TARGET0;	// �F
};

// �}�e���A���p�����[�^
struct CONST_MATERIAL
{
	float4		diffuse;				// �f�B�t���[�Y�J���[
	float4		specular;				// �X�y�L�����J���[
	float4		ambient_Emissive;		// �}�e���A���G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[

	float		power;					// �X�y�L�����̋���
	float		typeParam0;			// �}�e���A���^�C�v�p�����[�^0
	float		typeParam1;			// �}�e���A���^�C�v�p�����[�^1
	float		typeParam2;			// �}�e���A���^�C�v�p�����[�^2
};

// �t�H�O�p�����[�^
struct CONST_FOG
{
	float		linearAdd;				// �t�H�O�p�p�����[�^ end / ( end - start )
	float		linearDiv;				// �t�H�O�p�p�����[�^ -1  / ( end - start )
	float		density;				// �t�H�O�p�p�����[�^ density
	float		e;						// �t�H�O�p�p�����[�^ ���R�ΐ��̒�

	float4		color;					// �J���[
};

// ���C�g�p�����[�^
struct CONST_LIGHT
{
	int			type;					// ���C�g�^�C�v( DX_LIGHTTYPE_POINT �Ȃ� )
	int3		padding1;				// �p�f�B���O�P

	float3		position;				// ���W( �r���[��� )
	float		rangePow2;				// �L�������̂Q��

	float3		direction;				// ����( �r���[��� )
	float		fallOff;				// �X�|�b�g���C�g�pFallOff

	float3		diffuse;				// �f�B�t���[�Y�J���[
	float		spotParam0;			// �X�|�b�g���C�g�p�p�����[�^�O( cos( Phi / 2.0f ) )

	float3		specular;				// �X�y�L�����J���[
	float		spotParam1;			// �X�|�b�g���C�g�p�p�����[�^�P( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	float4		ambient;				// �A���r�G���g�J���[�ƃ}�e���A���̃A���r�G���g�J���[����Z��������

	float		attenuation0;			// �����ɂ�錸�������p�p�����[�^�O
	float		attenuation1;			// �����ɂ�錸�������p�p�����[�^�P
	float		attenuation2;			// �����ɂ�錸�������p�p�����[�^�Q
	float		padding2;				// �p�f�B���O�Q
};

// �s�N�Z���V�F�[�_�[�E���_�V�F�[�_�[���ʃp�����[�^
struct CONST_BUFFER_COMMON
{
    CONST_LIGHT light[6]; // ���C�g�p�����[�^
    CONST_MATERIAL material; // �}�e���A���p�����[�^
    CONST_FOG fog; // �t�H�O�p�����[�^
};

// �萔�o�b�t�@�s�N�Z���V�F�[�_�[��{�p�����[�^
struct CONST_BUFFER_BASE
{
	float4		factorColor;			// �A���t�@�l��

	float		mulAlphaColor;			// �J���[�ɃA���t�@�l����Z���邩�ǂ���( 0.0f:��Z���Ȃ�  1.0f:��Z���� )
	float		alphaTestRef;			// �A���t�@�e�X�g�Ŏg�p�����r�l
	float2		padding1;

	int			alphaTestCmpMode;		// �A���t�@�e�X�g��r���[�h( DX_CMP_NEVER �Ȃ� )
	int3		padding2;

	float4		ignoreTextureColor;	// �e�N�X�`���J���[���������p�J���[
};

// ���_�V�F�[�_�[�E�s�N�Z���V�F�[�_�[���ʃp�����[�^
cbuffer cbD3D11_CONST_BUFFER_COMMON					: register(b0)
{
    CONST_BUFFER_COMMON g_Common;
};

// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1)
{
    CONST_BUFFER_BASE g_Base;
};

//Texture
SamplerState texsam : register(s0);
Texture2D<float4> tex:register(t0);
Texture2D<float4> norm : register(t1);
Texture2D<float4> spec : register(t2);
Texture2D<float4> depthtex : register(t3);		// �[�x�o�b�t�@�e�N�X�`��

// �@���̌v�Z
float3 CalcNormal(float3 normal, float3 tan, float3 binrom, float2 uv)
{
    float3 binSpaceNormal = norm.SampleLevel(texsam, uv, 0.0f).rgb;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;
    float3 newNormal = tan * binSpaceNormal.x + binrom * binSpaceNormal.y + normal * binSpaceNormal.z;
    return newNormal;
}

// Lambert�g�U���˂��v�Z
float3 CalcLambertDiffuse(float3 normal)
{
    float t = dot(normal, g_Common.light[0].direction);
    t *= -1.0f;
	
    if (t < 0.0f)
    {
        t = 0.0f;
    }

    return t * g_Common.light[0].diffuse * g_Common.material.diffuse.rgb + g_Common.light[0].ambient.rgb;
}

// Phong���ʔ��˂��v�Z
float3 CalcPhongSpecular(float3 normal, float3 eyePos, float3 worldPos)
{
    // ���˃x�N�g�������߂�
    float3 refVec = reflect(g_Common.light[0].direction, normal);
    // �������������T�[�t�F�C�X���王�_�ɐL�т�x�N�g�������߂�
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);
    
    // ���ʔ��˂̋��������߂�
    float t = saturate(dot(refVec, toEye));
    
    if (t<0.0f)
    {
        t = 0.0;
    }
    // ���ʔ��˂̋������i��
    t = pow(t, 5.0f);
    
    // ���ʔ��ˌ������߂�
    float3 specular = g_Common.light[0].specular * t * g_Common.material.specular.rgb;
    return specular;
}
// �t�s��ɕϊ�
float3x3 Invert(float3x3 m)
{
    float3x3 invert;
    invert = 1.0f / determinant(m) *
    float3x3(
            m._22 * m._33 - m._23 * m._32, -(m._12 * m._33 - m._13 * m._32), m._12 * m._23 - m._13 * m._22,
            -(m._21 * m._33 - m._23 * m._31), m._11 * m._33 - m._13 * m._31, -(m._11 * m._23 - m._13 * m._21),
            m._21 * m._32 - m._22 * m._31, -(m._11 * m._32 - m._12 * m._31), m._11 * m._22 - m._12 * m._21
        );
    return invert;
}

// main�֐�
PSOutput main(PSInput input)
{
	PSOutput output;
	
    // �]�@���Ɛڐ������߂�
    float3 dp1 = ddx(input.worldpos.xyz);
    float3 dp2 = ddy(input.worldpos.xyz);
    float2 duv1 = ddx(input.uv.rb);
    float2 duv2 = ddy(input.uv.rb);
    float3x3 M = float3x3(dp1, dp2, cross(dp1, dp2));
    float3x3 UV = float3x3(float3(input.uv.rb, 1.0f), float3(duv1, 0.0f), float3(duv2, 0.0f));
    float3x3 inverseM = float3x3(Invert(M));
    float3x3 derivative = -inverseM * UV;
    float3 tan = normalize(derivative[1]);
    float3 bin = normalize(derivative[2]);
    float3 normal = normalize(derivative[0]);    
    
    // �@�����v�Z
    input.norm = CalcNormal(normal, tan, bin, input.uv.rb);
    
    // Lambert�g�U���˂��v�Z
    float3 diffuseLig = CalcLambertDiffuse(input.norm);
	// Phong���ʔ���
    float3 specularLig = CalcPhongSpecular(input.norm,input.eyepos,input.worldpos.xyz);
	
    float3 lig = diffuseLig + specularLig + g_Common.light[0].ambient.rgb;
    float4 texCol = tex.Sample(texsam, input.uv.xy);
	
    output.color0.rgb = texCol.xyz /* * lig + g_Common.material.ambient_Emissive.xyz*/;
    //output.color0.rgb = bin;
    output.color0.a = texCol.a;
	
    float2 shadowMap;
    // �[�x�e�N�X�`���̍��W���Z�o
    shadowMap.x = (input.lpos.x + 1.0f) * 0.5f;
    // y�͏㉺���]���Ȃ��Ƃ����Ȃ�
    shadowMap.y = 1.0f - (input.lpos.y + 1.0f) * 0.5f;
	// �}�b�n�o���h���N�����Ȃ��悤�ɂ��邽��
    input.lpos.z -= 0.005f;

	// ���͂̃f�[�^�Ɛ[�x�e�N�X�`���̐[�x���擾
    float comp = 0;
    float U = 1.0f / 2048;
    float V = 1.0f / 2048;
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(0, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(U, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(-U, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(0, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(0, -V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(U, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(-U, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(U, -V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(texsam, shadowMap + float2(-U, -V)).r, 0.0f) * 1500 - 0.5f);
	
    // �o�������̂̕��ς��擾
    comp = 1 - saturate(comp / 9);
	
	// ���̂܂ܓ����ƍ��������̂ŁA�����������߂�
    output.color0.xyz *= comp / 2.0f + 0.2f;

	return output;
}