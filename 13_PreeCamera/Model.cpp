#include "Model.h"
#include "Node.h"
#include "../Engine/TimeManager.h"
#include "ImGuiMenu.h"

Model::Model()
	: m_Nodes()
	, m_HeadNode()
	, m_Animation()
{
}

Model::~Model()
{
}

void Model::Init(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> ModelBuffer)
{
	m_BoneTransformBuffer = ModelBuffer->m_pCBBoneTransformData;

	m_HeadNode->Init(device, ModelBuffer);
}

void Model::Update(ComPtr<ID3D11DeviceContext> deviceContext, SimpleMath::Matrix objectTrasnform)
{
	m_HeadNode->Update(deviceContext, objectTrasnform);

	// 본 오프셋 트랜스폼 x 월드 트랜스폼 상수 버퍼로 등록
	updateMatrixPallete(deviceContext);
}

void Model::Render(ComPtr<ID3D11DeviceContext> deviceContext)
{
	m_HeadNode->Draw(deviceContext);
}

void Model::updateMatrixPallete(ComPtr<ID3D11DeviceContext> deviceContext)
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
	deviceContext->VSSetConstantBuffers(7, 1, m_BoneTransformBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(7, 1, m_BoneTransformBuffer.GetAddressOf());
}