
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
    float4 mPositionProj : SV_POSITION;
    float4 mPositionWorld : POSITION;
    float4 mPositionShadow : POSITION1;
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
    matrix View;
    matrix Projection;
    Matrix ShadowView;
    Matrix ShadowProjection;
    float3 CameraPos;
}
// Direction Light Constant Buffer
cbuffer LightData : register(b1)
{
    float4 LightDir;
    float4 LightColor;
    float SpecularPower;
    float3 AmbientColor;
}
// ImGui�� �ؽ��� üũ���ִ� Constant Buffer
cbuffer NormalMap : register(b3)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseEmissiveMap;
    int UseGammaCorrection;
}
// �ؽ��� ���� �ִ��� üũ�ϴ� Constant Buffer
cbuffer bisTextureMapData : register(b4)
{
    int bIsValidBaseColor;
    int bIsValidDiffuseMap;
    int bIsValidNormalMap;
    int bIsValidSpecularMap;
    int bIsValidOpcityMap;
    int bIsValidEmissiveMap;
    int bIsValidMetalnessMap;
    int bIsValidRoughnessMap;
}

cbuffer MaterialData : register(b5)
{
    float4 baseColor;
    float4 emissiveColor;
    float OpacityValue;
    float EmissivePower;
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

// Normal Distribution Function : ��ĥ�⿡ ���� �ݻ����� �۰��Ѵ�.
float ndfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphasq = alpha * alpha;
    
    float denominator = 3.141592f * pow((cosLh * cosLh) * (alpha - 1.f) + 1.f, 2);
    return alpha / denominator;
}

float3 FresnelReflection(float costheta, float3 F0)
{
    return F0 + (1.f - F0) * pow((1.f - costheta), 5);
}

// 
float gaSchlickG1(float cosTheta, float k)
{
    float denominator = cosTheta * (1.f - k) + k;
    
    return cosTheta / denominator;
}

// 
float gaSchlickGGX(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return gaSchlickG1(cosLi, k) * gaSchlickG1(cosLo, k);
}

// IA - VS - RS - PS - OM
// Vertex Shader(VS) ���α׷���
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ������Ʈ ���� ��ȯ
    float4 pos = input.mPosition;
    float4x4 matWorldBlended = MatrixPalleteArray[0];
    
    pos = mul(pos, meshWorld);
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
    output.mNormal = normalize(mul(input.mNormal, (float3x3) meshWorld));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) meshWorld));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) meshWorld));
    
    //output.mPositionWorld.a = 1.f;
    output.mPositionShadow = mul(output.mPositionWorld, ShadowView);
    output.mPositionShadow = mul(output.mPositionShadow, ShadowProjection);
    
    // ���ݻ�(Diffuse) �������� ���ϱ�
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // �ؽ�ó�� ������ ���� UV�� ����
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D basecolorTex : register(t0);
Texture2D textureTex : register(t1);
Texture2D normalTex : register(t2);
Texture2D specularTex : register(t3);
Texture2D opcityTex : register(t4);
Texture2D emissiveTex : register(t5);
Texture2D metalnessTex : register(t6);
Texture2D roughnessTex : register(t7);
Texture2D ShadowTex : register(t8);

