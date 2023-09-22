
struct VS_INPUT
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
	float4 color : COLOR;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float4 Diffuse : COLOR;
	float3 normal : NORMAL;
};

cbuffer TransfromData : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;

	float3 LightDir;
	float4 LightColor;
}

// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(input.position, World);
	output.Diffuse = saturate(dot(-input.normal, LightDir));
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
	output.uv = input.uv;
	output.normal = input.normal;

	return output;
}

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = texture0.Sample(sampler0, input.uv);
	//float4 color;
	//color.r = input.normal.r;
	//color.g = input.normal.g;
	//color.b = input.normal.b;

	return input.Diffuse;
	//return color;
}