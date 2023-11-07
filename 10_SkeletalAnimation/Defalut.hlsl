
// Vertex Shader(VS) �Է�
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
    float3 mBiTangent : NORMAL2;
    int4 mBlendIndices : BLENDINDICES;
    float4 mBlendWeights : BLENDWEIGHTS;
};

// Vertex Shader(VS) ���
// SV_POSITION�� SV�� SystemValue�� �����̴�.
// HLSL�󿡼� �̹� ����Ǿ� �ִ� �̸��̰� ������ ���������� �󿡼��� �ǹ̰� �������ִ�.
struct VS_OUTPUT
{
    float4 mPositionProj : SV_POSITION;
    float4 mPositionWorld : POSITION;
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
// ImGui�� �ؽ��� üũ���ִ� Constant Buffer
cbuffer NormalMap : register(b3)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseGammaCorrection;
}
// �ؽ��� ���� �ִ��� üũ�ϴ� Constant Buffer
cbuffer bisTextureMapData : register(b4)
{
    int bIsValidDiffuseMap;
    int bIsValidNormalMap;
    int bIsValidSpecularMap;
    int bIsValidOpcityMap;
    int bIsValidBone;
}

cbuffer MaterialData : register(b5)
{
    float4 baseColor;
}

// �޽� ���� ��ǥ ConstantBuffer
cbuffer MeshData : register(b6)
{
    matrix meshWorld;
}
// �� ������ ��ǥ ConstantBuffer
cbuffer MeshData : register(b7)
{
    matrix MatrixPalleteArray[128];
}

// IA - VS - RS - PS - OM
// Vertex Shader(VS) ���α׷���
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ������Ʈ ���� ��ȯ
    float4 pos = input.mPosition;
    float4x4 matWorldBlended = MatrixPalleteArray[0];
    
    // ���� ������ ���� ���� Ʈ������ ��ġ�� ���缭 ��ġ �ƴϸ� ���� ���� Ʈ������ ��ġ�� ��ġ

        // ���� ���� ��ġ
    matWorldBlended = mul(input.mBlendWeights.x, MatrixPalleteArray[input.mBlendIndices.x]);
    matWorldBlended += mul(input.mBlendWeights.y, MatrixPalleteArray[input.mBlendIndices.y]);
    matWorldBlended += mul(input.mBlendWeights.z, MatrixPalleteArray[input.mBlendIndices.z]);
    matWorldBlended += mul(input.mBlendWeights.w, MatrixPalleteArray[input.mBlendIndices.w]);
    pos = mul(pos, matWorldBlended);
    output.mPositionWorld = pos;
    
    // ndc ���� ���� ��ġ
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.mPositionProj = pos;
    
    // �� ����, ������Ʈ���� ī�޶� ���� ��� ( ���� )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPositionWorld.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // ������Ʈ ���忡�� �븻 ���� ��� (������Ʈ�� ���鿡 90���� �̷�� ����)
    output.mNormal = normalize(mul(input.mNormal, (float3x3) matWorldBlended));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) matWorldBlended));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) matWorldBlended));
    
    // ���ݻ�(Diffuse) �������� ���ϱ�
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // �ؽ�ó�� ������ ���� UV�� ����
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D basecolor : register(t0);
Texture2D texture0 : register(t1);
Texture2D normal0 : register(t2);
Texture2D specular0 : register(t3);
Texture2D opcity0 : register(t4);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 TextureColor = baseColor;
    
    // �ؽ�ó
    if (bIsValidDiffuseMap)
    {
        TextureColor = texture0.Sample(sampler0, input.mUV);
    }
    
    // ���� �ݷ���
    if (UseGammaCorrection)
    {
        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
    }
    
    // �븻
    float3 Normal = input.mNormal;
    float3 lightDir = normalize(LightDir);
    
    float3 NormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
    if (UseNormalMap && bIsValidNormalMap && NormalTangentSpace.x != -1.f)
    {
        // ��� ���� ���ø��Ͽ� ��� ���͸� �����ɴϴ�.
        NormalTangentSpace = normalize(NormalTangentSpace);
        
        // ��� �ʿ��� ������ ���͸� ��� �� ��ǥ�迡�� ���� ��ǥ��� ��ȯ�մϴ�.
        float3x3 WorldTransform = float3x3(input.mTangent, input.mBiTangent, input.mNormal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        
        // ��� ���͸� ����ȭ�մϴ�.
        Normal = normalize(Normal);
        
        // �븻 �� ������ ����
        input.mDiffuse = saturate(dot(-lightDir, Normal));
    }
    
    float4 diffuse = input.mDiffuse * LightColor;
    
    // �ݻ籤�� ��ֶ�����(����ȭ), ������Ʈ���� ī�޶������ �Ÿ� ��ֶ�����(����ȭ)
    float3 viewDir = normalize(input.mViewDir);
    float4 specular = 0;
    
    // �� �� ����
    if (diffuse.x > 0)
    {
        float4 specularMap = UseSpecularMap && bIsValidSpecularMap ? specular0.Sample(sampler0, input.mUV) : float4(1.f, 1.f, 1.f, 1.f);
        if (specularMap.x != -1.f)
            specularMap = float4(1.f, 1.f, 1.f, 1.f);
        
        float3 halfDirection = normalize(lightDir + viewDir);
        float speculardot = saturate(dot(-halfDirection, Normal));
        specular = pow(speculardot, SpecularPower) * LightColor * specularMap;
    }
    
    // ���Ľ�Ƽ ���� ������ �ش� ���Ľ�Ƽ ���� ���İ��� ���� �ȼ� ������
    float alpha = 0.f;
    if (TextureColor.a < 1.f)
    {
        alpha = TextureColor.a;
        clip(alpha < 0.5f ? -1.f : 1.f);
    }
    
    float4 TotalAmbient = float4(AmbientColor, 1) * float4(TextureColor.rgb, alpha) * 0.1f;
    float4 TotalSpecular = specular * float4(TextureColor.rgb, alpha);
    float4 TotalDiffuse = diffuse * float4(TextureColor.rgb, alpha);
    float4 finalColor = float4(TotalDiffuse.rgb + TotalSpecular.rgb + TotalAmbient.rgb, alpha);
    
    // ���� �ݷ���
    if (UseGammaCorrection)
    {
        finalColor.rgb = sqrt(finalColor.rgb);
    }
    
    //(���ݻ籤 + ������ + �ֺ���)
    return finalColor;
}
