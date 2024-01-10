#pragma once
#include "Component.h"

enum class ColliderType
{
	Box = 0,
	Sphere,
	OrientedBox,
	End
};

enum class CollisionType
{
	NoCollision = 0,
	Overlap,
	Block,
	End
};

enum class CollisionState
{
	None = 0,
	Overlap,
	Block,
	End
};

class CollisionComponent;

class CollisionNotify
{
public:
	virtual void OnBeginOverlap(unsigned int _otherObjectid) {}
	virtual void OnEndOverlap(unsigned int _otherObjectid) {}
	virtual void OnOverlap(unsigned int _otherObjectid) {}
	virtual void OnBlock(unsigned int _otherObjectid) {}
};

struct CollisionComponentInfo
{
	ComponentInfo m_ComponentInfo;
	CollisionNotify* m_CollisionNotify;
	CollisionType m_CollisionType;
};

class CollisionComponent : public Component
{
public:
	CollisionComponent(unsigned int _id);
	virtual ~CollisionComponent();

public:
	virtual void Setting(CollisionComponentInfo _info);

public:
	virtual void Update();

public:
	void ProcessOverlap(unsigned int _otherCollisionID);
	void ProcessBlock(unsigned int _otherCollisionID);
	void ProcessEscape(unsigned int _otherCollisionID);
	virtual bool IsCollision(unsigned int _otherCollisionID) abstract;

public:
	inline unsigned int GetID();
	inline const ColliderType& GetColliderType();
	inline void SetColliderType(ColliderType _colliderType);
	inline const CollisionType& GetCollisionType();
	inline void SetCollisionType(CollisionType _collisionType);
	inline const CollisionState& GetCollisionState();
	inline void SetCollisionState(CollisionState _collisionState);
	inline void SetCollisionNotify(CollisionNotify* _collisionNotify);

private:
	unsigned int m_ID;
	vector<unsigned int> m_otherCollisionID;
	CollisionNotify* m_CollisionNotify;

	ColliderType m_ColliderType = ColliderType::End;
	CollisionType m_CollisionType = CollisionType::End;
	CollisionState m_CollisionStateCurrent = CollisionState::None;

	friend class CollisionManager;
};

unsigned int CollisionComponent::GetID()
{
	return m_ID;
}
const ColliderType& CollisionComponent::GetColliderType()
{
	return m_ColliderType;
}
void CollisionComponent::SetColliderType(ColliderType _colliderType)
{
	m_ColliderType = _colliderType;
}
const CollisionType& CollisionComponent::GetCollisionType()
{
	return m_CollisionType;
}
void CollisionComponent::SetCollisionType(CollisionType _collisionType)
{
	m_CollisionType = _collisionType;
}
const CollisionState& CollisionComponent::GetCollisionState()
{
	return m_CollisionStateCurrent;
}
void CollisionComponent::SetCollisionState(CollisionState _collisionState)
{
	m_CollisionStateCurrent = _collisionState;
}
void CollisionComponent::SetCollisionNotify(CollisionNotify* _collisionNotify)
{
	m_CollisionNotify = _collisionNotify;
}
