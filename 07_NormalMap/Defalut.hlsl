
// Vertex Shader(VS) 입력
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float4 mColor : COLOR;
    float3 mNormal : NORMAL0;
    float3 mTangent : NORMAL1;
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
    float3 vNormal : NORMAL1;
    float3 vTangent : NORMAL2;
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

// IA - VS - RS - PS - OM
// Vertex Shader(VS) 프로그래밍
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 오브젝트 월드 변환
    output.mPosition = mul(input.mPosition, World);

    // 빛 방향, 오브젝트에서 카메라 방향 계산 ( 월드 )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPosition.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // 오브젝트 카메라 변환, 투영 변환
    output.mPosition = mul(output.mPosition, View);
    output.mPosition = mul(output.mPosition, Projection);
    
    // 오브젝트 월드에서 노말 벡터 계산 (오브젝트의 정면에 90도를 이루는 벡터)
    output.vNormal = mul(input.mNormal, (float3x3) World);
    output.vTangent = mul(input.mTangent, (float3x3) World);
    
    // 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse     = dot(-lightDir, output.vNormal);
    
    // 텍스처를 입히기 위한 UV값 전달
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D texture0 : register(t0);
Texture2D normal0 : register(t1);
Texture2D specular0 : register(t2);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{ 
    // 텍스처
    float4 TextureColor = texture0.Sample(sampler0, input.mUV);
    
    if (UseGammaCorrection)
    {
        TextureColor.rgb = TextureColor.rgb * TextureColor.rgb;
    }
    
    // 노말맵
    float3 vNormal = normalize(input.vNormal);
    float3 vTangent = normalize(input.vTangent);
    float3 vBiTangent = normalize(cross(vNormal, vTangent));
    float3 lightDir = normalize(LightDir);
    
    if (UseNormalMap)
    {
        // 노멀 맵을 샘플링하여 노멀 벡터를 가져옵니다.
        float3 vNormalTangentSpace = normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
        vNormalTangentSpace = normalize(vNormalTangentSpace);
        
        // 노멀 맵에서 가져온 벡터를 노멀 맵 좌표계에서 월드 좌표계로 변환합니다.
        float3x3 WorldTransform = float3x3(vTangent, vBiTangent, vNormal);
        vNormal = mul(vNormalTangentSpace, WorldTransform);
        
        // 노멀 벡터를 정규화합니다.
        vNormal = normalize(vNormal);
        
        // 노말 맵 라이팅 적용
        input.mDiffuse = dot(-lightDir, vNormal);
    }
    
    // saturate(0~1을 넘어서는 값을 잘라냄)
    float4 diffuse = saturate(input.mDiffuse) * LightColor;
    
    // 반사광의 노멀라이즈(정규화), 오브젝트에서 카메라까지의 거리 노멀라이즈(정규화)
    float3 viewDir = normalize(input.mViewDir);
    float4 specular = 0;
    
    // 반사광과 카메라(반전) 방향 내적하여 직접광의 값 구한 후
    // SpecularPower 값만큼 제곱하여 빛의 확산 조절
    // SpecularMap 적용
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
    
    // (난반사광 + 직접광 + 주변광)
    return finalColor;
}