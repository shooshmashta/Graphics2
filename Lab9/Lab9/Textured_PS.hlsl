#pragma pack_matrix(row_major)

struct Directional
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
}; 

//struct Point
//{
//	float4 dir;
//	
//	float4 ambient;
//	float4 diffuse; 
//
//	float4 pos;
//
//	float range;
//	float3 attentuation;
//	float4 diffuse;
//	
//
//
//
//};

cbuffer cbPerFrame
{
	Directional dLight;
	//Point pLight
};

//Texture2D ObjTexture;
//SamplerState ObjSamplerState;

struct PS_IN
{
	float4 posL : SV_POSITION;
	float2 tex : UVS;
	float3 norm : NORMS;
};

Texture2D baseTexture : register(t0); // first texture
SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(PS_IN input) : SV_TARGET
{
	input.norm = normalize(input.norm);

	float4 diffuse = baseTexture.Sample(filters, input.tex);

		float3 finalColor;

		finalColor = diffuse * dLight.ambient;
		finalColor += saturate(dot(dLight.dir, input.norm) * dLight.diffuse * diffuse);

		return float4(finalColor, diffuse.a);

	
}
