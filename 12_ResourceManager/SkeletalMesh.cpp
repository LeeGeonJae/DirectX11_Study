#include "SkeletalMesh.h"

#include "ImGuiMenu.h"

namespace Math = DirectX::SimpleMath;

SkeletalMesh::SkeletalMesh()
	: Mesh::Mesh()
	, m_BoneWeightVertices{}
	, m_Indices{}
{
}

SkeletalMesh::~SkeletalMesh()
{
}

void SkeletalMesh::Draw(ComPtr<ID3D11DeviceContext> deviceContext)
{
	UINT stride = sizeof(BoneWeightVertex);
	UINT offset = 0;

	// 텍스쳐 파일 세팅
	for (int i = 0; i < static_cast<int>(TextureType::END); i++)
	{
		auto find = m_Material->GetTexture(static_cast<TextureType>(i));

		if (find != nullptr)
		{
			deviceContext->PSSetShaderResources(i, 1, find->m_Texture.GetAddressOf());
		}
	}

	m_CBMaterial.BaseColor = Color(m_Material->m_BaseColor.x, m_Material->m_BaseColor.y, m_Material->m_BaseColor.z, 1.f);
	m_CBMaterial.EmissiveColor = Color(m_Material->m_EmissiveColor.x, m_Material->m_EmissiveColor.y, m_Material->m_EmissiveColor.z, 1.f);
	m_CBMaterial.EmissivePower = ImGuiMenu::EmissivePower;
	m_CBMaterial.OpacityValue = ImGuiMenu::OpacityValue;

	deviceContext->UpdateSubresource(m_bisTextureMapBuffer.Get(), 0, nullptr, &m_CBIsValidTextureMap, 0, 0);
	deviceContext->UpdateSubresource(m_MaterialBuffer.Get(), 0, nullptr, &m_CBMaterial, 0, 0);

	deviceContext->VSSetConstantBuffers(4, 1, m_bisTextureMapBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(4, 1, m_bisTextureMapBuffer.GetAddressOf());
	deviceContext->VSSetConstantBuffers(5, 1, m_MaterialBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(5, 1, m_MaterialBuffer.GetAddressOf());

	deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void SkeletalMesh::Close()
{
}

void SkeletalMesh::SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer)
{
	m_MaterialBuffer = NodeBuffer->m_pCBMaterialData;
	m_bisTextureMapBuffer = NodeBuffer->m_pCBbisTextureMap;

	HRESULT hr;

	D3D11_BUFFER_DESC vertexDesc;
	vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.ByteWidth = static_cast<UINT>(sizeof(BoneWeightVertex) * m_BoneWeightVertices.size());
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_BoneWeightVertices[0];

	hr = device->CreateBuffer(&vertexDesc, &initData, m_VertexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_BUFFER_DESC indexDesc;
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.CPUAccessFlags = 0;
	indexDesc.MiscFlags = 0;

	initData.pSysMem = &m_Indices[0];

	hr = device->CreateBuffer(&indexDesc, &initData, m_IndexBuffer.GetAddressOf());
	assert(SUCCEEDED(hr));
}