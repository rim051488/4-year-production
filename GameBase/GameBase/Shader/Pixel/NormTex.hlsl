struct PSInput {
	float4 pos:POSITION0;
    float4 lpos:POSITION1;
    float4 worldpos:POSITION2;
    float3 eyepos:POSITION3;
	float3 norm:NORMAL0;
    float3 cnorm:NORMAL1;
	float2 uv:TECOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float3 tan:TANGENT;
	float3 bin:BINORMAL;
    float4 svpos : SV_POSITION;
};

// ピクセルシェーダーの出力
struct PSOutput
{
    float4 color0 : SV_TARGET0; // 色
};

// マテリアルパラメータ
struct CONST_MATERIAL
{
    float4 diffuse; // ディフューズカラー
    float4 specular; // スペキュラカラー
    float4 ambient_Emissive; // マテリアルエミッシブカラー + マテリアルアンビエントカラー * グローバルアンビエントカラー

    float power; // スペキュラの強さ
    float typeParam0; // マテリアルタイプパラメータ0
    float typeParam1; // マテリアルタイプパラメータ1
    float typeParam2; // マテリアルタイプパラメータ2
};

// フォグパラメータ
struct CONST_FOG
{
    float linearAdd; // フォグ用パラメータ end / ( end - start )
    float linearDiv; // フォグ用パラメータ -1  / ( end - start )
    float density; // フォグ用パラメータ density
    float e; // フォグ用パラメータ 自然対数の低

    float4 color; // カラー
};

// ライトパラメータ
struct CONST_LIGHT
{
    int type; // ライトタイプ( DX_LIGHTTYPE_POINT など )
    int3 padding1; // パディング１

    float3 position; // 座標( ビュー空間 )
    float rangePow2; // 有効距離の２乗

    float3 direction; // 方向( ビュー空間 )
    float fallOff; // スポットライト用FallOff

    float3 diffuse; // ディフューズカラー
    float spotParam0; // スポットライト用パラメータ０( cos( Phi / 2.0f ) )

    float3 specular; // スペキュラカラー
    float spotParam1; // スポットライト用パラメータ１( 1.0f / ( cos( Theta / 2.0f ) - cos( Phi / 2.0f ) ) )

    float4 ambient; // アンビエントカラーとマテリアルのアンビエントカラーを乗算したもの

    float attenuation0; // 距離による減衰処理用パラメータ０
    float attenuation1; // 距離による減衰処理用パラメータ１
    float attenuation2; // 距離による減衰処理用パラメータ２
    float padding2; // パディング２
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
    float4 factorColor; // アルファ値等

    float mulAlphaColor; // カラーにアルファ値を乗算するかどうか( 0.0f:乗算しない  1.0f:乗算する )
    float alphaTestRef; // アルファテストで使用する比較値
    float2 padding1;

    int alphaTestCmpMode; // アルファテスト比較モード( DX_CMP_NEVER など )
    int3 padding2;

    float4 ignoreTextureColor; // テクスチャカラー無視処理用カラー
};

// 頂点シェーダー・ピクセルシェーダー共通パラメータ
cbuffer cbD3D11_CONST_BUFFER_COMMON : register(b0)
{
    CONST_BUFFER_COMMON g_Common;
};

// 基本パラメータ
cbuffer cbD3D11_CONST_BUFFER_PS_BASE : register(b1)
{
    CONST_BUFFER_BASE g_Base;
};

SamplerState texsam : register(s0);
SamplerState depthsam : register(s3);
SamplerComparisonState depthsmp : register(s3);
//SamplerComparisonState depthsmp  		// 深度バッファテクスチャ
//{
//	// sampler state
//    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
//    MaxAnisotropy = 1;
//    AddressU = MIRROR;
//    AddressV = MIRROR;
	
//	// sampler conmparison state
//    ComparisonFunc = GREATER;
//};

Texture2D<float4> tex : register(t0);
Texture2D<float4> norm : register(t1);
Texture2D<float4> spec : register(t2);
Texture2D<float4> depthtex : register(t3);

// 法線の計算
float3 CalcNormal(float3 normal,float3 tan,float3 binrom,float2 uv)
{
    float3 binSpaceNormal = norm.SampleLevel(texsam, uv, 0.0f).rgb;
    binSpaceNormal = (binSpaceNormal * 2.0f) - 1.0f;
    float3 newNormal = tan * binSpaceNormal.x + binrom * binSpaceNormal.y + normal * binSpaceNormal.z;
    return newNormal;
}

// Lambert拡散反射を計算
float3 CalcLambertDiffuse(float3 normal)
{
    return max(0.0f, dot(normal, -g_Common.light[0].direction)) * g_Common.light[0].diffuse + g_Common.light[0].ambient.xyz;
}

// Phong鏡面反射をい計算
float3 CalcPhongSpecular(float3 normal ,float3 eyePos, float3 worldPos)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(g_Common.light[0].direction, normal);
    // 光が当たったサーフェイス～視点に伸びるベクトルを求める
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);
    
    // 鏡面反射の強さを求める
    float t = saturate(dot(refVec, toEye));
    
    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);
    
    // 鏡面反射光を求める
    float3 specular = g_Common.light[0].specular * t;
    return specular;
}

PSOutput main(PSInput input) : SV_TARGET
{
    PSOutput output;
    
    // モデルのテクスチャ
    float4 diffuse = tex.Sample(texsam, input.uv);
    // 法線を計算
    float3 normal = CalcNormal(input.norm, input.tan, input.bin, input.uv);
    
    // Lambert拡散反射光を計算
    float3 diffuseLig = CalcLambertDiffuse(normal);
    
    // Phong鏡面反射を計算
    float3 specLig = CalcPhongSpecular(normal, input.eyepos, input.worldpos.xyz);
    
    // スペキュラマップ
    float specPower = spec.Sample(texsam, input.uv).r;
    
    // 鏡面反射の強さを鏡面反射光に乗算する
    specLig *= specPower;
    
    // 拡散反射 + 鏡面反射 + 環境項を合算して最終的な反射光を計算する
    float3 lig = diffuseLig + specLig + g_Common.light[0].ambient.rgb;
    
    
    output.color0.rgb = diffuse.rgb * lig + g_Common.material.ambient_Emissive.xyz;
    //output.color0.rgb = input.bin;
    output.color0.a = diffuse.a;
    
    float2 shadowUV;
    // 深度テクスチャの座標を算出
    shadowUV.x = (input.lpos.x + 1.0f) * 0.5f;
    // yは上下反転しないといけない
    shadowUV.y = 1.0f - (input.lpos.y + 1.0f) * 0.5f;
	// マッハバンドを起こさないようにするため
    input.lpos.z -= 0.005f;
    
    // 周囲のデータと深度テクスチャの深度を取得
    float comp = 0;
    float U = 1.0f / 2048;
    float V = 1.0f / 2048;
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(0, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(U, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(-U, 0)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(0, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(0, -V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(U, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(-U, V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(U, -V)).r, 0.0f) * 1500 - 0.5f);
    comp += saturate(max(input.lpos.z - depthtex.Sample(depthsam, shadowUV + float2(-U, -V)).r, 0.0f) * 1500 - 0.5f);
	
    // 出したものの平均を取得
    comp = 1 - saturate(comp / 9);
    
	// そのまま入れると黒が強いので、少しだけ薄める
    output.color0.xyz *= comp / 2.0f + 0.6f;

    return output;
}