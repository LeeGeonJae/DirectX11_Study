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
	virtual void ShadowDraw(ComPtr<ID3D11DeviceContext> deviceContext) abstract;
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
	inline void SetAABB(Vector3 AABBmin, Vector3 AABBmax);
	inline Vector3 GetAABBmin();
	inline Vector3 GetAABBmax();

protected:
	string m_Name;
	Vector3 m_AABBmin;
	Vector3 m_AABBmax;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;
	shared_ptr<Material> m_Material;

	CBMaterial					m_CBMaterial;
	CBIsValidTextureMap			m_CBIsValidTextureMap;
	ComPtr<ID3D11Buffer>		m_MaterialBuffer;
	ComPtr<ID3D11Buffer>		m_bisTextureMapBuffer;

	// Geometry
	ComPtr<ID3D11InputLayout> m_inputLayout = nullptr;
	ComPtr<ID3D11InputLayout> m_ShadowinputLayout = nullptr;
	// VS
	ComPtr<ID3D11VertexShader> m_vertexShader = nullptr;
	ComPtr<ID3DBlob> m_vsBlob = nullptr;
	// PS
	ComPtr<ID3D11PixelShader> m_pixelShader = nullptr;
	ComPtr<ID3DBlob> m_psBlob = nullptr;
	// VS
	ComPtr<ID3D11VertexShader> m_vertexShadowShader = nullptr;
	ComPtr<ID3DBlob> m_vsShadowBlob = nullptr;
	// PS
	ComPtr<ID3D11PixelShader> m_pixelShadowShader = nullptr;
	ComPtr<ID3DBlob> m_psShadowBlob = nullptr;
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

void Mesh::SetAABB(Vector3 AABBmin, Vector3 AABBmax)
{
	m_AABBmin = AABBmin;
	m_AABBmax = AABBmax;
}
Vector3 Mesh::GetAABBmin()
{
	return m_AABBmin;
}
Vector3 Mesh::GetAABBmax()
{
	return m_AABBmax;
}