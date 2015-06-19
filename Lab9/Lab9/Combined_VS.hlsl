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
	//float lightSwitch : LIGHT;

};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float lightSwitch : LIGHT;
	float4 worldPos : WORLD;
};

PS_IN  main(V_IN input)
{
	PS_IN output;

	float4 worldpos;
	float4 outputPos = float4(input.position.xyz, 1.0f);

	output.position = mul(outputPos, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;

	output.worldPos = mul(outputPos, world);

	output.normal = mul(input.normal, (float3x3) world);
	output.normal = normalize(output.normal);

//	output.lightSwitch = input.lightSwitch;

	return output;
}