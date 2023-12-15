#include "ResourceManager.h"
#include "Mesh.h"

ResourceManager* ResourceManager::pInstance = nullptr;

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

std::shared_ptr<Material> ResourceManager::CreateMaterial(std::string key, aiMaterial* pAiMaterial)
{
	auto material = m_MaterialMap.find(key);

	if (material != m_MaterialMap.end())
		return material->second.lock();


}

void ResourceManager::Initialize()
{

}

void ResourceManager::Uninitialize()
{

}