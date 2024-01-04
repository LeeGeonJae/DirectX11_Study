#include "RenderComponent.h"
#include "Model.h"
#include "ResourceManager.h"

RenderComponent::RenderComponent()
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::Setting(RenderComponentDesc desc)
{
	__super::Setting(desc.m_ComponentDesc);

	m_Device = desc.m_Device;
	m_DeviceContext = desc.m_DeviceContext;
	m_ModelBuffer = desc.m_ModelBuffer;
	
	m_pModel = ResourceManager::GetInstance()->CreateModel(desc.m_Path);
}

void RenderComponent::Init()
{
	m_pModel->Init(m_Device, m_ModelBuffer);
}

void RenderComponent::Update()
{
	__super::Update();

	m_pModel->Update(m_DeviceContext, GetWorldTransform());
}

void RenderComponent::LateUpdate()
{
}

void RenderComponent::FixedUpdate()
{
}

void RenderComponent::Render()
{
	m_pModel->Render(m_DeviceContext);
}
