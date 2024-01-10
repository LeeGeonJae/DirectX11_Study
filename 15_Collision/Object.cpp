#include "Object.h"
#include "Component.h"

#include "RenderComponent.h"
#include "RenderManager.h"

Object::Object(unsigned int _id)
	:m_ID(_id)
{
}

Object::~Object()
{

}

void Object::Setting(ObjectInfo info)
{
	SetName(info.m_Name);
	SetPosition(info.m_Position);
	SetRotation(info.m_Rotation);
	SetScale(info.m_Scale);
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