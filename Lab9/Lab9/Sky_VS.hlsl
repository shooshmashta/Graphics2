#pragma pack_matrix(row_major)

struct V_IN
{
	float4 posL : POSITION;
	float4 tex : UVS;
};

struct V_OUT
{
	float4 posH : SV_POSITION;
	float3 tex : UVS;
};

cbuffer SCENE : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};


V_OUT main(V_IN input)
{
	V_OUT output;
	float4 pos = float4(input.posL.xyz, 1);

	pos = mul(pos, worldMatrix);
	pos = mul(pos, viewMatrix);
	pos = mul(pos, projectionMatrix);

	output.tex = input.posL.xyz;
	output.posH = pos;

	return output;
}