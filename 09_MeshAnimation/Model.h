#pragma once
#include "../Engine/Header.h"
#include "Struct.h"
#include "BufferStruct.h"

class Node;

class Model
{
public:
	Model();
	~Model();

public:
	void Init(ID3D11Device* device, ComPtr<ID3D11Buffer> modelData);
	void Update(ID3D11DeviceContext* deviceContext);

public:
	inline void SetHeadNode(Node* node);
	inline Node* GetHeadNode();
	inline void SetNode(Node* node);
	inline vector<Node*>& GetNode();
	inline bool HasNode();
	inline void SetAnimation(asAnimation* animation);
	inline asAnimation* GetAnimation();
	inline void SetMaterial(Material* material);
	inline Material* GetMaterial();

private:


private:
	vector<Node*>			m_Nodes;
	Node*					m_HeadNode;
	asAnimation*			m_Animation;
	Material*				m_Material;
	ComPtr<ID3D11Buffer>			m_CBNodeData = nullptr;
};

void Model::SetHeadNode(Node* node)
{
	m_HeadNode = node;
}

Node* Model::GetHeadNode()
{
	if (m_HeadNode != nullptr)
		return m_HeadNode;

	return nullptr;
}

void Model::SetNode(Node* node)
{
	m_Nodes.push_back(node);
}

vector<Node*>& Model::GetNode()
{
	return m_Nodes;
} 

bool Model::HasNode()
{
	return m_Nodes.empty();
}

void Model::SetAnimation(asAnimation* animation)
{
	m_Animation = animation;
}

asAnimation* Model::GetAnimation()
{
	if (m_Animation != nullptr)
		return m_Animation;

	return nullptr;
}

void Model::SetMaterial(Material* material)
{
	m_Material = material;
}

Material* Model::GetMaterial()
{
	if (m_Material != nullptr)
		return m_Material;

	return nullptr;
}