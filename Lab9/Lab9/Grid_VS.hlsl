#pragma pack_matrix(row_major)

struct V_IN
{
	float3 posL : POSITION;
	float4 colorOut : COLOR;
};

struct V_OUT
{
	float4 Pos : SV_POSITION;
	float4 worldPos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;

	float4 difColor;
	bool hasTexture;
	bool hasNormMap;
};

V_OUT main(float4 inPos : POSITION, float2 inTexCoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT)
{
	V_OUT output;

	output.Pos = mul(inPos, WVP);
	output.worldPos = mul(inPos, World);

	output.normal = mul(normal, World);

	output.tangent = mul(tangent, World);

	output.TexCoord = inTexCoord;

	return output;
}
