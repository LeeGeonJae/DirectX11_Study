#include "Component.h"
#include "Object.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::Setting(ComponentInfo info)
{
	SetName(info.m_Name);
	SetLocalTransform(info.m_Local);
}

void Component::Init()
{

}

void Component::Update()
{
	UpdateMatrix();
}

void Component::LateUpdate()
{

}

void Component::FixedUpdate()
{

}

void Component::Render()
{

}

void Component::UpdateMatrix()
{
	m_World = m_pOwner.lock()->GetWorldTransform() * m_Local;
}