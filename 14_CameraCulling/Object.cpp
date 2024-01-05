#include "Object.h"
#include "Component.h"

#include "RenderComponent.h"
#include "RenderManager.h"

Object::Object()
{

}

Object::~Object()
{

}

void Object::Setting(ObjectDesc desc)
{
	SetName(desc.m_Name);
	SetPosition(desc.m_Position);
	SetRotation(desc.m_Rotation);
	SetScale(desc.m_Scale);
}

void Object::Init()
{
	for (auto component : m_pMyComponents)
	{
		component->Init();
	}
}

void Object::Update()
{
	m_World = SimpleMath::Matrix::CreateScale(m_Scale) 
		* SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation) 
		* SimpleMath::Matrix::CreateTranslation(m_Position);

	for (auto component : m_pMyComponents)
	{
		component->Update();
	}
}

void Object::LateUpdate()
{
	for (auto component : m_pMyComponents)
	{
		component->LateUpdate();
	}
}

void Object::FixedUpdate()
{
	for (auto component : m_pMyComponents)
	{
		component->FixedUpdate();
	}
}

void Object::Render()
{
	for (auto component : m_pMyComponents)
	{
		shared_ptr<RenderComponent> renderComponent = dynamic_pointer_cast<RenderComponent>(component);

		if (renderComponent)
		{
			RenderManager::GetInstance()->SetRender(renderComponent);
		}
	}
}