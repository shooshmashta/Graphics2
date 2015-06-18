textureCUBE cubetexture;
SamplerState m_sample : register(s0);

struct PS_IN
{
	float4 posL : SV_POSITION;
	float3 tex : UVS;

};


float4 main(PS_IN input) : SV_TARGET
{
	return cubetexture.Sample(m_sample, input.tex);
}