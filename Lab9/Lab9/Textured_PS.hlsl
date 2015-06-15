

struct Light
{
	float3 dir;
	float4 ambient;
	float4 diffuse;
};

cbuffer cbPerFrame
{
	Light light;
};

//Texture2D ObjTexture;
//SamplerState ObjSamplerState;

struct PS_IN
{
	float4 posL : SV_POSITION;
	float3 tex : UVS;
	float3 norm : NORMS;
};

//Texture2DMS<float4, 2> baseTexture : register(t0); // first texture
Texture2D baseTexture : register(t0); // first texture

//texture2D detailTexture : register( t1 ); // second texture

SamplerState filters : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(PS_IN input) : SV_TARGET
{
	input.norm = normalize(input.norm);

	float4 diffuse = baseTexture.Sample(filters, input.tex);

		float3 finalColor;

	finalColor = diffuse * light.ambient;
	finalColor += saturate(dot(light.dir, input.norm) * light.diffuse * diffuse);

	//return diffuse;

	return float4(finalColor, diffuse.a);

	//return baseTexture.Load(input.tex.xy, 0).argb;
}
//float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET
//{
//	return colorFromRasterizer;
//}
