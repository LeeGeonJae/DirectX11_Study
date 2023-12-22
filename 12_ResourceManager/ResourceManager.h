#pragma once
#include "../Engine/Header.h"

class ModelLoadManager;
class Material;
class AnimationNode;
class Texture;
class SkeletalMesh;
class StaticMesh;
class NodeData;
class Bone;
class Model;

struct ResourceManager
{
private:
	ResourceManager() {}
public:
	static ResourceManager* GetInstance()
	{
		static ResourceManager instance;
		return &instance;
	}

public:
	void Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);

	shared_ptr<Model> CreateModel(string path);

public:
	inline void SetStaticMesh(string key, weak_ptr<StaticMesh> staticMesh);
	inline void SetSkeletalMesh(string key, weak_ptr<SkeletalMesh> skeletalMesh);
	inline void SetBone(string key, weak_ptr<Bone> bone);
	inline void SetAnimation(string key, weak_ptr<AnimationNode> animation);
	inline void SetMaterial(string key, weak_ptr<Material> material);
	inline void SetNodeData(string key, weak_ptr<NodeData> nodeData);
	inline void SetModel(string key, weak_ptr<Model> model);
	inline shared_ptr<StaticMesh> FindStaticMesh(string key);
	inline shared_ptr<SkeletalMesh> FindSkeletalMesh(string key);
	inline shared_ptr<Bone> FindBone(string key);
	inline shared_ptr<AnimationNode> FindAnimation(string key);
	inline shared_ptr<Material> FindMaterial(string key);
	inline shared_ptr<NodeData> FindNodeData(string key);
	inline shared_ptr<Model> FindModel(string key);

private:
	// Model
	map<string, weak_ptr<Model>> m_ModelMap;
	// StaticMesh
	map<string, weak_ptr<StaticMesh>> m_StaticMeshMap;
	// SkeletalMesh
	map<string, weak_ptr<SkeletalMesh>> m_SkeletalMeshMap;
	// Skeleton
	map<string, weak_ptr<Bone>> m_BoneMap;
	// Animation
	map<string, weak_ptr<AnimationNode>> m_AnimationMap;
	// Material
	map<string, weak_ptr<Material>> m_MaterialMap;
	// NodeData
	map<string, weak_ptr<NodeData>> m_NodeDataMap;

private:
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_DeviceContext;
};

void ResourceManager::SetStaticMesh(string key, weak_ptr<StaticMesh> staticMesh)
{
	auto staticmesh = FindStaticMesh(key);

	if (staticmesh == nullptr)
		m_StaticMeshMap.insert(make_pair(key, staticMesh));
}
void ResourceManager::SetSkeletalMesh(string key, weak_ptr<SkeletalMesh> skeletalMesh)
{
	auto staticmesh = FindSkeletalMesh(key);

	if (staticmesh == nullptr)
		m_SkeletalMeshMap.insert(make_pair(key, skeletalMesh));
}
void ResourceManager::SetBone(string key, weak_ptr<Bone> bone)
{
	auto staticmesh = FindBone(key);

	if (staticmesh == nullptr)
		m_BoneMap.insert(make_pair(key, bone));
}
void ResourceManager::SetAnimation(string key, weak_ptr<AnimationNode> animation)
{
	auto staticmesh = FindAnimation(key);

	if (staticmesh == nullptr)
		m_AnimationMap.insert(make_pair(key, animation));
}
void ResourceManager::SetMaterial(string key, weak_ptr<Material> material)
{
	auto staticmesh = FindMaterial(key);

	if (staticmesh == nullptr)
		m_MaterialMap.insert(make_pair(key, material));
}
void ResourceManager::SetNodeData(string key, weak_ptr<NodeData> nodeData)
{
	auto staticmesh = FindNodeData(key);

	if (staticmesh == nullptr)
		m_NodeDataMap.insert(make_pair(key, nodeData));
}
void ResourceManager::SetModel(string key, weak_ptr<Model> model)
{
	auto staticmesh = FindModel(key);

	if (staticmesh == nullptr)
		m_ModelMap.insert(make_pair(key, model));
}

shared_ptr<StaticMesh> ResourceManager::FindStaticMesh(string key)
{
	auto staticMesh = m_StaticMeshMap.find(key);

	if (staticMesh != m_StaticMeshMap.end())
		return staticMesh->second.lock();

	return nullptr;
}
shared_ptr<SkeletalMesh> ResourceManager::FindSkeletalMesh(string key)
{
	auto skeletalMesh = m_SkeletalMeshMap.find(key);

	if (skeletalMesh != m_SkeletalMeshMap.end())
		return skeletalMesh->second.lock();

	return nullptr;
}
shared_ptr<Bone> ResourceManager::FindBone(string key)
{
	auto bone = m_BoneMap.find(key);

	if (bone != m_BoneMap.end())
		return bone->second.lock();

	return nullptr;
}
shared_ptr<AnimationNode> ResourceManager::FindAnimation(string key)
{
	auto animation = m_AnimationMap.find(key);

	if (animation != m_AnimationMap.end())
		return animation->second.lock();

	return nullptr;
}
shared_ptr<Material> ResourceManager::FindMaterial(string key)
{
	auto material = m_MaterialMap.find(key);

	if (material != m_MaterialMap.end())
		return material->second.lock();

	return nullptr;
}
shared_ptr<NodeData> ResourceManager::FindNodeData(string key)
{
	auto nodeData = m_NodeDataMap.find(key);

	if (nodeData != m_NodeDataMap.end())
		return nodeData->second.lock();

	return nullptr;
}
shared_ptr<Model> ResourceManager::FindModel(string key)
{
	auto model = m_ModelMap.find(key);

	if (model != m_ModelMap.end())
		return model->second.lock();

	return nullptr;
}