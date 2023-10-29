
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
    output.mNormal = normalize(mul(input.mNormal, (float3x3) World));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) World));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) World));
    
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

//Pixel Shader(PS) ���α׷���

//float4 PS(VS_OUTPUT input) : SV_Target
//{
//    // �ؽ�ó
//    float4 TextureColor = texture0.Sample(sampler0, input.mUV);
    
//    // ���� �ݷ���
//    if (UseGammaCorrection)
//    {
//        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
//    }
    
//    // �븻
//    float3 Normal = input.mNormal;
//    float3 lightDir = normalize(LightDir);
    
//    float3 NormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
//    if (UseNormalMap && NormalTangentSpace.x != -1.f)
//    {
//        // ��� ���� ���ø��Ͽ� ��� ���͸� �����ɴϴ�.
//        NormalTangentSpace = normalize(NormalTangentSpace);
        
//        // ��� �ʿ��� ������ ���͸� ��� �� ��ǥ�迡�� ���� ��ǥ��� ��ȯ�մϴ�.
//        float3x3 WorldTransform = float3x3(input.mTangent, input.mBiTangent, input.mNormal);
//        Normal = mul(NormalTangentSpace, WorldTransform);
        
//        // ��� ���͸� ����ȭ�մϴ�.
//        Normal = normalize(Normal);
        
//        // �븻 �� ������ ����
//        input.mDiffuse = saturate(dot(-lightDir, Normal));
//    }
    
//    float4 diffuse = input.mDiffuse * LightColor;
    
//    // �ݻ籤�� ��ֶ�����(����ȭ), ������Ʈ���� ī�޶������ �Ÿ� ��ֶ�����(����ȭ)
//    float3 viewDir = normalize(input.mViewDir);
//    float4 specular = 0;
    
//    // �� �� ����
//    if (diffuse.x > 0)
//    {
//        float4 specularMap = UseSpecularMap ? specular0.Sample(sampler0, input.mUV) : float4(1.f, 1.f, 1.f, 1.f);
//        if (specularMap.x != -1.f)
//            specularMap = float4(1.f, 1.f, 1.f, 1.f);
        
//        float3 halfDirection = normalize(lightDir + viewDir);
//        float speculardot = saturate(dot(-halfDirection, Normal));
//        specular = pow(speculardot, SpecularPower) * LightColor * specularMap;
//    }
    
//    float alpha = 0.f;
//    if (TextureColor.a < 1.f)
//    {
//        alpha = opcity0.Sample(sampler0, input.mUV).a;
//        clip(alpha < 0.5f ? -1.f : 1.f);
//    }
    
//    float4 TotalAmbient = float4(AmbientColor, 1) * float4(TextureColor.rgb, alpha) * 0.1f;
//    float4 TotalSpecular = specular * float4(TextureColor.rgb, alpha);
//    float4 TotalDiffuse = diffuse * float4(TextureColor.rgb, alpha);
//    float4 finalColor = float4(TotalDiffuse.rgb + TotalSpecular.rgb + TotalAmbient.rgb, alpha);
    
//    // ���� �ݷ���
//    if (UseGammaCorrection)
//    {
//        finalColor.rgb = sqrt(finalColor.rgb);
//    }
    
//    //(���ݻ籤 + ������ + �ֺ���)
//    return finalColor;
//}

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    //(���ݻ籤 + ������ + �ֺ���)
    return float4(1.f, 1.f, 1.f, 1.f);
}
