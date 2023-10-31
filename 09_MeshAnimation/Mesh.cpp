#include "Mesh.h"
#include "ImGuiMenu.h"
#include "../Engine/Header.h"

namespace Math = DirectX::SimpleMath;

void Mesh::Draw(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer> meshDataBuffer)
{
	UINT stride = sizeof(ShaderVertex);
	UINT offset = 0;

	// 애니메이션이 있을 경우 해당 키 프레임 정보 읽어와서 등록 ( 시간 추가해서 시간당 애니메이션 진행하는거 만들어야 함 )
	if (m_AnimationNode != nullptr)
	{
		Math::Vector3 position, scaling;
		Math::Quaternion rotation;

		//position.x = m_AnimationNode->m_KeyFrame[0].m_Transtation.x;
		//position.y = m_AnimationNode->m_KeyFrame[0].m_Transtation.y;
		//position.z = m_AnimationNode->m_KeyFrame[0].m_Transtation.z;
		position.x =0.f;
		position.y =0.f;
		position.z =0.f;
		//scaling.x = m_AnimationNode->m_KeyFrame[0].m_Scale.x;
		//scaling.y = m_AnimationNode->m_KeyFrame[0].m_Scale.y;
		//scaling.z = m_AnimationNode->m_KeyFrame[0].m_Scale.z;
		scaling.x = 1.f;
		scaling.y = 1.f;
		scaling.z = 1.f;
		//rotation.x = m_AnimationNode->m_KeyFrame[0].m_Rotation.x;
		//rotation.y = m_AnimationNode->m_KeyFrame[0].m_Rotation.y;
		//rotation.z = m_AnimationNode->m_KeyFrame[0].m_Rotation.z;
		//rotation.w = m_AnimationNode->m_KeyFrame[0].m_Rotation.w;
		rotation.x = 0.f;
		rotation.y = 0.f;
		rotation.z = 0.f;
		rotation.w = 0.f;

		XMMATRIX mSpin1 = XMMatrixRotationX(ImGuiMenu::CubeRotation.x * 3.14f);
		XMMATRIX mSpin2 = XMMatrixRotationY(ImGuiMenu::CubeRotation.y * 3.14f);

		//m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);
		m_Local = Math::Matrix::CreateScale(scaling) * mSpin1 * mSpin2 * Math::Matrix::CreateTranslation(position);

		//XMMATRIX mTranslate = XMMatrixTranslation(ImGuiMenu::CubePosition.x, ImGuiMenu::CubePosition.y, ImGuiMenu::CubePosition.z);
		//m_Local = mTranslate;
	}

	 //부모가 있으면 해당 부모의 트랜스폼 곱하기
	if (m_pParent != nullptr)
	{
		m_World = m_Local * m_pParent->m_World;
		m_CBMeshTransform.World = m_World;
	}
	else
	{
		m_World = m_Local;
		m_CBMeshTransform.World = m_World;
	}

	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (int i = 0; i < static_cast<int>(TextureType::END); i++)
	{
		auto find = m_Textures.find(i);
		if (find != m_Textures.end())
		{
			deviceContext->PSSetShaderResources(i, 1, &(find->second->m_Texture));
		}
	}

	m_Name;
	m_ParnetName;

	deviceContext->UpdateSubresource(meshDataBuffer.Get(), 0, nullptr, &m_CBMeshTransform, 0, 0);
	deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);

	// 자식 메시 그리기
	for (auto child : m_pChilds)
	{
		if (child != nullptr)
		{
			child->Draw(deviceContext, meshDataBuffer);
		}
	}
}

void Mesh::Close()
{
	if (m_VertexBuffer != nullptr)
	{
		delete m_VertexBuffer;
		m_VertexBuffer = nullptr;
	}

	if (m_IndexBuffer != nullptr)
	{
		delete m_IndexBuffer;
		m_IndexBuffer = nullptr;
	}
}

void Mesh::SetupMesh(ID3D11Device* device)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = static_cast<UINT>(sizeof(ShaderVertex) * m_Vertices.size());
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_Vertices[0];

	hr = device->CreateBuffer(&vertexDesc, &initData, &m_VertexBuffer);
	assert(SUCCEEDED(hr));

	D3D11_BUFFER_DESC indexDesc;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;

	initData.pSysMem = &m_Indices[0];

	hr = device->CreateBuffer(&indexDesc, &initData, &m_IndexBuffer);
	assert(SUCCEEDED(hr));
}

void Mesh::SetVertices(aiMatrix4x4 matrix, ID3D11Device* device)
{
	m_NodeMatrix *= matrix;

	for (int i = 0; i < m_Vertices.size(); i++)
	{
		aiVector3D vec;
		vec.x = m_Vertices[i].m_Position.x;
		vec.y = m_Vertices[i].m_Position.y;
		vec.z = m_Vertices[i].m_Position.z;

		vec = m_NodeMatrix * vec;

		m_Vertices[i].m_Position.x = vec.x;
		m_Vertices[i].m_Position.y = vec.y;
		m_Vertices[i].m_Position.z = vec.z;
	}

	for (auto child : m_pChilds)
	{
		child->SetVertices(m_NodeMatrix, device);
	}

	SetupMesh(device);
}