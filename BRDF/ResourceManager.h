#pragma once
#include "../Engine/pch.h"

class Material;
class Animation;

class ResourceManager
{
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
	std::shared_ptr<Material> CreateMaterial(std::string key, aiMaterial* pAiMaterial);

private:
	// StaticMesh

	// SkeletalMesh
	
	// Skeleton
	
	// Animation
	std::map<std::string, std::weak_ptr<Animation>> m_AnimationMap;
	// Material
	std::map<std::string, std::weak_ptr<Material>> m_MaterialMap;


};