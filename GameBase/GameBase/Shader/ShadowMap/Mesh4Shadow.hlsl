struct VSInput {
	float3 pos:POSITION;//座標
	float3 norm:NORMAL;//法線
	float4 diff:COLOR0;//ディフューズカラー
	float4 spec:COLOR1;//スペキュラカラー
	float4 uv0:TEXCOORD0;//テクスチャ座標
	float4 uv1:TEXCOORD1;//サブテクスチャ座標
	// スキニングメッシュ
	int4 blendIndices0:BLENDINDICES0;//ボーン初期用float型定数配列インデックス0
	float4 blendWeight0:BLENDWEIGHT0;//ボーン処理用ウエイト値0
};

struct VSOutput {
	float4 pos:POSITION0;
	float4 vpos:POSITION1;
	float3 norm:NORMAL;
	float4 uv:TEXCOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float4 svpos:SV_POSITION;
};

// 基本パラメータ
struct CONST_BUFFER_BASE
{
	float4x4		AntiViewportMat;				// アンチビューポート行列
	float4x4		ProjMat;					// ビュー　→　プロジェクション行列
	float4x3		ViewMat;						// ワールド　→　ビュー行列
	float4x3		LocalWorldMat;					// ローカル　→　ワールド行列

	float4		ToonOutLineSize;						// トゥーンの輪郭線の大きさ
	float		DiffuseSource;							// ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float		SpecularSource;						// スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float		MulSpecularColor;						// スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float		Padding;
};

// その他の行列
struct CONST_BUFFER_OTHERMAT
{
	float4		ShadowMapLightViewProjectionMatrix[3][4];			// シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
	float4		TextureMatrix[3][2];								// テクスチャ座標操作用行列
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct CONST_BUFFER_LOCALWORLDMAT
{
	float4		Mat[54 * 3];					// ローカル　→　ワールド行列
};

// 基本パラメータ
cbuffer BUFFER_BASE				: register(b1)
{
	CONST_BUFFER_BASE				g_Base;
};

// その他の行列
cbuffer BUFFER_OTHERMAT			: register(b2)
{
	CONST_BUFFER_OTHERMAT		g_OtherMat;
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
cbuffer BUFFER_LOCALWORLDMAT	: register(b3)
{
	CONST_BUFFER_LOCALWORLDMAT	g_LocalWorldMat;
};

VSOutput main(VSInput input)
{
	VSOutput output;
	float4 WorldPos;
	float4 ViewPos;
	// ローカル座標をセット
	float4 pos = float4(input.pos.xyz, 1);

	// 複数のフレームのブレンド行列の作成(1つ目のやり方)
	float4 LocalWorldMat[3];
    LocalWorldMat[0] = g_LocalWorldMat.Mat[input.blendIndices0.x + 0] * input.blendWeight0.x;
    LocalWorldMat[1] = g_LocalWorldMat.Mat[input.blendIndices0.x + 1] * input.blendWeight0.x;
    LocalWorldMat[2] = g_LocalWorldMat.Mat[input.blendIndices0.x + 2] * input.blendWeight0.x;

    LocalWorldMat[0] += g_LocalWorldMat.Mat[input.blendIndices0.y + 0] * input.blendWeight0.y;
    LocalWorldMat[1] += g_LocalWorldMat.Mat[input.blendIndices0.y + 1] * input.blendWeight0.y;
    LocalWorldMat[2] += g_LocalWorldMat.Mat[input.blendIndices0.y + 2] * input.blendWeight0.y;

    LocalWorldMat[0] += g_LocalWorldMat.Mat[input.blendIndices0.z + 0] * input.blendWeight0.z;
    LocalWorldMat[1] += g_LocalWorldMat.Mat[input.blendIndices0.z + 1] * input.blendWeight0.z;
    LocalWorldMat[2] += g_LocalWorldMat.Mat[input.blendIndices0.z + 2] * input.blendWeight0.z;

    LocalWorldMat[0] += g_LocalWorldMat.Mat[input.blendIndices0.w + 0] * input.blendWeight0.w;
    LocalWorldMat[1] += g_LocalWorldMat.Mat[input.blendIndices0.w + 1] * input.blendWeight0.w;
    LocalWorldMat[2] += g_LocalWorldMat.Mat[input.blendIndices0.w + 2] * input.blendWeight0.w;

	// ローカル座標をワールド座標にする
	WorldPos.x = dot(pos, LocalWorldMat[0]);
	WorldPos.y = dot(pos, LocalWorldMat[1]);
	WorldPos.z = dot(pos, LocalWorldMat[2]);
	WorldPos.w = 1.0f;

	// ワールド座標をビュー行列にする
	ViewPos.xyz = mul(WorldPos, g_Base.ViewMat);
	ViewPos.w = 1.0f;

	// ローカル座標をワールド座標にする
	output.svpos = mul(ViewPos, g_Base.ProjMat);

	// カメラの情報も取得できるように
	output.vpos = output.svpos;
	
	// 座標のセット
	output.pos = ViewPos;
	return output;
}