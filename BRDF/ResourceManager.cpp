
#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Material> ResourceManager::CreateMaterial(std::string key, aiMaterial* pAiMaterial)
{
	return std::shared_ptr<Material>();
}

void ResourceManager::Initialize()
{
}

void ResourceManager::Uninitialize()
{
}
