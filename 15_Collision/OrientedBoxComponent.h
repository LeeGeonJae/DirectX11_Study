#pragma once
#include "CollisionComponent.h"

struct OrientedBoxComponentInfo
{
	CollisionComponentInfo m_CollisionComponentInfo;
	DirectX::BoundingOrientedBox m_BoundingOrientedBox;
};

class OrientedBoxComponent : public CollisionComponent
{
public:
	OrientedBoxComponent(unsigned int _id);
	virtual ~OrientedBoxComponent();

public:
	virtual void Update();

public:
	void Setting(OrientedBoxComponentInfo _info);

public:
	virtual bool IsCollision(unsigned int _otherCollisionID);

public:
	inline const DirectX::BoundingOrientedBox& GetGeometry();

private:
	DirectX::BoundingOrientedBox m_Geometry;
};

const DirectX::BoundingOrientedBox& OrientedBoxComponent::GetGeometry()
{
	return m_Geometry;
}