
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
    matrix View;
    matrix Projection;
    Matrix ShadowView;
    Matrix ShadowProjection;
    float3 CameraPos;
}

// 본 오프셋 좌표 ConstantBuffer
cbuffer MeshData : register(b7)
{
    matrix MatrixPalleteArray[128];
}


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
    pos = mul(pos, ShadowView);
    pos = mul(pos, ShadowProjection);
    output.mPositionProj = pos;
    
    return output;
}

// Pixel Shader(PS) 프로그래밍
float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1.f, 1.f, 1.f, 1.f);
}