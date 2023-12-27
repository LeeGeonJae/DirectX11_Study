#pragma once

#include "../Engine/Header.h"

#include "Struct.h"
#include "Material.h"

class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

public:
	virtual void Draw(ComPtr<ID3D11DeviceContext> deviceContext) abstract;
	virtual void Close() abstract;

	virtual void SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer) abstract;

protected:
	virtual void createVS() abstract;
	virtual void createPS() abstract;
	virtual void createInputLayout() abstract;

	void LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob);

public:
	inline string GetName();
	inline void SetName(string name);
	inline shared_ptr<Material> GetMaterial();
	inline void SetMaterial(shared_ptr<Material> material);
	inline CBIsValidTextureMap* GetIsValidTextureMap();

protected:
	string m_Name;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	shared_ptr<Material> m_Material;

	CBMaterial					m_CBMaterial;
	CBIsValidTextureMap			m_CBIsValidTextureMap;
	ComPtr<ID3D11Buffer>		m_MaterialBuffer;
	ComPtr<ID3D11Buffer>		m_bisTextureMapBuffer;

	ComPtr<ID3D11Device> m_Device;

	// Geometry
	ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;
	// VS
	ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
	ComPtr<ID3DBlob> m_vsBlob = nullptr;
	// PS
	ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
	ComPtr<ID3DBlob> m_psBlob = nullptr;
};

string Mesh::GetName()
{
	return m_Name;
}

void Mesh::SetName(string name)
{
	m_Name = name;
}

shared_ptr<Material> Mesh::GetMaterial()
{
	return m_Material;
}

void Mesh::SetMaterial(shared_ptr<Material> material)
{
	m_Material = material;
}

CBIsValidTextureMap* Mesh::GetIsValidTextureMap()
{
	return &m_CBIsValidTextureMap;
}