
// Vertex Shader(VS) 입력
struct VS_INPUT
{
    float4 mPosition : POSITION;
    float2 mUV : TEXCOORD;
    float4 mColor : COLOR;
    float3 mNormal : NORMAL;
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
    float3 mNormal : NORMAL;
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

// Texture, Sampler 
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

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
    float3 worldNormal = mul(input.mNormal, (float3x3) World);
    worldNormal = normalize(worldNormal);
    output.mNormal = worldNormal;
    
    // 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse = dot(-lightDir, worldNormal);
    
    // 텍스처를 입히기 위한 UV값 전달
    output.mUV = input.mUV;
    
    return output;
}

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{
    // 텍스처
    float4 color = texture0.Sample(sampler0, input.mUV);
    
    // saturate(0~1을 넘어서는 값을 잘라냄)
    float3 diffuse = saturate(input.mDiffuse) * color * LightColor;
    
    // 반사광의 노멀라이즈(정규화), 오브젝트에서 카메라까지의 거리 노멀라이즈(정규화)
    float3 normal = normalize(input.mNormal);
    float3 specular = 0;
    
    // 반사광과 카메라(반전) 방향 내적하여 직접광의 값 구한 후
    // SpecularPower 값만큼 제곱하여 빛의 확산 조절
    // 퐁을 -> 블린퐁으로 수정함 ( 빛 방향 + 카메라 방향 )을 노말값과 내적
    if (diffuse.x > 0)
    {
        float3 lightdir = normalize(LightDir);
        float3 halfDirection = normalize(lightdir + input.mViewDir);
        float specularDot = saturate(dot(normal, -halfDirection));
        specular = pow(specularDot, SpecularPower) * (float3) color * (float3) LightColor;
    }
    
    // Ambient(주변광)
    //float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 ambient = AmbientColor * (float3) color * 0.1f;
    
    // (난반사광 + 직접광 + 주변광)
    return float4(diffuse + specular + ambient, 1);
}