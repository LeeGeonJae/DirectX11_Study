#pragma once
#include "CollisionComponent.h"

struct BoxComponentInfo
{
	CollisionComponentInfo m_CollisionComponentInfo;
	DirectX::BoundingBox m_BoundingBox;
};

class BoxComponent : public CollisionComponent
{
public:
	BoxComponent(unsigned int _id);
	virtual ~BoxComponent();

public:
	virtual void Update();

public:
	void Setting(BoxComponentInfo _info);

public:
	virtual bool IsCollision(unsigned int _otherCollisionID);

public:
	inline const DirectX::BoundingBox& GetGeometry();

private:
	DirectX::BoundingBox m_Geometry;
};

const DirectX::BoundingBox& BoxComponent::GetGeometry()
{
	return m_Geometry;
}