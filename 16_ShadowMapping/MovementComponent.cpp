#include "MovementComponent.h"

#include "Object.h"
#include "../Engine/TimeManager.h"

MovementComponent::MovementComponent()
{
	m_Speed = 1.f;
}

MovementComponent::~MovementComponent()
{
}

void MovementComponent::Setting(MovementComponentInfo _info)
{
	__super::Setting(_info.m_ComponentInfo);
	m_Speed = _info.m_Speed;
}

void MovementComponent::AddInputVector(const Vector3& input)
{
	m_InputVector += input;
	m_InputVector.Normalize();
}

void MovementComponent::Update()
{
	if (m_InputVector.Length() > 0.f)
	{
		shared_ptr<CollisionComponent> collision = GetOwner()->GetComponent<CollisionComponent>()[0];

		m_PositionBefore = GetOwner()->GetPosition();
		m_PositionAfter = m_PositionBefore + m_InputVector * m_Speed * TimeManager::GetInstance()->GetfDT();
		GetOwner()->SetPosition(m_PositionAfter);
		m_InputVector = Vector3::Zero;
	}
}