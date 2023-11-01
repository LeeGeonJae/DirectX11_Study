
// Vertex Shader(VS) �Է�
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
    float3 mBiTangent : NORMAL2;
};

// Vertex Shader(VS) ���
// SV_POSITION�� SV�� SystemValue�� �����̴�.
// HLSL�󿡼� �̹� ����Ǿ� �ִ� �̸��̰� ������ ���������� �󿡼��� �ǹ̰� �������ִ�.
struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float2 mUV : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float4 mDiffuse : COLOR;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
    float3 mBiTangent : NORMAL2;
};

// ��ǥ ��ȯ Constant Buffer
cbuffer BufferData : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}
// Direction Light Constant Buffer
cbuffer LightData : register(b1)
{
    float4 LightDir;
    float4 LightColor;
    float SpecularPower;
    float3 AmbientColor;
}
// Camera Constant Buffer
cbuffer Camera : register(b2)
{
    float4 CameraPos;
}
cbuffer NormalMap : register(b3)
{
    bool UseNormalMap;
    bool UseSpecularMap;
    bool UseGammaCorrection;
}
cbuffer MeshData : register(b4)
{
    matrix meshWorld;
}

// IA - VS - RS - PS - OM
// Vertex Shader(VS) ���α׷���
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ������Ʈ ���� ��ȯ
    output.mPosition = mul(input.mPosition, meshWorld);

    // �� ����, ������Ʈ���� ī�޶� ���� ��� ( ���� )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPosition.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // ������Ʈ ī�޶� ��ȯ, ���� ��ȯ
    output.mPosition = mul(output.mPosition, View);
    output.mPosition = mul(output.mPosition, Projection);
    
    // ������Ʈ ���忡�� �븻 ���� ��� (������Ʈ�� ���鿡 90���� �̷�� ����)
    output.mNormal = normalize(mul(input.mNormal, (float3x3) meshWorld));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) meshWorld));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) meshWorld));
    
    // ���ݻ�(Diffuse) �������� ���ϱ�
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // �ؽ�ó�� ������ ���� UV�� ����
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D texture0 : register(t0);
Texture2D normal0 : register(t1);
Texture2D specular0 : register(t2);
Texture2D opcity0 : register(t3);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 finalColor = input.mDiffuse * LightColor;
	
    return finalColor;
}
