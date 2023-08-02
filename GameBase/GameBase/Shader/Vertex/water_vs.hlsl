struct VS_INPUT
{
	float3 pos : POSITION;//座標
	float3 norm : NORMAL;//法線
	float4 diffuse : COLOR0;
	float4 specular : COLOR1;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float3 tangent:TANGENT;//接ベクトル
	float3 binormal:BINORMAL;//従法線ベクトル
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
	matrix AntiViewportM;//4x4ビューポート逆行列
	matrix ProjectionM;//4x4プロジェクション行列
	float4x3 ViewM;//4x3(ビュー行列)
	float4x3 LocalM;//4x3(回転拡縮平行移動)
	float4		ToonOutLineSize;						// トゥーンの輪郭線の大きさ
	float		DiffuseSource;							// ディフューズカラー( 0.0f:マテリアル  1.0f:頂点 )
	float		SpecularSource;						// スペキュラカラー(   0.0f:マテリアル  1.0f:頂点 )
	float		MulSpecularColor;						// スペキュラカラー値に乗算する値( スペキュラ無効処理で使用 )
	float		Padding;//詰め物(無視)
}

cbuffer cBuff : register(b7)
{
	float cnt;			//タイム
	float gravity;		//重力
	float pi;			//π
}

cbuffer cBuffFloat4 : register(b8)
{
	float4 cPos;	//カメラ位置(ワールド)
}


//ゲルストナー波
// ローカル位置, 時間, 波の頂点距離, 強さ, 強さ２, 波の方向
void GerstnerWave(in float3 localPos,float t,float waveLength,float Q,float R,float3 waveDir,inout float3 outPos,inout float3 norm)
{
	//波の方向
	float2 dir = waveDir.xz;
	dir = normalize(dir);
	//振幅(波長に対して振幅は最大でも1/14)
	float ampRate = waveLength / 14.0f;
	//nπ/waveLength(値が大きいほど波も強くなる)
	float PIperL = 3.0f * pi / waveLength;
	//移動速度
	float velocity = sqrt(gravity / PIperL) * t;
	//頂点位置と波の方向の内積
	float d = dot(dir, localPos.xz);
	//シータ
	float theta = PIperL * d + velocity;
	float3 gPos = float3(0.0f, 0.0f, 0.0f);
	//QとRはそれぞれ波の強さに関係
	gPos.xz =  Q * ampRate * dir * cos(theta);
	gPos.y =  R * ampRate * sin(theta);

	//法線
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
	// ローカル位置, 時間, 波の頂点距離, 強さ, 強さ２, 波の方向
    GerstnerWave(input.pos, cnt + 2.0f, 0.6f, 0.4f, 0.3f, float3(0.2f, 0.0f, 0.3f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt, 1.2f, 0.3f, 0.5f, float3(-0.4f, 0.0f, 0.7f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt + 3.0f, 1.8f, 0.3f, 0.5f, float3(0.4f, 0.0f, 0.4f), pos.xyz, norm);
    GerstnerWave(input.pos, cnt, 2.2f, 0.4f, 0.4f, float3(-0.3f, 0.0f, 0.6f), pos.xyz, norm);

	pos.xyz += input.pos;

	pos.xyz = mul(pos, LocalM);//ワールド
	ret.pos = pos;
	pos.xyz = mul(pos, ViewM);//ビュー
	pos = mul(pos, ProjectionM);//プロジェクション

	//法線も計算
	norm = mul(norm, LocalM);

	ret.svpos = pos;

	ret.uv = input.uv0;
	ret.normal = normalize(norm);
	ret.diffuse = input.diffuse;
	ret.viewVec = cPos - ret.pos.xyz;

	return ret;
}