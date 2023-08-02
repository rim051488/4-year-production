// ピクセルシェーダーの入力
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

// ピクセルシェーダーの出力
struct PSOutput
{
	float4 color0           : SV_TARGET0;	// 色
};

// マテリアルパラメータ
struct CONST_MATERIAL
{
	float4		diffuse;				// ディフューズカラー
	float4		specular;				// スペキュラカラー
	float4		ambient_Emissive;		// マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

	float		power;					// スペキュラの強さ
	float		typeParam0;			// マテリアルタイプパラメータ0
	float		typeParam1;			// マテリアルタイプパラメータ1
	float		typeParam2;			// マテリアルタイプパラメータ2
};

// フォグパラメータ
struct CONST_FOG
{
	float		linearAdd;				// フォグ用パラメータ end / ( end - start )
	float		linearDiv;				// フォグ用パラメータ -1  / ( end - start )
	float		density;				// フォグ用パラメータ density
	float		e;						// フォグ用パラメータ 自然対数の低

	float4		color;					// カラー
};

// ライトパラメータ
struct CONST_LIGHT
{
	int			type;					// ライトタイプ( DX_LIGHTTYPE_POINT など )
	int3		padding1;				// パディング１

	float3		position;				// 座標( ビュー空間 )
	float		rangePow2;				// 有効距離の２乗

	float3		direction;				// 方向( ビュー空間 )
	float		fallOff;				// スポットライト用FallOff

	float3		diffuse;				// ディフューズカラー
	float		spotParam0;			// スポットライト用パラメータ０( cos( Phi / 2.0f ) )

	float3		specular;				// スペキュラカラー
	float		spotParam1;			// スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

	float4		ambient;				// アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの

	float		attenuation0;			// 距離による減衰処理用パラメータ０
	float		attenuation1;			// 距離による減衰処理用パラメータ１
	float		attenuation2;			// 距離による減衰処理用パラメータ２
	float		padding2;				// パディング２
};

// ピクセルシェーダー・頂点シェーダー共通パラメータ
struct CONST_BUFFER_COMMON
{
    CONST_LIGHT light[6]; // ライトパラメータ
    CONST_MATERIAL material; // マテリアルパラメータ
    CONST_FOG fog; // フォグパラメータ
};

// 定数バッファピクセルシェーダー基本パラメータ
struct CONST_BUFFER_BASE
{
	float4		factorColor;			// アルファ値等

	float		mulAlphaColor;			// カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
	float		alphaTestRef;			// アルファテストで使用する比較値
	float2		padding1;

	int			alphaTestCmpMode;		// アルファテスト比較モード( DX_CMP_NEVER など )
	int3		padding2;

	float4		ignoreTextureColor;	// テクスチャカラー無視処理用カラー
};

// 頂点シェーダー・ピクセルシェーダー共通パラメータ
cbuffer cbD3D11_CONST_BUFFER_COMMON					: register(b0)
{
    CONST_BUFFER_COMMON g_Common;
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE				: register(b1)
{
    CONST_BUFFER_BASE g_Base;
};

//Texture
SamplerState texsam : register(s0);
Texture2D<float4> tex:register(t0);
Texture2D<float4> norm : register(t1);
Texture2D<float4> spec : register(t2);
Texture2D<float4> depthtex : register(t3);		// 深度バッファテクスチャ

// 法線の計算
float3 CalcNormal(float3 normal, float3 tan, float3 binrom, float2 uv)
{
    float3 binSpaceNormal = norm.SampleLevel(texsam, uv, 0.0f).rgb;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;
    float3 newNormal = tan * binSpaceNormal.x + binrom * binSpaceNormal.y + normal * binSpaceNormal.z;
    return newNormal;
}

// Lambert拡散反射を計算
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

// Phong鏡面反射を計算
float3 CalcPhongSpecular(float3 normal, float3 eyePos, float3 worldPos)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(g_Common.light[0].direction, normal);
    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);
    
    // 鏡面反射の強さを求める
    float t = saturate(dot(refVec, toEye));
    
    if (t<0.0f)
    {
        t = 0.0;
    }
    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);
    
    // 鏡面反射光を求める
    float3 specular = g_Common.light[0].specular * t * g_Common.material.specular.rgb;
    return specular;
}
// 逆行列に変換
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

// main関数
PSOutput main(PSInput input)
{
	PSOutput output;
	
    // 従法線と接線を求める
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
    
    // 法線を計算
    input.norm = CalcNormal(normal, tan, bin, input.uv.rb);
    
    // Lambert拡散反射を計算
    float3 diffuseLig = CalcLambertDiffuse(input.norm);
	// Phong鏡面反射
    float3 specularLig = CalcPhongSpecular(input.norm,input.eyepos,input.worldpos.xyz);
	
    float3 lig = diffuseLig + specularLig + g_Common.light[0].ambient.rgb;
    float4 texCol = tex.Sample(texsam, input.uv.xy);
	
    output.color0.rgb = texCol.xyz /* * lig + g_Common.material.ambient_Emissive.xyz*/;
    //output.color0.rgb = bin;
    output.color0.a = texCol.a;
	
    float2 shadowMap;
    // 深度テクスチャの座標を算出
    shadowMap.x = (input.lpos.x + 1.0f) * 0.5f;
    // yは上下反転しないといけない
    shadowMap.y = 1.0f - (input.lpos.y + 1.0f) * 0.5f;
	// マッハバンドを起こさないようにするため
    input.lpos.z -= 0.005f;

	// 周囲のデータと深度テクスチャの深度を取得
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
	
    // 出したものの平均を取得
    comp = 1 - saturate(comp / 9);
	
	// そのまま入れると黒が強いので、少しだけ薄める
    output.color0.xyz *= comp / 2.0f + 0.2f;

	return output;
}