#pragma once

#include "../Engine/Header.h"
#include "../Engine/Types.h"

class Object;
class Pawn;

class PawnController : public enable_shared_from_this<PawnController>
{
public:
	PawnController();
	~PawnController();

public:
	inline void SetPitch(float pitch);
	inline float GetPitch();
	inline void SetYaw(float yaw);
	inline float GetYaw();
	void AddPitch(float value);
	void AddYaw(float value);

	inline shared_ptr<Pawn> GetPawn();
	void SetPawn(shared_ptr<Object> object);

private:
	weak_ptr<Pawn> m_pPawn;

	float m_Pitch = 0.f;
	float m_Yaw = 0.f;
};

void PawnController::SetPitch(float pitch)
{
	m_Pitch = pitch;
}
float PawnController::GetPitch()
{
	return m_Pitch;
}
void PawnController::SetYaw(float yaw)
{
	m_Yaw = yaw;
}
float PawnController::GetYaw()
{
	return m_Yaw;
}

shared_ptr<Pawn> PawnController::GetPawn()
{
	return m_pPawn.lock();
}