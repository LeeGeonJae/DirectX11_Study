#include "Mesh.h"

void Mesh::Draw(ID3D11DeviceContext* deviceContext)
{
	UINT stride = sizeof(ShaderVertex);
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	for (int i = 0; i < static_cast<int>(TextureType::END); i++)
	{
		auto find = m_Textures.find(i);
		if (find != m_Textures.end())
		{
			deviceContext->PSSetShaderResources(i, 1, &(find->second.m_Texture));
		}
	}

	deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
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