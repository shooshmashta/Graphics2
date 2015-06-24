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
	float4 tangent : TANGENTS;
	//float3 binormal : BINORMAL;
//	float  lightSwitch : LIGHT; 
	//float4 worldPos : WORLD;
};

Texture2D baseTexture[2] : register(t0);
//Texture2D baseTexture : register(t0);
SamplerState filters : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 texColor = baseTexture[0].Sample(filters, input.tex);
	if (texColor.a < 0.1f)
	{
		discard;
	}
	
	float3 normal = normalize(input.normal);
	float ViewDir = normalize(input.View - input.worldPosition);
	float3 location = normalize((-dLight.DirectDirection) + ViewDir);
	float  intensity = max(pow(saturate(dot(normal, location)), 25.0f), 0);

	//clip(texColor.a - 0.25f);


	//normal mapping
	float4 normalMap = baseTexture[1].Sample(filters, input.tex);

	normalMap = (2.0f*normalMap) - 1.0f;

	float3 tangent = normalize(input.tangent - dot(input.tangent, input.normal)*input.normal).xyz;

	float3 binormal = cross(input.normal, tangent);
	 
	float3x3 texSpace = float3x3(tangent, binormal, input.normal);

	normal = normalize(mul(normalMap, texSpace));



//	Directional Light
	float4 DirectionalResult = saturate(dot(location, normal) * texColor) + (dLight.DirectAmbient * texColor) + intensity;

	location = normalize((-pLight.PointDirection) + ViewDir);
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
			PointResult = saturate(PointResult + PointAmbient + intensity);
		}


		location = normalize((-sLight.SpotDirection) + ViewDir);
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

		SpotResult = saturate(SpotResult + SpotAmbient + intensity);
		}

		float4 allColors = (DirectionalResult + PointResult + SpotResult);
		

	return allColors;

}