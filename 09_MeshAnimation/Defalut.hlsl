
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
cbuffer MeshData : register(b4)
{
    matrix meshWorld;
}

// IA - VS - RS - PS - OM
// Vertex Shader(VS) 프로그래밍
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // 오브젝트 월드 변환
    output.mPosition = mul(input.mPosition, meshWorld);

    // 빛 방향, 오브젝트에서 카메라 방향 계산 ( 월드 )
    float3 lightDir = normalize(LightDir);
    float3 viewDir = normalize(output.mPosition.xyz - CameraPos.xyz);
    output.mViewDir = viewDir;
    
    // 오브젝트 카메라 변환, 투영 변환
    output.mPosition = mul(output.mPosition, View);
    output.mPosition = mul(output.mPosition, Projection);
    
    // 오브젝트 월드에서 노말 벡터 계산 (오브젝트의 정면에 90도를 이루는 벡터)
    output.mNormal = normalize(mul(input.mNormal, (float3x3) meshWorld));
    output.mTangent = normalize(mul(input.mTangent, (float3x3) meshWorld));
    output.mBiTangent = normalize(mul(input.mBiTangent, (float3x3) meshWorld));
    
    // 난반사(Diffuse) 내적으로 구하기
    output.mDiffuse = saturate(dot(-lightDir, output.mNormal));
    
    // 텍스처를 입히기 위한 UV값 전달
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
    float4 finalColor = input.mDiffuse * LightColor;
	
    return finalColor;
}
