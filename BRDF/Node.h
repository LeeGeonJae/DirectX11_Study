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
	void Init(ID3D11Device* device, shared_ptr<ModelCBBuffer> NodeBuffer);
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
	inline vector<Mesh*> GetMesh();
	inline void SetMaterial(Material* material);
	inline Material* GetMaterial();
	inline void SetAnimation(asAnimationNode* animation);
	inline asAnimationNode* GetAnimation();
	inline void SetBone(Bone* bone);
	inline Bone* GetBone();

	inline DirectX::SimpleMath::Matrix GetNodeTransform();
	inline CBIsValidTextureMap* GetIsValidTextureMap();

	inline Math::Matrix GetNodeWorldTransform();

private:
	string				m_Name;
	string				m_ParentName;

	Node* m_Parent;
	vector<Node*>		m_Children;

	vector<Mesh*>		m_Mesh;
	Bone* m_Bone;
	asAnimationNode* m_Animation;

private:
	Math::Matrix m_Local;
	Math::Matrix m_World;
	aiMatrix4x4					m_Transform;

	CBNodeTransform				m_CBNodeTransform;
	ComPtr<ID3D11Buffer>		m_NodeBuffer;
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

	parent->SetChildNode(this);
}

Node* Node::GetParentNode()
{
	return m_Parent;
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
	m_Mesh.push_back(mesh);
}

vector<Mesh*> Node::GetMesh()
{
	return m_Mesh;
}

void Node::SetAnimation(asAnimationNode* animation)
{
	m_Animation = animation;
}

void Node::SetBone(Bone* bone)
{
	m_Bone = bone;
}

Bone* Node::GetBone()
{
	return m_Bone;
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
		m_Transform.a1, m_Transform.b1, m_Transform.c1, m_Transform.d1,
		m_Transform.a2, m_Transform.b2, m_Transform.c2, m_Transform.d2,
		m_Transform.a3, m_Transform.b3, m_Transform.c3, m_Transform.d3,
		m_Transform.a4, m_Transform.b4, m_Transform.c4, m_Transform.d4
	);

	return transform;
}

Math::Matrix Node::GetNodeWorldTransform()
{
	return m_World;
}