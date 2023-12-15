#include "Mesh.h"

void Mesh::Draw(ID3D11DeviceContext* deviceContext)
{
    UINT stride = sizeof(ShaderVertex);
    UINT offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    for (int i = 0; i < static_cast<int>(TextureType::END); i++)
    {
        auto texture = m_Textures.find(i);
        if (texture != m_Textures.end())
        {
            deviceContext->PSSetShaderResources(i, 1, &(texture->second.m_Texture));
        }
    }

    deviceContext->DrawIndexed(static_cast<UINT>(m_Indices.size()), 0, 0);
}

void Mesh::Close()
{
}

void Mesh::SetupMesh(ID3D11Device* device)
{
    HRESULT hr;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = static_cast<UINT>(sizeof(ShaderVertex) * m_Vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &m_Vertices[0];

    hr = device->CreateBuffer(&vbd, &initData, &m_VertexBuffer);
    assert(SUCCEEDED(hr));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

    initData.pSysMem = &m_Indices[0];

    hr = device->CreateBuffer(&ibd, &initData, &m_IndexBuffer);
    assert(SUCCEEDED(hr));
}
