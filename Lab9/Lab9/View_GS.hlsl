
struct GS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 worldPosition: WORLD;
	float4 View : VIEW;

};



struct GSOutput
{
	float4 pos : SV_POSITION;
	uint ViewIndex : SV_ViewportArrayIndex;
};

[maxvertexcount(3)]
void main(
	triangle float4 input[3] : SV_POSITION, 
	inout TriangleStream< GSOutput > output
	
)
{
	for (uint i = 0; i < 2; i++)
	{
			GSOutput element;

			element.ViewIndex = i;
		for (uint i = 0; i < 3; i++)
		{
			element.pos = input[i];
			output.Append(element);
		}
	}
}