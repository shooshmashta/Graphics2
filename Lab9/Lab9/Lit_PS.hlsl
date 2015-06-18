#define NUM_LIGHTS 4


cbuffer LightColors
{
	float4 diffuseColor[4];
};


#pragma pack_matrix(row_major)
struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	//world pos here
	//add to const buffer
	float3 lightPos[4] : LIGHTCOORD;
};


Texture2D baseTexture : register(t0);
SamplerState filters : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 texCol;
	float lightIntensity[NUM_LIGHTS];
	float4 colorAll, color[NUM_LIGHTS];


	/*input.lightPos[0] = normalize(input.lightPosition[0].xyz - worldpos.xyz);
	input.lightPos[1] = normalize(input.lightPosition[1].xyz - worldpos.xyz);
	input.lightPos[2] = normalize(input.lightPosition[2].xyz - worldpos.xyz);
	input.lightPos[3] = normalize(input.lightPosition[3].xyz - worldpos.xyz);
*/



	// Calculate the different amounts of light on this pixel based on the positions of the lights.
	lightIntensity[0] = saturate(dot(input.normal, input.lightPos[0]));
	lightIntensity[1] = saturate(dot(input.normal, input.lightPos[1]));
	lightIntensity[2] = saturate(dot(input.normal, input.lightPos[2]));
	lightIntensity[3] = saturate(dot(input.normal, input.lightPos[3]));

	// Determine the diffuse color amount of each of the four lights.
	color[0] = diffuseColor[0] * lightIntensity[0];
	color[1] = diffuseColor[1] * lightIntensity[1];
	color[2] = diffuseColor[2] * lightIntensity[2];
	color[3] = diffuseColor[3] * lightIntensity[3];

	// Sample the texture pixel at this location.
	texCol = baseTexture.Sample(filters, input.tex.xy);

	// Multiply the texture pixel by the combination of all four light colors to get the final result.
	colorAll = saturate(color[0] + color[1] + color[2] + color[3]) * texCol;
	//colorAll = saturate(color[0]) * texCol; //+color[1] + color[2] + color[3]) * texCol;

	return colorAll;
}