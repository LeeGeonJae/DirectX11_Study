
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

// �޽� ���� ��ǥ ConstantBuffer
cbuffer MeshData : register(b6)
{
    matrix meshWorld;
}

// Vertex Shader(VS) ���α׷���
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;

    // ������Ʈ ���� ��ȯ
    float4 pos = input.mPosition;
    pos = mul(pos, meshWorld);
    output.mPositionWorld = pos;
    
    // ndc ���� ���� ��ġ
    pos = mul(pos, ShadowView);
    pos = mul(pos, ShadowProjection);
    output.mPositionProj = pos;
    
    return output;
}

// Pixel Shader(PS) ���α׷���
float4 PS(VS_OUTPUT input) : SV_Target
{
    return float4(1.f, 1.f, 1.f, 1.f);
}