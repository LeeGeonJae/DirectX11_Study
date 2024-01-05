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

	// �� ������ Ʈ������ x ���� Ʈ������ ��� ���۷� ���
	updateMatrixPallete(deviceContext);
}

void Model::Render(ComPtr<ID3D11DeviceContext> deviceContext)
{
	m_HeadNode->Draw(deviceContext);
}

void Model::updateMatrixPallete(ComPtr<ID3D11DeviceContext> deviceContext)
{
	// �� Ʈ������ ���
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

void Model::SetBoundingBox(Vector3 _min, Vector3 _max)
{
	m_AABBmin = Math::Vector3::Min(m_AABBmin, _min);
	m_AABBmax = Math::Vector3::Max(m_AABBmax, _max);

	// Extents = Center���� �� ���� ������ �ִ� ��ǥ(x, y, z) ��
	m_BoundingBox.Center = Math::Vector3(m_AABBmin + m_AABBmax) * 0.5;
	m_BoundingBox.Extents = Math::Vector3(m_AABBmax - m_AABBmin) * 0.5;

	// �� ���� ���� ����ؼ� z��� x�� �� ū������ AABB�� �����.
	float max = fmax(m_BoundingBox.Extents.z, fmax(m_BoundingBox.Extents.x, m_BoundingBox.Extents.y));
	m_BoundingBox.Extents.x = max;
	m_BoundingBox.Extents.y = max;
	m_BoundingBox.Extents.z = max;

	// �ٿ���ڽ��� ���͸� ����� ����
	//m_BoundingBox.Center.y += max;
}