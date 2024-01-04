#pragma once
#include "../Engine/Header.h"
#include "Struct.h"
#include "Animation.h"

class ModelLoadManager;
class Material;
class AnimationNode;
class Animation;
class Texture;
class SkeletalMesh;
class StaticMesh;
class NodeData;
class ModelData;
class Bone;
class Skeletal;
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
	inline void SetStaticMesh(string key, shared_ptr<StaticMesh> staticMesh);
	inline void SetSkeletalMesh(string key, shared_ptr<SkeletalMesh> skeletalMesh);
	inline void SetSkeletal(string key, shared_ptr<Bone> bone);
	inline void SetAnimation(string key, shared_ptr<Animation> animation);
	inline void SetMaterial(string key, shared_ptr<Material> material);
	inline void SetNodeData(string key, shared_ptr<NodeData> nodeData);
	inline shared_ptr<StaticMesh> FindStaticMesh(string key);
	inline shared_ptr<SkeletalMesh> FindSkeletalMesh(string key);
	inline shared_ptr<Skeletal> FindSkeletal(string key);
	inline shared_ptr<Animation> FindAnimation(string key);
	inline shared_ptr<Material> FindMaterial(string key);
	inline shared_ptr<ModelData> FindModelData(string key);

private:
	// StaticMesh
	map<string, shared_ptr<StaticMesh>> m_StaticMeshMap;
	// SkeletalMesh
	map<string, shared_ptr<SkeletalMesh>> m_SkeletalMeshMap;
	// Skeleton
	map<string, shared_ptr<Skeletal>> m_SkeletalMap;
	// Animation
	map<string, shared_ptr<Animation>> m_AnimationMap;
	// Material
	map<string, shared_ptr<Material>> m_MaterialMap;
	// ModelData
	map<string, shared_ptr<ModelData>> m_ModelDataMap;

private:
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_DeviceContext;
};

void ResourceManager::SetStaticMesh(string key, shared_ptr<StaticMesh> staticMesh)
{
	auto staticmesh = FindStaticMesh(key);

	if (staticmesh == nullptr)
		m_StaticMeshMap.insert(make_pair(key, staticMesh));
}
void ResourceManager::SetSkeletalMesh(string key, shared_ptr<SkeletalMesh> skeletalMesh)
{
	auto skeletalmesh = FindSkeletalMesh(key);

	if (skeletalmesh == nullptr)
		m_SkeletalMeshMap.insert(make_pair(key, skeletalMesh));
}
void ResourceManager::SetSkeletal(string key, shared_ptr<Bone> bone)
{
	auto findSkeletal = FindSkeletal(key);

	if (findSkeletal == nullptr)
	{
		findSkeletal = make_shared<Skeletal>();
		findSkeletal->m_Bones.push_back(bone);

		m_SkeletalMap.insert(make_pair(key, findSkeletal));
	}
	else
	{
		findSkeletal->m_Bones.push_back(bone);
	}
}
void ResourceManager::SetAnimation(string key, shared_ptr<Animation> animation)
{
	auto findAnimation = FindAnimation(key);

	if (findAnimation == nullptr)
		m_AnimationMap.insert(make_pair(key, animation));
}
void ResourceManager::SetMaterial(string key, shared_ptr<Material> material)
{
	auto findMaterial = FindMaterial(key);

	if (findMaterial == nullptr)
		m_MaterialMap.insert(make_pair(key, material));
}
void ResourceManager::SetNodeData(string key, shared_ptr<NodeData> nodeData)
{
	auto findModelData = FindModelData(key);

	if (findModelData == nullptr)
	{
		findModelData = make_shared<ModelData>();
		findModelData->m_Nodes.push_back(nodeData);

		m_ModelDataMap.insert(make_pair(key, findModelData));
	}
	else
	{
		findModelData->m_Nodes.push_back(nodeData);
	}
}

shared_ptr<StaticMesh> ResourceManager::FindStaticMesh(string key)
{
	auto staticMesh = m_StaticMeshMap.find(key);

	if (staticMesh != m_StaticMeshMap.end())
		return staticMesh->second;

	return nullptr;
}
shared_ptr<SkeletalMesh> ResourceManager::FindSkeletalMesh(string key)
{
	auto skeletalMesh = m_SkeletalMeshMap.find(key);

	if (skeletalMesh != m_SkeletalMeshMap.end())
		return skeletalMesh->second;

	return nullptr;
}
shared_ptr<Skeletal> ResourceManager::FindSkeletal(string key)
{
	auto bone = m_SkeletalMap.find(key);

	if (bone != m_SkeletalMap.end())
		return bone->second;

	return nullptr;
}
shared_ptr<Animation> ResourceManager::FindAnimation(string key)
{
	auto animation = m_AnimationMap.find(key);

	if (animation != m_AnimationMap.end())
		return animation->second;

	return nullptr;
}
shared_ptr<Material> ResourceManager::FindMaterial(string key)
{
	auto material = m_MaterialMap.find(key);

	if (material != m_MaterialMap.end())
		return material->second;

	return nullptr;
}
shared_ptr<ModelData> ResourceManager::FindModelData(string key)
{
	auto modelData = m_ModelDataMap.find(key);

	if (modelData != m_ModelDataMap.end())
		return modelData->second;

	return nullptr;
}