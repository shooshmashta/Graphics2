#define NUM_LIGHTS 4

struct DirectionalLight
{
	//float4 DirectDirection;
	float3 DirectDirection;
	float  DirectIntenisty;
	float4 DirectColor;
};

struct PointLight
{
	float3 PointPosition;
	float  PointRange;

	float3 PointDirection;
	float  PointIntenisty;
	
	float3 PointAttenuation;
	float  PointPad1;

	float4 PointColor;
	float4 PointAmbient;
};

struct SpotLight
{
	float3 SpotPosition;
	float  SpotRange;

	float3 SpotDirection;
	float  SpotCone;

	float3 SpotAttenuation;
	float SpotPad;

	float4 SpotColor;
};

cbuffer AllofTheLights
{
	DirectionalLight dLight;
	PointLight pLight;
	SpotLight sLight;
};

cbuffer MatrixBuffer
{
	float4x4 world;
	float4x4 view;
	float4x4 proj;
};

#pragma pack_matrix(row_major)
struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
//	float  lightSwitch : LIGHT; 
	float4 worldPos : WORLD;
};


Texture2D baseTexture : register(t0);
SamplerState filters : register(s0);

float4 main(PS_IN input) : SV_TARGET
{

	float3 WorldNorm = normalize(input.worldPos);

	//Directional Light
	float3 DirectionalLightDirection = -normalize(dLight.DirectDirection.xyz);
	float  DirectionalLightRatio = saturate(dot(-DirectionalLightDirection, input.normal));
	float4 DirectionalResult = DirectionalLightRatio * dLight.DirectColor;

	//PointLight
	float3 PointLightDir = normalize(pLight.PointDirection.xyz - input.worldPos.xyz);
	float  PointLightRatio = saturate(dot(PointLightDir, input.normal));
	float4 PointResult = PointLightRatio * pLight.PointColor;

	//SpotLight
	float3 SpotLightDir = normalize(sLight.SpotDirection.xyz - input.worldPos.xyz);
	float SpotSurfaceRatio = saturate(dot(SpotLightDir, sLight.SpotDirection));
	float SpotFactor = (SpotSurfaceRatio > sLight.SpotCone) ? 1 : 0;
	float SpotLightRatio = saturate(dot(SpotLightDir, input.normal));
	float4 SpotResult = SpotFactor * SpotLightRatio * sLight.SpotColor;


	//Specular
	/*float4 SpecViewDir = normalize(worldPos - input.position.xyz);
	float HALFVECTOR = normalize((-SpotLightDir) + SpecViewDir);
	float INTENSITY  = max(pow(saturize(dot(input.normal, normalize(HALFVECTOR)))), 0.0f );
	
	float4 SpecResult = SpotLightRatio * INTENSITY;*/


	return saturate(DirectionalResult + PointResult + SpotResult) * baseTexture.Sample(filters, input.tex.xy);

}