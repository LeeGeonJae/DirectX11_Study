#include "PawnController.h"
#include "Pawn.h"

PawnController::PawnController()
{
}

PawnController::~PawnController()
{
}

void PawnController::AddPitch(float value)
{
	m_Pitch += value;
	if (m_Pitch > XM_PI)
	{
		m_Pitch -= XM_2PI;
	}
	else if (m_Pitch < -XM_PI)
	{
		m_Pitch += XM_2PI;
	}
}
void PawnController::AddYaw(float value)
{
	m_Yaw += value;
	if (m_Yaw > XM_PI)
	{
		m_Yaw -= XM_2PI;
	}
	else if (m_Yaw < -XM_PI)
	{
		m_Yaw += XM_2PI;
	}
}

void PawnController::SetPawn(shared_ptr<Object> object)
{
	shared_ptr<Pawn> pawn = dynamic_pointer_cast<Pawn>(object);
	assert(pawn != nullptr);

	if (m_pPawn.lock() != nullptr)
	{
		m_pPawn.lock()->UnController();
	}

	m_pPawn = pawn;
	m_pPawn.lock()->OnController(shared_from_this());
}