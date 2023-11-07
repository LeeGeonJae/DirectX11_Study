
// Vertex Shader(VS) 입력
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

// Vertex Shader(VS) 출력
// SV_POSITION의 SV는 SystemValue의 약자이다.
// HLSL상에서 이미 예약되어 있는 이름이고 렌더링 파이프라인 상에서의 의미가 정해져있다.
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

// 좌표 변환 Constant Buffer
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
// ImGui로 텍스쳐 체크해주는 Constant Buffer
cbuffer NormalMap : register(b3)
{
    int UseNormalMap;
    int UseSpecularMap;
    int UseGammaCorrection;
}
// 텍스쳐 맵이 있는지 체크하는 Constant Buffer
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

// IA - VS - RS - PS - OM
// Vertex Shader(VS) 프로그래밍
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 오브젝트 월드 변환
    float4 pos = input.mPosition;
    float4x4 matWorldBlended = MatrixPalleteArray[0];
    
    // 본이 있으면 본의 월드 트랜스폼 위치에 맞춰서 위치 아니면 기존 월드 트랜스폼 위치에 위치

        // 월드 내의 위치
    matWorldBlended = mul(input.mBlendWeights.x, MatrixPalleteArray[input.mBlendIndices.x]);
    matWorldBlended += mul(input.mBlendWeights.y, MatrixPalleteArray[input.mBlendIndices.y]);
    matWorldBlended += mul(input.mBlendWeights.z, MatrixPalleteArray[input.mBlendIndices.z]);
    matWorldBlended += mul(input.mBlendWeights.w, MatrixPalleteArray[input.mBlendIndices.w]);
    pos = mul(pos, matWorldBlended);
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
    output.mNormal = normalize(mul(input.mNormal, (float3x3) matWorldBlended));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) matWorldBlended));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) matWorldBlended));
    
    // 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // 텍스처를 입히기 위한 UV값 전달
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

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{
    float4 TextureColor = baseColor;
    
    // 텍스처
    if (bIsValidDiffuseMap)
    {
        TextureColor = texture0.Sample(sampler0, input.mUV);
    }
    
    // 감마 콜렉션
    if (UseGammaCorrection)
    {
        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
    }
    
    // 노말
    float3 Normal = input.mNormal;
    float3 lightDir = normalize(LightDir);
    
    float3 NormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
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
    
    float4 diffuse = input.mDiffuse * LightColor;
    
    // 반사광의 노멀라이즈(정규화), 오브젝트에서 카메라까지의 거리 노멀라이즈(정규화)
    float3 viewDir = normalize(input.mViewDir);
    float4 specular = 0;
    
    // 블린 퐁 적용
    if (diffuse.x > 0)
    {
        float4 specularMap = UseSpecularMap && bIsValidSpecularMap ? specular0.Sample(sampler0, input.mUV) : float4(1.f, 1.f, 1.f, 1.f);
        if (specularMap.x != -1.f)
            specularMap = float4(1.f, 1.f, 1.f, 1.f);
        
        float3 halfDirection = normalize(lightDir + viewDir);
        float speculardot = saturate(dot(-halfDirection, Normal));
        specular = pow(speculardot, SpecularPower) * LightColor * specularMap;
    }
    
    // 오파시티 맵이 있으면 해당 오파시티 맵의 알파값에 따라 픽셀 버리기
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
    
    // 감마 콜렉션
    if (UseGammaCorrection)
    {
        finalColor.rgb = sqrt(finalColor.rgb);
    }
    
    //(난반사광 + 직접광 + 주변광)
    return finalColor;
}
