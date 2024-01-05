#include "RenderComponent.h"
#include "Model.h"
#include "ResourceManager.h"
#include "RenderManager.h"
#include "Node.h"
#include "Mesh.h"

RenderComponent::RenderComponent()
{
}

RenderComponent::~RenderComponent()
{
}

void RenderComponent::Setting(RenderComponentDesc desc)
{
	__super::Setting(desc.m_ComponentDesc);

	m_ModelBuffer = desc.m_ModelBuffer;
	
	m_pModel = ResourceManager::GetInstance()->CreateModel(desc.m_Path);
}

void RenderComponent::Init()
{
	m_pModel->Init(DEVICE, m_ModelBuffer);
}

void RenderComponent::Update()
{
	__super::Update();
}

void RenderComponent::LateUpdate()
{
}

void RenderComponent::FixedUpdate()
{
}

void RenderComponent::Render()
{
	m_pModel->Update(DEVICE_CONTEXT, GetWorldTransform());
	m_pModel->Render(DEVICE_CONTEXT);
}

DirectX::BoundingBox RenderComponent::GetBoundingBox()
{
	DirectX::BoundingBox boundingBox = m_pModel->GetBoundingBox();
	SimpleMath::Vector3 world = GetWorldTransform().Translation();

	boundingBox.Center.x += world.x;
	boundingBox.Center.y += world.y;
	boundingBox.Center.z += world.z;

	return boundingBox;
}