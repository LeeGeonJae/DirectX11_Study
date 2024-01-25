#include "StaticMesh.h"
#include "ImGuiMenu.h"
#include "RenderManager.h"

namespace Math = DirectX::SimpleMath;

StaticMesh::StaticMesh()
	: Mesh::Mesh()
	, m_Vertices{}
	, m_Indices{}
{
	m_CBIsValidTextureMap.bIsValidBone = false;
	m_CBMaterial.BaseColor = Color(1.f, 1.f, 1.f, 1.f);
	m_CBMaterial.EmissiveColor = Color(1.f, 1.f, 1.f, 1.f);
}

StaticMesh::~StaticMesh()
{
}

void StaticMesh::ShadowDraw(ComPtr<ID3D11DeviceContext> deviceContext)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	deviceContext->IASetInputLayout(m_ShadowinputLayout.Get());
	deviceContext->VSSetShader(m_vertexShadowShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShadowShader.Get(), nullptr, 0);

	deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void StaticMesh::Draw(ComPtr<ID3D11DeviceContext> deviceContext)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 텍스쳐 파일 세팅
	for (int i = 0; i < static_cast<int>(TextureType::END); i++)
	{
		auto find = m_Material->GetTexture(static_cast<TextureType>(i));

		if (find != nullptr)
		{
			m_CBIsValidTextureMap.bIsValidTextureMap[i] = true;
			deviceContext->PSSetShaderResources(i, 1, find->m_Texture.GetAddressOf());
		}
		else
			m_CBIsValidTextureMap.bIsValidTextureMap[i] = false;
	}

	m_CBMaterial.BaseColor = Color(m_Material->m_BaseColor.x, m_Material->m_BaseColor.y, m_Material->m_BaseColor.z, 1.f);
	m_CBMaterial.EmissiveColor = Color(m_Material->m_EmissiveColor.x, m_Material->m_EmissiveColor.y, m_Material->m_EmissiveColor.z, 1.f);
	m_CBMaterial.EmissivePower = ImGuiMenu::EmissivePower;
	m_CBMaterial.OpacityValue = ImGuiMenu::OpacityValue;
	m_CBIsValidTextureMap.bIsValidBone = false;

	deviceContext->IASetInputLayout(m_inputLayout.Get());
	deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

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

void StaticMesh::Close()
{
}

void StaticMesh::SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer)
{
	m_MaterialBuffer = NodeBuffer->m_pCBMaterialData;
	m_bisTextureMapBuffer = NodeBuffer->m_pCBbisTextureMap;

	HRESULT hr;

	if (m_VertexBuffer == nullptr)
	{
		D3D11_BUFFER_DESC vertexDesc;
		vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_Vertices.size());
		vertexDesc.CPUAccessFlags = 0;
		vertexDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &m_Vertices[0];

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

	if (m_vertexShader == nullptr)
	{
		createVS();
		createInputLayout();
		createPS();
	}
}

void StaticMesh::createVS()
{
	LoadShaderFromFile(L"StaticMeshShader.hlsl", "VS", "vs_5_0", m_vsBlob);
	HRESULT hr = DEVICE->CreateVertexShader(m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
	assert(SUCCEEDED(hr));
	LoadShaderFromFile(L"StaticShadowShader.hlsl", "VS", "vs_5_0", m_vsShadowBlob);
	hr = DEVICE->CreateVertexShader(m_vsShadowBlob->GetBufferPointer(), m_vsShadowBlob->GetBufferSize(), nullptr, m_vertexShadowShader.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void StaticMesh::createPS()
{
	LoadShaderFromFile(L"StaticMeshShader.hlsl", "PS", "ps_5_0", m_psBlob);
	HRESULT hr = DEVICE->CreatePixelShader(m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());
	assert(SUCCEEDED(hr));
	LoadShaderFromFile(L"StaticShadowShader.hlsl", "PS", "ps_5_0", m_psShadowBlob);
	hr = DEVICE->CreatePixelShader(m_psShadowBlob->GetBufferPointer(), m_psShadowBlob->GetBufferSize(), nullptr, m_pixelShadowShader.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void StaticMesh::createInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	const int count = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	DEVICE->CreateInputLayout(layout, count, m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
	DEVICE->CreateInputLayout(layout, count, m_vsShadowBlob->GetBufferPointer(), m_vsShadowBlob->GetBufferSize(), m_ShadowinputLayout.GetAddressOf());
}
