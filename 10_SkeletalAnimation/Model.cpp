#include "Model.h"

#include "Node.h"

Model::Model()
	: m_Nodes()
	, m_HeadNode(nullptr)
	, m_Animation(nullptr)
	, m_Material(nullptr)
{
}

Model::~Model()
{
}

void Model::Init(ID3D11Device* device, ComPtr<ID3D11Buffer> modelBuffer, ComPtr<ID3D11Buffer> bisTextureMapBuffer, ComPtr<ID3D11Buffer> BoneTransformBuffer)
{
	m_BoneTransformBuffer = BoneTransformBuffer;

	m_HeadNode->Init(device, modelBuffer, bisTextureMapBuffer);
}

void Model::Update(ID3D11DeviceContext* deviceContext)
{
	m_HeadNode->Update(deviceContext);

	// 본 오프셋 트랜스폼 x 월드 트랜스폼 상수 버퍼로 등록
	updateMatrixPallete(deviceContext);

	m_HeadNode->Draw(deviceContext);
}

void Model::updateMatrixPallete(ID3D11DeviceContext* deviceContext)
{
	// 본 트랜스폼 계산
	assert(m_Bones.size() < 128);
	for (UINT i = 0; i < m_Bones.size(); i++)
	{
		Math::Matrix BoneNodeWorldMatrix = m_Bones[i]->m_Owner->GetNodeWorldTransform();
		BoneNodeWorldMatrix = (m_Bones[i]->m_OffsetMatrix * BoneNodeWorldMatrix).Transpose();

		m_CBMatrixPallete.Array[i] = BoneNodeWorldMatrix;
	}

	deviceContext->UpdateSubresource(m_BoneTransformBuffer.Get(), 0, nullptr, &m_CBMatrixPallete, 0, 0);
	deviceContext->VSSetConstantBuffers(6, 1, m_BoneTransformBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(6, 1, m_BoneTransformBuffer.GetAddressOf());
}