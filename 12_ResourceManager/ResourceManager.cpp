#include "ResourceManager.h"

#include "ModelLoadManager.h"
#include "Model.h"

void ResourceManager::Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
}

shared_ptr<Model> ResourceManager::CreateModel(string path)
{
	shared_ptr<Model> model = FindModel(path);

	if (model == nullptr)
	{
		model = ModelLoadManager::GetInstance()->Load(path);
	}

	return model;
}