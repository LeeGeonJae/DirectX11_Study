
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
    float4 color : COLOR;
	float3 diffuse : NORMAL;
};

// constant buffer
cbuffer ConstantData : register(b0)
{
	matrix World;
	matrix View;
	matrix Projection;
	float4 LightDir[2];
	float4 LightColor[2];
}

// IA - VS - RS - PS - OM
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(input.position, World);
	output.position = mul(output.position, View);
	output.position = mul(output.position, Projection);
    float3 normal = mul(input.normal, (float3x3) World);
    normal = normalize(normal);
    output.diffuse = saturate(dot(normal, (float3) LightDir[0])) * LightColor[0];
    output.diffuse += saturate(dot(normal, (float3) LightDir[1])) * LightColor[1];
	output.uv = input.uv;

	return output;
}

// texture, sampler 
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 PS(VS_OUTPUT input) : SV_Target
{
	float4 color = texture0.Sample(sampler0, input.uv);
    float4 finalColor = float4(input.diffuse, 1.f) * color;
    finalColor.a = 1.f;
	
    return finalColor;
}