
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
    float4 mPosition : SV_POSITION;
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
cbuffer NormalMap : register(b3)
{
    bool UseNormalMap;
    bool UseSpecularMap;
    bool UseGammaCorrection;
}
cbuffer TextureIsValid : register(b4)
{
    bool bIsValidDiffuseMap;
    bool bIsValidNormalMap;
    bool bIsValidSpecularMap;
    bool bIsValidOpcityMap;
}

// IA - VS - RS - PS - OM
// Vertex Shader(VS) 프로그래밍
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    //// 오브젝트 월드 변환
    output.mPosition = mul(input.mPosition, World);

    //// 빛 방향, 오브젝트에서 카메라 방향 계산 ( 월드 )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPosition.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    //// 오브젝트 카메라 변환, 투영 변환
    output.mPosition = mul(output.mPosition, View);
    output.mPosition = mul(output.mPosition, Projection);
    
    //// 오브젝트 월드에서 노말 벡터 계산 (오브젝트의 정면에 90도를 이루는 벡터)
    output.mNormal = normalize(mul(input.mNormal, (float3x3) World));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) World));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) World));
    
    //// 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    //// 텍스처를 입히기 위한 UV값 전달
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D texture0 : register(t0);
Texture2D normal0 : register(t1);
Texture2D specular0 : register(t2);
Texture2D opcity0 : register(t3);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{
    // 텍스처
    float4 TextureColor = texture0.Sample(sampler0, input.mUV);
    
    // 감마 콜렉션
    if (UseGammaCorrection)
    {
        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
    }
    
    // 노말
    float3 Normal = input.mNormal;
    float3 lightDir = normalize(LightDir);
    
    float3 NormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
    
    if (UseNormalMap && NormalTangentSpace.x != -1.f)
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
        float4 specularMap = UseSpecularMap ? specular0.Sample(sampler0, input.mUV) : float4(1.f, 1.f, 1.f, 1.f);
        if (specularMap.x != -1.f)
            specularMap = float4(1.f, 1.f, 1.f, 1.f);
        
        float3 halfDirection = normalize(lightDir + viewDir);
        float speculardot = saturate(dot(-halfDirection, Normal));
        specular = pow(speculardot, SpecularPower) * LightColor * specularMap;
    }
    
    float alpha = 0.f;
    if (TextureColor.a < 1.f)
    {
        alpha = opcity0.Sample(sampler0, input.mUV).a;
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
