#pragma once
#include "../Engine/Header.h"
#include "Struct.h"
#include "Animation.h"

class Material;
class Node;

class Model
{
public:
	Model();
	~Model();

public:
	void Init(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> ModelBuffer);
	void Update(ComPtr<ID3D11DeviceContext> deviceContext, SimpleMath::Matrix objectTrasnform);
	void Render(ComPtr<ID3D11DeviceContext> deviceContext);

private:
	void updateMatrixPallete(ComPtr<ID3D11DeviceContext> deviceContext);

public:
	inline string GetName();
	inline void SetName(string name);
	inline void SetHeadNode(shared_ptr<Node> node);
	inline shared_ptr<Node> GetHeadNode();
	inline void SetNode(shared_ptr<Node> node);
	inline vector<shared_ptr<Node>>& GetNode();
	inline bool HasNode();
	inline void SetAnimation(shared_ptr<Animation> animation);
	inline shared_ptr<Animation> GetAnimation();
	inline void SetBone(shared_ptr<Bone> bone);
	inline vector<shared_ptr<Bone>> GetBones();

private:
	string m_Name;

	vector<shared_ptr<Node>>	m_Nodes;
	vector<shared_ptr<Bone>>	m_Bones;
	shared_ptr<Node> m_HeadNode;
	shared_ptr<Animation> m_Animation;

private:
	CBMatrixPallete			m_CBMatrixPallete;
	ComPtr<ID3D11Buffer>	m_BoneTransformBuffer;
};

string Model::GetName()
{
	return m_Name;
}
void Model::SetName(string name)
{
	m_Name = name;
}

void Model::SetHeadNode(shared_ptr<Node> node)
{
	m_HeadNode = node;
}

shared_ptr<Node> Model::GetHeadNode()
{
	if (m_HeadNode != nullptr)
		return m_HeadNode;

	return nullptr;
}

void Model::SetNode(shared_ptr<Node> node)
{
	m_Nodes.push_back(node);
}

vector<shared_ptr<Node>>& Model::GetNode()
{
	return m_Nodes;
}

bool Model::HasNode()
{
	return m_Nodes.empty();
}

void Model::SetAnimation(shared_ptr<Animation> animation)
{
	m_Animation = animation;
}

shared_ptr<Animation> Model::GetAnimation()
{
	if (m_Animation != nullptr)
		return m_Animation;

	return nullptr;
}

void Model::SetBone(shared_ptr<Bone> bone)
{
	m_Bones.push_back(bone);
}

vector<shared_ptr<Bone>> Model::GetBones()
{
	return m_Bones;
}