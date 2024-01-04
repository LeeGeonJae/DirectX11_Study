#include "Component.h"
#include "Object.h"

Component::Component()
{
}

Component::~Component()
{
}

void Component::Setting(ComponentDesc desc)
{
	SetName(desc.m_Name);
	SetLocalTransform(desc.m_Local);
}

void Component::Update()
{
	UpdateMatrix();
}

void Component::UpdateMatrix()
{
	m_World = m_pOwner.lock()->GetWorldTransform() * m_Local;
}