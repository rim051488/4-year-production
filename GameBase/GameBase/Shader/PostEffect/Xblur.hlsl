struct VSInput
{
    float3 pos : POSITION0;
    float4 spos : POSITION1;
    float3 norm : NORMAL;
    float3 tan : TANGEN0;
    float3 binorm : BINORMAL0;
    float4 diff : COLOR0;
    float4 spec : COLOR1;
    float2 uv0 : TEXCOORD0;
    float2 uv1 : TEXCOORD1;
};

//struct VSOutput
//{
//    float4 dif : COLOR0; // ディフューズカラー
//    float2 uv : TEXCOORD0; // テクスチャ座標
//    float4 svpos : SV_POSITION; // 座標( プロジェクション空間 )
//};

struct VSOutput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
    float4 tex0 : TEXCOORD1;
    float4 tex1 : TEXCOORD2;
    float4 tex2 : TEXCOORD3;
    float4 tex3 : TEXCOORD4;
    float4 tex4 : TEXCOORD5;
    float4 tex5 : TEXCOORD6;
    float4 tex6 : TEXCOORD7;
    float4 tex7 : TEXCOORD8;
    float4 svpos : SV_POSITION;
};

struct CONST_BUFFER_BASE
{
    float4 AntiViewportMat[4]; // アンチビューポート行列
    float4 ProjMat[4]; // ビュー　→　プロジェクション行列
    float4 ViewMat[3]; // ワールド　→　ビュー行列
    float4 LocalWorldMat[3]; // ローカル　→　ワールド行列

    float4 ToonOutLineSize; // トゥーンの輪郭線の大きさ
    float DiffuseSource; // ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
    float SpecularSource; // スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
    float MulSpecularColor; // スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
    float Padding;
};

// その他の行列
struct CONST_BUFFER_OTHERMAT
{
    float4 ShadowMapLightViewProjectionMatrix[3][4]; // シャドウマップ用のライトビュー行列とライト射影行列を乗算したもの
    float4 TextureMatrix[3][2]; // テクスチャ座標操作用行列
};

// スキニングメッシュ用の　ローカル　→　ワールド行列
struct CONST_BUFFER_LOCALWORLDMAT
{
    float4 Mat[54 * 3]; // ローカル　→　ワールド行列
};

// 基本パラメータ
cbuffer BUFFER_BASE : register(b1)
{
    CONST_BUFFER_BASE g_Base;
};

cbuffer ScrSize : register(b5)
{
    float2 size;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    float4 WorldPos;
    float4 ViewPos;
    
    // 座標変換
    float4 pos = float4(input.pos, 1);
    WorldPos.x = dot(pos, g_Base.LocalWorldMat[0]);
    WorldPos.y = dot(pos, g_Base.LocalWorldMat[1]);
    WorldPos.z = dot(pos, g_Base.LocalWorldMat[2]);
    WorldPos.w = 1.0f;
    
    //ViewPos.xyz = mul(WorldPos, g_Base.ViewMat);
    ViewPos.x = dot(WorldPos, g_Base.ViewMat[0]);
    ViewPos.y = dot(WorldPos, g_Base.ViewMat[1]);
    ViewPos.z = dot(WorldPos, g_Base.ViewMat[2]);
    ViewPos.w = 1.0f;
    
    //output.svpos = mul(ViewPos, g_Base.ProjMat);
    output.svpos.x = dot(ViewPos, g_Base.ProjMat[0]);
    output.svpos.y = dot(ViewPos, g_Base.ProjMat[1]);
    output.svpos.z = dot(ViewPos, g_Base.ProjMat[2]);
    output.svpos.w = dot(ViewPos, g_Base.ProjMat[3]);
    
    output.uv = input.uv0;
    
    // テクスチャサイズを変換
    float2 texSize = float2(0.0f,0.0f);
    float level;
    // 基準テクセルのUV座標を記録
    float2 tex = input.uv0; 
    // 基準テクセルからU座標を+1テクセルずらすためのオフセットを計算する
    output.tex0.xy = float2(1.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+3テクセルずらすためのオフセットを計算する
    output.tex1.xy = float2(3.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+5テクセルずらすためのオフセットを計算する
    output.tex2.xy = float2(5.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+7テクセルずらすためのオフセットを計算する
    output.tex3.xy = float2(7.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+9テクセルずらすためのオフセットを計算する
    output.tex4.xy = float2(9.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+11テクセルずらすためのオフセットを計算する
    output.tex5.xy = float2(11.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+13テクセルずらすためのオフセットを計算する
    output.tex6.xy = float2(13.0f / size.x, 0.0f);
    
    // 基準テクセルからU座標を+15テクセルずらすためのオフセットを計算する
    output.tex7.xy = float2(15.0f / size.x, 0.0f);
    
    // オフセットに-1をかけてマイナス方向のオフセットも計算する
    output.tex0.zw = output.tex0.xy * -1.0f;
    output.tex1.zw = output.tex1.xy * -1.0f;
    output.tex2.zw = output.tex2.xy * -1.0f;
    output.tex3.zw = output.tex3.xy * -1.0f;
    output.tex4.zw = output.tex4.xy * -1.0f;
    output.tex5.zw = output.tex5.xy * -1.0f;
    output.tex6.zw = output.tex6.xy * -1.0f;
    output.tex7.zw = output.tex7.xy * -1.0f;
    // オフセットに基準テクセルのUV座標を足し算して、実際にサンプリングするUV座標に変換
    output.tex0 += float4(tex, tex);
    output.tex1 += float4(tex, tex);
    output.tex2 += float4(tex, tex);
    output.tex3 += float4(tex, tex);
    output.tex4 += float4(tex, tex);
    output.tex5 += float4(tex, tex);
    output.tex6 += float4(tex, tex);
    output.tex7 += float4(tex, tex);
	return output;
}