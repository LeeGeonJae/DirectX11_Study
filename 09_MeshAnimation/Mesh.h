#pragma once

#include "../Engine/Header.h"
#include "BufferStruct.h"
#include "Struct.h"
#include "enum.h"

// 진짜 억지 코드.. 수정해야함.. node class 만들어서 수정 예정
class Mesh
{
public:
	void Draw(ID3D11DeviceContext* deviceContext, ComPtr<ID3D11Buffer> meshDataBuffer);
	void Close();

	void SetupMesh(ID3D11Device* device);
	
public:
	inline string GetName();
	inline void SetName(string name);
	inline string GetParentName();
	inline void SetParentName(string name);
	inline void SetParent(Mesh* parent);
	inline void SetChild(Mesh* child);
	inline void SetAnimationNode(asAnimationNode* animationNode);

	void SetVertices(aiMatrix4x4 matrix, ID3D11Device* device);

public:
	vector<ShaderVertex> m_Vertices;
	vector<UINT> m_Indices;
	map<int, Texture> m_Textures;
	CBIsValidTextureMap		m_CBIsValidTextureMap;
	CBMeshTransform		m_CBMeshTransform;
	DirectX::SimpleMath::Matrix m_Local;
	DirectX::SimpleMath::Matrix m_World;

	aiMatrix4x4 m_NodeMatrix;

private:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;

	string m_Name;
	string m_ParnetName;
	Mesh* m_pParent;
	vector<Mesh*> m_pChilds;
	asAnimationNode* m_AnimationNode;
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

void Mesh::SetParent(Mesh* parent)
{
	m_pParent = parent;
	parent->SetChild(this);
}

void Mesh::SetChild(Mesh* child)
{
	m_pChilds.push_back(child);
}

void Mesh::SetAnimationNode(asAnimationNode* animationNode)
{
	m_AnimationNode = animationNode;
}