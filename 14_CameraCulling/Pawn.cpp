#include "Pawn.h"

#include "../Engine/InputManager.h"
#include "CameraComponent.h"
#include "PawnController.h"
#include "../Engine/TimeManager.h"

constexpr float ROTATION_GAIN = 0.004f;
constexpr float MOVEMENT_GAIN = 0.07f;

Pawn::Pawn()
{
}

Pawn::~Pawn()
{
}

void Pawn::Setting(PawnDesc desc)
{
	Object::Setting(desc.m_ObjectDesc);
}

void Pawn::Init()
{
	__super::Init();

}

void Pawn::Update()
{
	if (m_pController == nullptr)
		return;

	shared_ptr<CameraComponent> camera = GetComponent<CameraComponent>()[0];

	float fowardScale = 0.0f, rightScale = 0.0f, upScale = 0.0f;
	float speed = 400.f;

	SimpleMath::Matrix rotMatrix = SimpleMath::Matrix::CreateFromYawPitchRoll(m_pController->GetYaw(), m_pController->GetPitch(), 0.0f);
	SimpleMath::Vector3 forward = Vector3(rotMatrix._31, rotMatrix._32, rotMatrix._33);
	SimpleMath::Vector3 right = rotMatrix.Right();

	SimpleMath::Vector3 inputVector = SimpleMath::Vector3::Zero;

	if (InputManager::GetInstance()->GetKeyboardStateTracker().IsKeyPressed(Keyboard::Keys::R))
	{
		SetWorldTransform(SimpleMath::Matrix::Identity);
		m_pController->SetPitch(0.0f);
		m_pController->SetYaw(0.0f);
	}

	if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::W))
	{
		inputVector += forward;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::S))
	{
		inputVector -= forward;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}

	if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::A))
	{
		inputVector -= right;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::D))
	{
		inputVector += right;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}

	if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::Q))
	{
		inputVector -= Vector3::Up;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::E))
	{
		inputVector += Vector3::Up;
		inputVector.Normalize();

		SimpleMath::Vector3 position = GetPosition();
		position += inputVector * speed * TimeManager::GetInstance()->GetfDT();
		SetPosition(position);
	}

	InputManager::GetInstance()->m_Mouse->SetMode(InputManager::GetInstance()->GetMouseState().rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (InputManager::GetInstance()->GetMouseState().positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(InputManager::GetInstance()->GetMouseState().x), float(InputManager::GetInstance()->GetMouseState().y), 0.f) * ROTATION_GAIN;
		m_pController->AddPitch(delta.y);
		m_pController->AddYaw(delta.x);

		SetRotation(Vector3(m_pController->GetPitch(), m_pController->GetYaw(), 0.0f));
	}

	__super::Update();
}

void Pawn::LateUpdate()
{
	__super::LateUpdate();

}

void Pawn::FixedUpdate()
{
	__super::FixedUpdate();

}

void Pawn::Render()
{
	__super::Render();

}

void Pawn::OnController(shared_ptr<PawnController> controller)
{
	m_pController = controller;
}

void Pawn::UnController()
{
	m_pController = nullptr;
}