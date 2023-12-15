#pragma once

#include "../Engine/Header.h"

#include "Struct.h"

// 진짜 억지 코드.. 수정해야함.. node class 만들어서 수정 예정
class Mesh
{
public:
	Mesh();
	~Mesh();

public:
	void Draw(ID3D11DeviceContext* deviceContext);
	void Close();


public:
	void SetupMesh(ID3D11Device* device, shared_ptr<ModelCBBuffer> NodeBuffer);

	inline string GetName();
	inline void SetName(string name);
	inline string GetParentName();
	inline void SetParentName(string name);
	inline Material* GetMaterial();
	inline void SetMaterial(Material* material);
	inline CBIsValidTextureMap* GetIsValidTextureMap();

public:
	vector<BoneWeightVertex>	m_BoneWeightVertices;
	vector<UINT>				m_Indices;

private:
	string m_Name;
	string m_ParnetName;

	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
	Material* m_Material;

	CBMaterial					m_CBMaterial;
	CBIsValidTextureMap			m_CBIsValidTextureMap;
	ComPtr<ID3D11Buffer>		m_MaterialBuffer;
	ComPtr<ID3D11Buffer>		m_bisTextureMapBuffer;
};

string Mesh::GetName()
{
	return m_Name;
}

void Mesh::SetName(string name)
{
	m_Name = name;
}

string Mesh::GetParentName()
{
	return m_ParnetName;
}

void Mesh::SetParentName(string name)
{
	m_ParnetName = name;
}

Material* Mesh::GetMaterial()
{
	return m_Material;
}

void Mesh::SetMaterial(Material* material)
{
	m_Material = material;
}

CBIsValidTextureMap* Mesh::GetIsValidTextureMap()
{
	return &m_CBIsValidTextureMap;
}