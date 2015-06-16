cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix proj;
};

cbuffer LightPositionBuffer
{
	float4 lightPosition[4];
};

struct V_IN
{
	float4 position : POSITION;
	float3 tex : UVS;
	float3 normal : NORMAL;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float3 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 lightPos[4] : LIGHTCOORD;

};

PS_IN  main(V_IN input)
{
	PS_IN output;
	float4 worldpos;

	input.position.w = 1.0f;

	output.position = mul(input.position, world);
	output.position = mul(output.position, view);
	output.position = mul(output.position, proj);

	output.tex = input.tex;
	output.normal = mul(input.normal, (float3x3) world);
	output.normal = normalize(output.normal);

	worldpos = mul(input.position, world);


	output.lightPos[0].xyz = lightPosition[0].xyz - worldpos.xyz;
	output.lightPos[1].xyz = lightPosition[1].xyz - worldpos.xyz;
	output.lightPos[2].xyz = lightPosition[2].xyz - worldpos.xyz;
	output.lightPos[3].xyz = lightPosition[3].xyz - worldpos.xyz;

	output.lightPos[0] = normalize(output.lightPos[0]);
	output.lightPos[1] = normalize(output.lightPos[1]);
	output.lightPos[2] = normalize(output.lightPos[2]);
	output.lightPos[3] = normalize(output.lightPos[3]);


	return output;
}