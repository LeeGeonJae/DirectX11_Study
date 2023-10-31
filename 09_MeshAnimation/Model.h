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
	void Draw();

public:
	inline void SetHeadNode(Node* node);
	inline Node* GetHeadNode();
	inline void SetNode(Node* node);
	inline vector<Node*>& GetNode();
	inline bool HasNode();
	inline void SetAnimation(asAnimation* animation);
	inline asAnimation* GetAnimation();
	inline void SetCBMeshData(ComPtr<ID3D11Buffer> meshData);

private:


private:
	vector<Node*>		m_Nodes;
	Node*				m_HeadNode;
	asAnimation*		m_Animation;
	ComPtr<ID3D11Buffer> m_CBMeshData;
};

void Model::SetHeadNode(Node* node)
{
	m_HeadNode = node;
}

Node* Model::GetHeadNode()
{
	return m_HeadNode;
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
	return m_Animation;
}

void Model::SetCBMeshData(ComPtr<ID3D11Buffer> meshData)
{
	m_CBMeshData = meshData;
}