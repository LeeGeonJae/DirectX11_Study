#pragma once
#include "Struct.h"
#include "BufferStruct.h"

#include "../Engine/Header.h"
#include "Struct.h"

class Mesh;

namespace Math = DirectX::SimpleMath;

class Node
{
public:
	Node();
	~Node();

public:
	void Init(ID3D11Device* device, ComPtr<ID3D11Buffer> nodeBuffer, ComPtr<ID3D11Buffer> bisTextureMapBuffer, ComPtr<ID3D11Buffer> BoneTransformBuffer);
	void Update(ID3D11DeviceContext* deviceContext);
	void Draw(ID3D11DeviceContext* deviceContext);

private:
	void interpolateAnimationData(float currentTime, Vector3& outPosition, Vector3& outScaling, Math::Quaternion& outRotation);

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
	inline DirectX::SimpleMath::Matrix GetNodeTransform();
	inline CBIsValidTextureMap* GetIsValidTextureMap();

private:
	string				m_Name;
	string				m_ParentName;
	Node*				m_Parent;
	vector<Node*>		m_Children;

	Mesh*				m_Mesh;
	Material*			m_Material;
	asAnimationNode*	m_Animation;

private:
	DirectX::SimpleMath::Matrix m_Local;
	DirectX::SimpleMath::Matrix m_World;
	aiMatrix4x4					m_Transform;

	CBIsValidTextureMap			m_CBIsValidTextureMap;
	CBModelTransform			m_CBNodeTransform;
	CBMatrixPallete				m_CBMatrixPallete;
	ComPtr<ID3D11Buffer>		m_bisTextureMapBuffer;
	ComPtr<ID3D11Buffer>		m_NodeBuffer;
	ComPtr<ID3D11Buffer>		m_BoneTransformBuffer;
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

DirectX::SimpleMath::Matrix Node::GetNodeTransform()
{
	DirectX::SimpleMath::Matrix transform(
		m_Transform.a1, m_Transform.a2, m_Transform.a3, m_Transform.a4,
		m_Transform.b1, m_Transform.b2, m_Transform.b3, m_Transform.b4,
		m_Transform.c1, m_Transform.c2, m_Transform.c3, m_Transform.c4,
		m_Transform.d1, m_Transform.d2, m_Transform.d3, m_Transform.d4
	);

	return transform;
}

CBIsValidTextureMap* Node::GetIsValidTextureMap()
{
	return &m_CBIsValidTextureMap;
}