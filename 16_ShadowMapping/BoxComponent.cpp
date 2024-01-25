#include "BoxComponent.h"

#include "CollisionManager.h"
#include "RenderManager.h"

#include "SphereComponent.h"
#include "OrientedBoxComponent.h"

BoxComponent::BoxComponent(unsigned int _id)
	: CollisionComponent::CollisionComponent(_id)
{
	SetColliderType(ColliderType::Box);
}

BoxComponent::~BoxComponent()
{
}

void BoxComponent::Update()
{
	__super::Update();

	m_Geometry.Center.x = m_World.Translation().x;
	m_Geometry.Center.y = m_World.Translation().y;
	m_Geometry.Center.z = m_World.Translation().z;
}

void BoxComponent::Setting(BoxComponentInfo _info)
{
	RenderManager::GetInstance()->SetBox(this);

	__super::Setting(_info.m_CollisionComponentInfo);
	m_Geometry = _info.m_BoundingBox;
}

bool BoxComponent::IsCollision(unsigned int _otherCollisionID)
{
	auto collision = CollisionManager::GetInstance()->FindCollision(_otherCollisionID);
	assert(collision);

	auto boxCollision = dynamic_pointer_cast<BoxComponent>(collision);
	if (boxCollision)
	{
		return m_Geometry.Intersects(boxCollision->GetGeometry());
	}

	auto sphereCollision = dynamic_pointer_cast<SphereComponent>(collision);
	if (sphereCollision)
	{
		return m_Geometry.Intersects(sphereCollision->GetGeometry());
	}

	auto orientedCollision = dynamic_pointer_cast<OrientedBoxComponent>(collision);
	if (sphereCollision)
	{
		m_Geometry.Intersects(orientedCollision->GetGeometry());
	}
}
