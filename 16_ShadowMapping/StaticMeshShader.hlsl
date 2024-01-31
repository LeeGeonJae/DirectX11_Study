
// Vertex Shader(VS) 입력
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
    float3 mBiTangent : NORMAL2;
};

// Vertex Shader(VS) 출력
// SV_POSITION의 SV는 SystemValue의 약자이다.
// HLSL상에서 이미 예약되어 있는 이름이고 렌더링 파이프라인 상에서의 의미가 정해져있다.
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

// 좌표 변환 Constant Buffer
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
// ImGui로 텍스쳐 체크해주는 Constant Buffer
cbuffer NormalMap : register(b3)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseEmissiveMap;
    int UseGammaCorrection;
}
// 텍스쳐 맵이 있는지 체크하는 Constant Buffer
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

// 메시 월드 좌표 ConstantBuffer
cbuffer MeshData : register(b6)
{
    matrix meshWorld;
}
// 본 오프셋 좌표 ConstantBuffer
cbuffer MeshData : register(b7)
{
    matrix MatrixPalleteArray[128];
}

// Normal Distribution Function : 거칠기에 따라 반사율을 작게한다.
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
// Vertex Shader(VS) 프로그래밍
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 오브젝트 월드 변환
    float4 pos = input.mPosition;
    float4x4 matWorldBlended = MatrixPalleteArray[0];
    
    pos = mul(pos, meshWorld);
    output.mPositionWorld = pos;
    
    // ndc 공간 내의 위치
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    output.mPositionProj = pos;
    
    // 빛 방향, 오브젝트에서 카메라 방향 계산 ( 월드 )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPositionWorld.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // 오브젝트 월드에서 노말 벡터 계산 (오브젝트의 정면에 90도를 이루는 벡터)
    output.mNormal = normalize(mul(input.mNormal, (float3x3) meshWorld));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) meshWorld));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) meshWorld));
    
    //output.mPositionWorld.a = 1.f;
    output.mPositionShadow = mul(output.mPositionWorld, ShadowView);
    output.mPositionShadow = mul(output.mPositionShadow, ShadowProjection);
    
    // 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // 텍스처를 입히기 위한 UV값 전달
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

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{
    float3 albedo = baseColor.rgb;
    float roughness = 1.f;
    float metalness = 0.f;
    
    //텍스처
    if (bIsValidDiffuseMap)
        albedo = textureTex.Sample(sampler0, input.mUV).rgb;
    // 러프니스
    if (bIsValidRoughnessMap)
        roughness = roughnessTex.Sample(sampler0, input.mUV).r;
    // 메탈릭
    if (bIsValidMetalnessMap)
        metalness = metalnessTex.Sample(sampler0, input.mUV).r;
    
    if (roughness <= 0)
        roughness = 0.1f;
    
    // 감마 콜렉션
    if (UseGammaCorrection)
    {
        albedo.rgb = albedo.rgb * albedo.rgb;
    }
    
    // 노말
    float3 Normal = input.mNormal;
    float3 lightDir = normalize(LightDir);
    
    float3 NormalTangentSpace = normalTex.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
    if (UseNormalMap && bIsValidNormalMap && NormalTangentSpace.x != -1.f)
    {
        // 노멀 맵을 샘플링하여 노멀 벡터를 가져옵니다.
        NormalTangentSpace = normalize(NormalTangentSpace);
        
        // 노멀 맵에서 가져온 벡터를 노멀 맵 좌표계에서 월드 좌표계로 변환합니다.
        float3x3 WorldTransform = float3x3(input.mTangent, input.mBiTangent, input.mNormal);
        Normal = mul(NormalTangentSpace, WorldTransform);
        
        // 노멀 벡터를 정규화합니다.
        Normal = normalize(Normal);
        
        // 노말 맵 라이팅 적용
        input.mDiffuse = saturate(dot(-lightDir, Normal));
    }
    
    // BRDF
    float3 directLighting = 0.0f;
    {
        float3 Lo = normalize(CameraPos - input.mPositionWorld);
        float cosLo = dot(Normal, Lo);
    
    // 카메라 정반사 방향
        float3 Lr = 2.0 * cosLo * Normal - Lo;
    
    // 프레넬
        float3 F0 = lerp(0.04f, albedo, metalness);
    
    // 빛 ( 빛의 양에 따라 for문 돌리는거 추가해야함 )
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
    
    // 오파시티 맵이 있으면 해당 오파시티 맵의 알파값에 따라 픽셀 버리기
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
    
	//// 그림자처리 부분
	// 광원NDC 좌표계에서의 좌표는 계산해주지 않으므로 계산한다.
    float currentShadowDepth = input.mPositionShadow.z / input.mPositionShadow.w;
	// 광원NDC 좌표계에서의 x(-1 ~ +1) , y(-1 ~ +1)  
    float2 uv = input.mPositionShadow.xy / input.mPositionShadow.w;
	// NDC좌표계 좌표로 샘플링하기위해 Texture 좌표계로 변환한다.
    uv.y = -uv.y; // y는 반대
    uv = uv * 0.5 + 0.5; // -1 에서 1을 0~1로 변환
    
    // 스태틱 메시 hlsl은 그림자 보정 처리 하고 스켈레탈 메시 hlsl은 그림자 보정 처리 안했읍니다
	// ShadowMap에 기록된 Depth가져오기
	// 커버할수 있는 영역이 아니면 처리하지않음
  //  if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
  //  {
  //      float sampleShadowDepth = ShadowTex.Sample(sampler0, uv).r;
		//// currentShadowDepth가 크면 더 뒤쪽에 있으므로 직접광이 차단된다.
  //      if (currentShadowDepth > sampleShadowDepth + 0.001)
  //      {
  //          directLighting = 0.0f;
  //      }
  //  }
    
    // PCF
	// ShadowMap에 기록된 Depth가져오기
	// 커버할수 있는 영역이 아니면 처리하지않음
    if (uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0)
    {
        const float SMAP_SIZE = 4096.f;
        const float SMAP_DX = 1.f / SMAP_SIZE;
        
        // 텍셀 크기
        float percentiLit = 0.0f;
        const float2 offsets[9] =
        {
            float2(-SMAP_DX, -SMAP_DX), float2(0.f, -SMAP_DX), float2(SMAP_DX, -SMAP_DX),
            float2(-SMAP_DX, 0.f), float2(0.f, 0.f), float2(SMAP_DX, 0.f),
            float2(-SMAP_DX, SMAP_DX), float2(0.f, SMAP_DX), float2(SMAP_DX, SMAP_DX)
        };

        // 3x3 상자 필터 패턴, 각 표본마다 4표본 PCF를 수행한다.
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
    
    // 감마 콜렉션
    if (UseGammaCorrection)
    {
        finalColor.rgb = sqrt(finalColor.rgb);
    }
    
    //(난반사광 + 직접광 + 주변광)
    return finalColor;
}