
// Vertex Shader(VS) �Է�
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float4 mColor : COLOR;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
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
    float3 vNormal : NORMAL1;
    float3 vTangent : NORMAL2;
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
    output.vNormal = mul(input.mNormal, (float3x3) World);
    output.vTangent = mul(input.mTangent, (float3x3) World);
    
    // ���ݻ�(Diffuse) �������� ���ϱ�
    output.mDiffuse     = dot(-lightDir, output.vNormal);
    
    // �ؽ�ó�� ������ ���� UV�� ����
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D texture0 : register(t0);
Texture2D normal0 : register(t1);
Texture2D specular0 : register(t2);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{ 
    // �ؽ�ó
    float4 TextureColor = texture0.Sample(sampler0, input.mUV);
    
    if (UseGammaCorrection)
    {
        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
    }
    
    // �븻��
    float3 vNormal = normalize(input.vNormal);
    float3 vTangent = normalize(input.vTangent);
    float3 vBiTangent = normalize(cross(vNormal, vTangent));
    float3 lightDir = normalize(LightDir);
    
    if (UseNormalMap)
    {
        // ��� ���� ���ø��Ͽ� ��� ���͸� �����ɴϴ�.
        float3 vNormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
        vNormalTangentSpace = normalize(vNormalTangentSpace);
        
        // ��� �ʿ��� ������ ���͸� ��� �� ��ǥ�迡�� ���� ��ǥ��� ��ȯ�մϴ�.
        float3x3 WorldTransform = float3x3(vTangent, vBiTangent, vNormal);
        vNormal = mul(vNormalTangentSpace, WorldTransform);
        
        // ��� ���͸� ����ȭ�մϴ�.
        vNormal = normalize(vNormal);
        
        // �븻 �� ������ ����
        input.mDiffuse = dot(-lightDir, vNormal);
    }
    
    // saturate(0~1�� �Ѿ�� ���� �߶�)
    float4 diffuse = saturate(input.mDiffuse) * LightColor;
    
    // �ݻ籤�� ��ֶ�����(����ȭ), ������Ʈ���� ī�޶������ �Ÿ� ��ֶ�����(����ȭ)
    float3 viewDir = normalize(input.mViewDir);
    float4 specular = 0;
    
    // �ݻ籤�� ī�޶�(����) ���� �����Ͽ� �������� �� ���� ��
    // SpecularPower ����ŭ �����Ͽ� ���� Ȯ�� ����
    // SpecularMap ����
    if (diffuse.x > 0)
    {
        float4 specularMap = UseSpecularMap ? specular0.Sample(sampler0, input.mUV) : float4(1.f, 1.f, 1.f, 1.f);
        
        float3 halfDirection = normalize(lightDir + viewDir);
        float speculardot = saturate(dot(-halfDirection, vNormal));
        specular = pow(speculardot, SpecularPower) * LightColor * specularMap;
    }
    
    float4 TotalAmbient = float4(AmbientColor, 1) * TextureColor * 0.1f;
    float4 TotalSpecular = specular * TextureColor;
    float4 TotalDiffuse = diffuse * TextureColor;
    float4 finalColor = TotalDiffuse + TotalSpecular + TotalAmbient;
    
    if (UseGammaCorrection)
    {
        finalColor.rgb = sqrt(finalColor.rgb);
    }
    
    // (���ݻ籤 + ������ + �ֺ���)
    return finalColor;
}