
// Vertex Shader(VS) �Է�
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float4 mColor : COLOR;
    float3 mNormal : NORMAL;
};

// Vertex Shader(VS) ���
//SV_POSITION�� SV�� SystemValue�� �����̴�.
// HLSL�󿡼� �̹� ����Ǿ� �ִ� �̸��̰� ������ ���������� �󿡼��� �ǹ̰� �������ִ�.
struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float2 mUV : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float4 mDiffuse : COLOR;
    float3 mReflection : NORMAL;
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

// Texture, Sampler 
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

// IA - VS - RS - PS - OM
// Vertex Shader(VS) ���α׷���
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // ������Ʈ ���� ��ȯ
    output.mPosition = mul(input.mPosition, World);

    // �� ����, ������Ʈ���� ī�޶� ���� ��� ( ���� )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPosition.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // ������Ʈ ī�޶� ��ȯ, ���� ��ȯ
    output.mPosition = mul(output.mPosition, View);
    output.mPosition = mul(output.mPosition, Projection);
    
    // ������Ʈ ���忡�� �븻 ���� ��� (������Ʈ�� ���鿡 90���� �̷�� ����)
    float3 worldNormal = mul(input.mNormal, (float3x3) World);
    worldNormal = normalize(worldNormal);
    
    // ���ݻ�(Diffuse) �������� ���ϱ�
    // �ݻ籤(Reflection) ������Ʈ�� �븻 ���͸� ���� ���ͷ� �Ͽ� ���� �Ի籤�� �ݻ籤���� ��ȯ
    output.mDiffuse = dot(-lightDir, worldNormal);
    output.mReflection = reflect(lightDir, worldNormal);
    
    // �ؽ�ó�� ������ ���� UV�� ����
    output.mUV = input.mUV;
    
    return output;
}

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    // �ؽ�ó
    float4 color = texture0.Sample(sampler0, input.mUV);
    
    // saturate(0~1�� �Ѿ�� ���� �߶�)
    float3 diffuse = saturate(input.mDiffuse) * color * LightColor;
    
    // �ݻ籤�� ��ֶ�����(����ȭ), ������Ʈ���� ī�޶������ �Ÿ� ��ֶ�����(����ȭ)
    float3 reflection = normalize(input.mReflection);
    float3 viewDir = normalize(input.mViewDir);
    float3 specular = 0;
    
    // �ݻ籤�� ī�޶�(����) ���� �����Ͽ� �������� �� ���� ��
    // SpecularPower ����ŭ �����Ͽ� ���� Ȯ�� ����
    if (diffuse.x > 0)
    {
        specular = saturate(dot(reflection, -viewDir));
        specular = pow(specular, SpecularPower) * (float3) color * (float3) LightColor;
    }
    
    // Ambient(�ֺ���)
    //float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 ambient = AmbientColor * (float3) color * 0.7f;
    
    // (���ݻ籤 + ������ + �ֺ���)
    return float4(diffuse + specular + ambient, 1);
}