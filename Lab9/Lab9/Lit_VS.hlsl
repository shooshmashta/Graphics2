#pragma pack_matrix(row_major)

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer LightPositionBuffer
{
	//add to ps with all the other light stuff

	float4 lightPosition[4];
};

struct V_IN
{
	float4 position : POSITION;
	float2 tex : UVS;
	float3 normal : NORMS;

};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//float light : LIGHT;
	//float4 worldPos : WORLD
	//add to PS const buffer
	float3 lightPos[4] : LIGHTCOORD;

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


	//add to PS instead of here
	output.normal = mul(input.normal, (float3x3) world);
	output.normal = -normalize(output.normal);
	//add to PS instead of here
	worldpos = mul(outputPos, world);
	//output.worldPos = mul(outputPos, world);
	output.lightPos[0] = normalize(lightPosition[0].xyz - worldpos.xyz);
	output.lightPos[1] = normalize(lightPosition[1].xyz - worldpos.xyz);
	output.lightPos[2] = normalize(lightPosition[2].xyz - worldpos.xyz);
	output.lightPos[3] = normalize(lightPosition[3].xyz - worldpos.xyz);

	return output;
}