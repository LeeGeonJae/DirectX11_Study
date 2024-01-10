#pragma once
#include "Component.h"

class Object;
class Pawn;

struct MovementComponentInfo
{
	ComponentInfo m_ComponentInfo;
	float m_Speed;
};

class MovementComponent : public Component
{
public:
	MovementComponent();
	virtual ~MovementComponent();

public:
	void Setting(MovementComponentInfo _info);

public:
	virtual void Update() override;
	void AddInputVector(const Vector3& input);

public:
	inline float GetSpeed();
	inline void SetSpeed(float _speed);
	inline Vector3 GetBeforePosition();
	inline void SetBeforePosition(Vector3 _position);
	inline Vector3 GetAfterPosition();
	inline void SetAfterPosition(Vector3 _position);
	inline Vector3 GetInputVector();

private:
	Vector3 m_InputVector;
	float m_Speed;

	Vector3 m_PositionBefore;
	Vector3 m_PositionAfter;
};

float MovementComponent::GetSpeed()
{
	return m_Speed;
}
void MovementComponent::SetSpeed(float _speed)
{
	m_Speed = _speed;
}
Vector3 MovementComponent::GetBeforePosition()
{
	return m_PositionBefore;
}
void MovementComponent::SetBeforePosition(Vector3 _position)
{
	m_PositionBefore = _position;
}
Vector3 MovementComponent::GetAfterPosition()
{
	return m_PositionAfter;
}
void MovementComponent::SetAfterPosition(Vector3 _position)
{
	m_PositionAfter = _position;
}

inline Vector3 MovementComponent::GetInputVector()
{
	return Vector3();
}
