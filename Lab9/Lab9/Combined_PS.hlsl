#define NUM_LIGHTS 4

struct DirectionalLight
{
	//float4 DirectDirection;
	float3 DirectDirection;
	float pad;
	float4  DirectAmbient;
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
	float4 SpotAmbient;
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
	float4 worldPosition: WORLD;
	float4 View : VIEW;
//	float  lightSwitch : LIGHT; 
	//float4 worldPos : WORLD;
};


Texture2D baseTexture : register(t0);
SamplerState filters : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float3 normal = normalize(input.normal);
	float4 texColor = baseTexture.Sample(filters, input.tex);

	float3 location = normalize((-dLight.DirectDirection) + input.View.xyz);
	float  intensity = max(pow(saturate(dot(normal, location)), 25.0f), 0);


	//Directional Light
	float4 DirectionalResult = saturate(dot(location, normal) * texColor) + (dLight.DirectAmbient * texColor) + intensity;


		location = normalize((-pLight.PointDirection) + input.View.xyz);
	intensity = max(pow(saturate(dot(normal, location)), 25.0f), 0);
	////PointLight
	float3 PointPixToLight = (pLight.PointPosition.xyz - input.worldPosition.xyz);
	float  PointD = length(PointPixToLight); 
	float4 PointAmbient = pLight.PointAmbient * texColor;
	float4 PointResult = float4(0, 0,0, 0);
	
		if (PointD > pLight.PointRange)
		{
			PointResult = PointAmbient;
		}
		else
		{

			PointPixToLight = normalize(PointPixToLight);
			float PointLightAmmount = dot(PointPixToLight, normal);

			if (PointLightAmmount > 0.0f)
			{
				PointResult += PointLightAmmount * texColor * pLight.PointColor;
				PointResult /= pLight.PointAttenuation.x + (pLight.PointAttenuation.y * PointD) + (pLight.PointAttenuation.z * PointD * PointD);
			}
			PointResult = saturate(PointResult + PointAmbient) + intensity;
		}


	location = normalize((-sLight.SpotDirection) + input.View.xyz);
	intensity = max(pow(saturate(dot(normal, location)), 25.0f), 0);

	////SpotLight
	float3 SpotPixToLight = (sLight.SpotPosition.xyz - input.worldPosition.xyz);
	float  SpotD = length(SpotPixToLight);
	float4 SpotAmbient = sLight.SpotAmbient * texColor;
	float4 SpotResult = float4(0, 0, 0, 0);

	if (SpotD > sLight.SpotRange)
	{
		SpotResult = SpotAmbient;
	}
	else
	{

		SpotPixToLight = normalize(SpotPixToLight);
		float SpotLightAmmount = dot(SpotPixToLight, normal);

		if (SpotLightAmmount > 0.0f)
		{
			SpotResult += SpotLightAmmount * texColor * sLight.SpotColor;
			
			SpotResult /= sLight.SpotAttenuation.x + (sLight.SpotAttenuation.y * SpotD) + (sLight.SpotAttenuation.z * SpotD * SpotD);

			SpotResult *= pow(max(dot(-SpotPixToLight, sLight.SpotDirection), 0.0f), sLight.SpotCone);
		}

		SpotResult = saturate(SpotResult + SpotAmbient) + intensity;
	}
	

	////Specular
	///*float4 SpecViewDir = normalize(worldPos - input.position.xyz);
	//float HALFVECTOR = normalize((-SpotLightDir) + SpecViewDir);
	//float INTENSITY  = max(pow(saturize(dot(normal, normalize(HALFVECTOR)))), 0.0f );
	//
	//float4 SpecResult = SpotLightRatio * INTENSITY;*/


	return (DirectionalResult + PointResult + SpotResult);

}