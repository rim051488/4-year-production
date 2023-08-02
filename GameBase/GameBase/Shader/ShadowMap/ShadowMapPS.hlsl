struct PSInput {
	float4 pos:POSITION0;
	float3 norm:NORMAL;
	float4 uv:TEXCOORD;
	float4 diff:COLOR0;
	float4 spec:COLOR1;
	float4 svpos:SV_POSITION;
};

struct PSOutput {
	float4 color:SV_TARGET0;
};

PSOutput main(PSInput input)
{
	PSOutput output;
	// Z値を色として出力
    float z = input.svpos.z / input.svpos.w;
    output.color.xyz = float3(z, z, z);
	//output.color = input.svpos.z / input.svpos.w;
	// 透明にならないようにアルファは必ず1
	output.color.a = 1.0f;
	return output;
}