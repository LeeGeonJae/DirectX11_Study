#pragma once
#include "Struct.h"
#include "Animation.h"

#include "../Engine/Header.h"

class Mesh;
class Material;

namespace Math = DirectX::SimpleMath;

class Node : public enable_shared_from_this<Node>
{
public:
	Node();
	~Node();

public:
	void Init(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer);
	void Update(ComPtr<ID3D11DeviceContext> deviceContext, SimpleMath::Matrix modelTransform);
	void ShadowDraw(ComPtr<ID3D11DeviceContext> deviceContext);
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	void interpolateAnimationData(float currentTime, Vector3& outPosition, Vector3& outScaling, Math::Quaternion& outRotation);

public:
	inline void							SetName(string name);
	inline string						GetName();
	inline void							SetParentNode(shared_ptr<Node> parent);
	inline shared_ptr<Node>				GetParentNode();
	inline void							SetChildNode(shared_ptr<Node> child);
	inline vector<shared_ptr<Node>>		GetChildNode();
	inline void							SetTransform(aiMatrix4x4 transform);
	inline aiMatrix4x4					GetTransform();
	inline void							SetMesh(shared_ptr<Mesh> mesh);
	inline vector<shared_ptr<Mesh>>		GetMesh();
	inline void							SetAnimation(shared_ptr<AnimationNode> animation);
	inline shared_ptr<AnimationNode>	GetAnimation();
	inline void							SetBone(shared_ptr<Bone> bone);
	inline shared_ptr<Bone>				GetBone();

	inline DirectX::SimpleMath::Matrix GetNodeTransform();

	inline Math::Matrix GetNodeWorldTransform();

private:
	string				m_Name;
	string				m_ParentName;

	shared_ptr<Node> m_Parent;
	vector<shared_ptr<Node>>		m_Children;

	vector<shared_ptr<Mesh>>		m_Mesh;
	shared_ptr<Bone> m_Bone;
	shared_ptr<AnimationNode> m_AnimationNode;
	float m_CurrentTime;

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

void Node::SetParentNode(shared_ptr<Node> parent)
{
	m_Parent = parent;

	parent->SetChildNode(shared_from_this());
}

shared_ptr<Node> Node::GetParentNode()
{
	return m_Parent;
}

void Node::SetChildNode(shared_ptr<Node> child)
{
	m_Children.push_back(child);
}

vector<shared_ptr<Node>> Node::GetChildNode()
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

void Node::SetMesh(shared_ptr<Mesh> mesh)
{
	m_Mesh.push_back(mesh);
}

vector<shared_ptr<Mesh>> Node::GetMesh()
{
	return m_Mesh;
}

void Node::SetAnimation(shared_ptr<AnimationNode> animation)
{
	m_AnimationNode = animation;
}

void Node::SetBone(shared_ptr<Bone> bone)
{
	m_Bone = bone;
}

shared_ptr<Bone> Node::GetBone()
{
	return m_Bone;
}

shared_ptr<AnimationNode> Node::GetAnimation()
{
	if (m_AnimationNode != nullptr)
		return m_AnimationNode;

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