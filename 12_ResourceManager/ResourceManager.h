#pragma once
#include "../Engine/pch.h"

class Material;
class Animation;
class Texture;

class ResourceManager
{
private:
	static ResourceManager* pInstance;

public:
	static ResourceManager* GetInstance();

public:
	ResourceManager();
	~ResourceManager();

public:
	void Initialize();
	void Uninitialize();

	// StaticMesh

	// SkeletalMesh

	// Skeleton

	// Animation

	// Material
	shared_ptr<Material> CreateMaterial(string key, aiMaterial* pAiMaterial);

private:
	// StaticMesh

	// SkeletalMesh
	
	// Skeleton
	map<string, weak_ptr<Bone>> m_BoneMap;
	// Animation
	map<string, weak_ptr<Animation>> m_AnimationMap;
	// Material
	map<string, weak_ptr<Material>> m_MaterialMap;

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
};