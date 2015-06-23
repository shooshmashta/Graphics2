#pragma pack_matrix(row_major)

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};



struct V_IN
{
	float4 position : POSITION;
	float2 tex : UVS;
	float3 normal : NORMS;
	float4 tangent : TANG;
	//float3 binormal : BINORMAL;


};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPosition: WORLD;
	float4 View : VIEW;
	float4 tangent : TANGENTS;
	//float3 binormal : BINORMAL;
};

GS_OUT  main(V_IN input)
{
	GS_OUT output;

	float4 worldpos;
	float4 outputPos = float4(input.position.xyz, 1.0f);

	outputPos = mul(outputPos, world);
	
	output.worldPosition = outputPos;
	outputPos = mul(outputPos, view);
	output.View = outputPos;
	
	output.position = mul(outputPos, proj);

	output.tex = input.tex;
	
	output.normal = mul(input.normal, (float3x3) world);
	output.tangent = mul(input.tangent, world);
	//output.binormal = mul(input.binormal, (float3x3)world);
	
	return output;
}