SamplerState sampler0 : register(s0);
SamplerState BRDFsampler : register(s1);
SamplerComparisonState Shadowsampler : register(s2);

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 albedo = baseColor.rgb;
    float roughness = 1.f;
    float metalness = 0.f;
    
    //�ؽ�ó
    if (bIsValidDiffuseMap)
        albedo = textureTex.Sample(sampler0, input.mUV).rgb;
    // �����Ͻ�
    if (bIsValidRoughnessMap)
        roughness = roughnessTex.Sample(sampler0, input.mUV).r;
    // ��Ż��
    if (bIsValidMetalnessMap)
        metalness = metalnessTex.Sample(sampler0, input.mUV).r;
    
    if (roughness <= 0)
        roughness = 0.1f;
    
    // ���� �ݷ���
    if (UseGammaCorrection)
    {
        albedo.rgb = albedo.rgb * albedo.rgb;
    }
    
    // �븻
    float3 Normal = input.mNormal;
    float3 lightDir = normalize(LightDir);
    
    float3 NormalTangentSpace = normalTex.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
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
    
    // BRDF
    float3 directLighting = 0.0f;
    {
        float3 Lo = normalize(CameraPos - input.mPositionWorld);
        float cosLo = dot(Normal, Lo);
    
    // ī�޶� ���ݻ� ����
        float3 Lr = 2.0 * cosLo * Normal - Lo;
    
    // ������
        float3 F0 = lerp(0.04f, albedo, metalness);
    
    // �� ( ���� �翡 ���� for�� �����°� �߰��ؾ��� )
        {
            float3 Lh = normalize(-lightDir + Lo);

            float cosLi = max(0.0, dot(Normal, -lightDir));
            float cosLh = max(0.0, dot(Normal, Lh));
        
            float3 F = FresnelReflection(max(0.0, dot(Lh, Lo)), F0);
            float D = ndfGGX(cosLh, roughness);
            float G = gaSchlickGGX(cosLi, cosLo, roughness);

            float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metalness);
        
            float3 diffuseBRDF = kd * albedo;
        
            float3 specularBRDF = (F * D * G) / (4.0 * cosLi * cosLo);
        
            directLighting += (diffuseBRDF + specularBRDF) * LightColor.rgb * cosLi;
        }
    }
    
    // ���Ľ�Ƽ ���� ������ �ش� ���Ľ�Ƽ ���� ���İ��� ���� �ȼ� ������
    float alpha = textureTex.Sample(sampler0, input.mUV).a;
    if (alpha > 1.f)
    {
        clip(alpha < OpacityValue ? -1.f : 1.f);
    }
    
    float3 emissive = float3(0.f, 0.f, 0.f);
    if (bIsValidEmissiveMap && UseEmissiveMap)
    {
        emissive = emissiveTex.Sample(sampler0, input.mUV).rgb;
        emissive = emissive * emissiveColor.rgb * EmissivePower;
    }
    
	//// �׸���ó�� �κ�
	// ����NDC ��ǥ�迡���� ��ǥ�� ��������� �����Ƿ� ����Ѵ�.
    float currentShadowDepth = input.mPositionShadow.z / input.mPositionShadow.w;
	// ����NDC ��ǥ�迡���� x(-1 ~ +1) , y(-1 ~ +1)  
    float2 uv = input.mPositionShadow.xy / input.mPositionShadow.w;
	// NDC��ǥ�� ��ǥ�� ���ø��ϱ����� Texture ��ǥ��� ��ȯ�Ѵ�.
    uv.y = -uv.y; // y�� �ݴ�
    uv = uv * 0.5 + 0.5; // -1 ���� 1�� 0~1�� ��ȯ
    
    // ����ƽ �޽� hlsl�� �׸��� ���� ó�� �ϰ� ���̷�Ż �޽� hlsl�� �׸��� ���� ó�� �������ϴ�
	// ShadowMap�� ��ϵ� Depth��������
	// Ŀ���Ҽ� �ִ� ������ �ƴϸ� ó����������
  //  if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
  //  {
  //      float sampleShadowDepth = ShadowTex.Sample(sampler0, uv).r;
		//// currentShadowDepth�� ũ�� �� ���ʿ� �����Ƿ� �������� ���ܵȴ�.
  //      if (currentShadowDepth > sampleShadowDepth + 0.001)
  //      {
  //          directLighting = 0.0f;
  //      }
  //  }
    
    // PCF
	// ShadowMap�� ��ϵ� Depth��������
	// Ŀ���Ҽ� �ִ� ������ �ƴϸ� ó����������
    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
    {
        const float SMAP_SIZE = 4096.f;
        const float SMAP_DX = 1.f / SMAP_SIZE;
        
        // �ؼ� ũ��
        float percentiLit = 0.0f;
        const float2 offsets[9] =
        {
            float2(-SMAP_DX, -SMAP_DX), float2(0.f, -SMAP_DX), float2(SMAP_DX, -SMAP_DX),
            float2(-SMAP_DX, 0.f), float2(0.f, 0.f), float2(SMAP_DX, 0.f),
            float2(-SMAP_DX, SMAP_DX), float2(0.f, SMAP_DX), float2(SMAP_DX, SMAP_DX)
        };

        // 3x3 ���� ���� ����, �� ǥ������ 4ǥ�� PCF�� �����Ѵ�.
        for (int i = 0; i < 9; i++)
        {
            percentiLit += ShadowTex.SampleCmpLevelZero(Shadowsampler, uv, currentShadowDepth).r;
        }
        
        percentiLit /= 9;
        
        directLighting = directLighting * percentiLit;
    }
    
    //float4 TotalAmbient = float4(AmbientColor, 1) * float4(albedo, alpha) * 0.1f;
    float4 TotalAmbient = float4(AmbientColor, 1) * float4(albedo, alpha) * 0.4f;
    float4 finalColor = float4(directLighting + emissive.rgb, 1.f) + TotalAmbient;
    
    // ���� �ݷ���
    if (UseGammaCorrection)
    {
        finalColor.rgb = sqrt(finalColor.rgb);
    }
    
    //(���ݻ籤 + ������ + �ֺ���)
    return finalColor;
}