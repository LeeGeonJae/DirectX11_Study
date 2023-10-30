#pragma once
#include "Struct.h"
#include "../Engine/Header.h"

class Mesh;
class Material;

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
	inline void SetParentNode(shared_ptr<Node> parent);
	inline Node* GetParentNode();
	inline void SetChildNode(Node* child);
	inline vector<Node*> GetChildNode();
	inline void SetTransform(Math::Matrix transform);
	inline Math::Matrix GetTransform();
	inline void SetMesh(shared_ptr<Mesh> mesh);
	inline Mesh* GetMesh();
	inline void SetMaterial(shared_ptr<Material> material);
	inline Material* GetMaterial();
	inline void SetAnimation(shared_ptr<asAnimation> animation);
	inline asAnimation* GetAnimation();

private:
	string			m_Name;
	string			m_ParentName;
	Node*			m_Parent;
	vector<Node*>	m_Children;
	Math::Matrix	m_Transform;

	Mesh*			m_Mesh;
	Material*		m_Material;
	asAnimation*	m_Animation;
};

void Node::SetName(string name)
{
	m_Name = name;
}

string Node::GetName()
{
	return m_Name;
}

void Node::SetParentNode(shared_ptr<Node> parent)
{
	m_Parent = parent;

	m_Parent->SetChildNode(this);
}

shared_ptr<Node> Node::GetParentNode()
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

void Node::SetTransform(Math::Matrix transform)
{
	m_Transform = transform;
}

Math::Matrix Node::GetTransform()
{
	return m_Transform;
}

void Node::SetMesh(shared_ptr<Mesh> mesh)
{
	m_Mesh = mesh;
}

shared_ptr<Mesh> Node::GetMesh()
{
	if (m_Mesh != nullptr)
		return m_Mesh;

	return nullptr;
}

void Node::SetMaterial(shared_ptr<Material> material)
{
	m_Material = material;
}

shared_ptr<Material> Node::GetMaterial()
{
	if (m_Material != nullptr)
		return m_Material;

	return nullptr;
}

void Node::SetAnimation(shared_ptr<asAnimation> animation)
{
	m_Animation = animation;
}

shared_ptr<asAnimation> Node::GetAnimation()
{
	if (m_Animation != nullptr)
		return m_Animation;

	return nullptr;
}