#pragma once
#include "CollisionComponent.h"

struct SphereComponentInfo
{
	CollisionComponentInfo m_CollisionComponentInfo;
	DirectX::BoundingSphere m_BoundingSphere;
};

class SphereComponent : public CollisionComponent
{
public:
	SphereComponent(unsigned int _id);
	virtual ~SphereComponent();

public:
	virtual void Update();

public:
	void Setting(SphereComponentInfo _info);

public:
	virtual bool IsCollision(unsigned int _otherCollisionID);

public:
	inline const DirectX::BoundingSphere& GetGeometry();

private:
	DirectX::BoundingSphere m_Geometry;
};

const DirectX::BoundingSphere& SphereComponent::GetGeometry()
{
	return m_Geometry;
}