
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
//SV_POSITION의 SV는 SystemValue의 약자이다.
// HLSL상에서 이미 예약되어 있는 이름이고 렌더링 파이프라인 상에서의 의미가 정해져있다.
struct VS_OUTPUT
{
    float4 mPosition : SV_POSITION;
    float2 mUV : TEXCOORD1;
    float3 mViewDir : TEXCOORD2;
    float4 mDiffuse : COLOR;
    float3 mReflection : NORMAL0;
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
    output.vNormal = normalize(output.vNormal);
    output.vTangent = mul(input.mTangent, (float3x3) World);
    output.vTangent = normalize(output.vTangent);
    
    // 난반사(Diffuse) 내적으로 구하기
    // 반사광(Reflection) 오브젝트의 노말 벡터를 법선 벡터로 하여 빛의 입사광을 반사광으로 변환
    output.mDiffuse     = dot(-lightDir, output.vNormal);
    output.mReflection  = reflect(lightDir, output.vNormal);
    
    // 텍스처를 입히기 위한 UV값 전달
    output.mUV = input.mUV;
    
    return output;
}

// Texture, Sampler 
Texture2D texture0 : register(t0);
Texture2D Normal0 : register(t1);
SamplerState sampler0 : register(s0);

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{ 
    // 텍스처
    float4 color = texture0.Sample(sampler0, input.mUV);
    
    // 노말맵
    float3 vNormal = input.vNormal;
    float3 vTangent = input.vTangent;
    float3 vBiTangent = cross(vNormal, vTangent);
    
    if (UseNormalMap)
    {
        // 노멀 맵을 샘플링하여 노멀 벡터를 가져옵니다.
        float3 vNormalTangentSpace = Normal0.Sample(sampler0, input.mUV).rgb * 2.f - 1.f;
        
        // 노멀 맵에서 가져온 벡터를 노멀 맵 좌표계에서 월드 좌표계로 변환합니다.
        float3x3 WorldTransform = float3x3(vTangent, vBiTangent, vNormal);
        vNormal = mul(vNormalTangentSpace, WorldTransform);
        
        // 노멀 벡터를 정규화합니다.
        vNormal = normalize(vNormal);
        
        // 노말 맵 라이팅 적용
        float3 lightDir = normalize(LightDir);
        //input.mDiffuse = dot(-lightDir, vNormal);
        input.mReflection = reflect(lightDir, vNormal);
    }
    
    // saturate(0~1을 넘어서는 값을 잘라냄)
    float3 diffuse = saturate(input.mDiffuse) * color * LightColor;
    
    // 반사광의 노멀라이즈(정규화), 오브젝트에서 카메라까지의 거리 노멀라이즈(정규화)
    float3 reflection = normalize(input.mReflection);
    float3 viewDir = normalize(input.mViewDir);
    float3 specular = 0;
    
    // 반사광과 카메라(반전) 방향 내적하여 직접광의 값 구한 후
    // SpecularPower 값만큼 제곱하여 빛의 확산 조절
    if (diffuse.x > 0)
    {
        specular = saturate(dot(reflection, -viewDir));
        specular = pow(specular, SpecularPower) * (float3) color * (float3) LightColor;
    }
    
    // Ambient(주변광)
    float3 ambient = AmbientColor * (float3) color * 0.7f;
    
    // (난반사광 + 직접광 + 주변광)
    return float4(diffuse + specular + ambient, 1);
}