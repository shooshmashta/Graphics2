float4 main(float4 colorFromRasterizer : COLOR) : SV_TARGET
{
	return colorFromRasterizer;
}



struct PS_IN
{
	float4 posL : SV_POSITION;
	float3 tex : UVS;

};

texture2D baseTexture : register(t0); // first texture

//texture2D detailTexture : register( t1 ); // second texture

SamplerState filters[2] : register(s0); // filter 0 using CLAMP, filter 1 using WRAP

// Pixel shader performing multi-texturing with a detail texture on a second UV channel
// A simple optimization would be to pack both UV sets into a single register
float4 main(PS_IN input) : SV_TARGET
{
	
	return baseTexture.Sample(filters[0], input.tex.xy).argb;
}
