#pragma once
#include "Struct.h"
#include "../Engine/Header.h"

class Mesh;

namespace Math = DirectX::SimpleMath;

class Node
{
public:
	Node();
	~Node();

public:
	void Draw(ID3D11DeviceContext* deviceContext);

public:
	inline void SetName(string name);
	inline string GetName();
	inline void SetParentNode(Node* parent);
	inline Node* GetParentNode();
	inline void SetChildNode(Node* child);
	inline vector<Node*> GetChildNode();
	inline void SetTransform(aiMatrix4x4 transform);
	inline aiMatrix4x4 GetTransform();
	inline void SetMesh(Mesh* mesh);
	inline Mesh* GetMesh();
	inline void SetMaterial(Material* material);
	inline Material* GetMaterial();
	inline void SetAnimation(asAnimationNode* animation);
	inline asAnimationNode* GetAnimation();

private:
	string				m_Name;
	string				m_ParentName;
	Node*				m_Parent;
	vector<Node*>		m_Children;
	aiMatrix4x4			m_Transform;

	Mesh*				m_Mesh;
	Material*			m_Material;
	asAnimationNode*	m_Animation;
};

void Node::SetName(string name)
{
	m_Name = name;
}

string Node::GetName()
{
	return m_Name;
}

void Node::SetParentNode(Node* parent)
{
	m_Parent = parent;

	m_Parent->SetChildNode(this);
}

Node* Node::GetParentNode()
{
	if (m_Parent != nullptr)
		return m_Parent;

	return nullptr;
}

void Node::SetChildNode(Node* child)
{
	m_Children.push_back(child);
}

vector<Node*> Node::GetChildNode()
{
	return m_Children;
}

void Node::SetTransform(aiMatrix4x4 transform)
{
	m_Transform = transform;
}

aiMatrix4x4 Node::GetTransform()
{
	return m_Transform;
}

void Node::SetMesh(Mesh* mesh)
{
	m_Mesh = mesh;
}

Mesh* Node::GetMesh()
{
	if (m_Mesh != nullptr)
		return m_Mesh;

	return nullptr;
}

void Node::SetMaterial(Material* material)
{
	m_Material = material;
}

Material* Node::GetMaterial()
{
	if (m_Material != nullptr)
		return m_Material;

	return nullptr;
}

void Node::SetAnimation(asAnimationNode* animation)
{
	m_Animation = animation;
}

asAnimationNode* Node::GetAnimation()
{
	if (m_Animation != nullptr)
		return m_Animation;

	return nullptr;
}