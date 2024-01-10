#include "Pawn.h"

#include "../Engine/InputManager.h"
#include "../Engine/TimeManager.h"
#include "RenderManager.h"
#include "CollisionManager.h"
#include "ObjectManager.h"

#include "CameraComponent.h"
#include "SphereComponent.h"
#include "BoxComponent.h"
#include "MovementComponent.h"
#include "PawnController.h"
#include "ImGuiMenu.h"

constexpr float ROTATION_GAIN = 0.004f;
constexpr float MOVEMENT_GAIN = 0.07f;

Pawn::Pawn(unsigned int _id)
	: Object::Object(_id)
{
}

Pawn::~Pawn()
{
}

void Pawn::Setting(PawnInfo info)
{
	Object::Setting(info.m_ObjectDesc);

	// 카메라 컴포넌트
	shared_ptr<CameraComponent> camera = make_shared<CameraComponent>();
	CameraComponentInfo cameraInfo;
	cameraInfo.m_ComponentInfo.m_Name = "CameraComponent";
	camera->Setting(cameraInfo);

	// 무브먼트 컴포넌트
	shared_ptr<MovementComponent> movement = make_shared<MovementComponent>();
	MovementComponentInfo movementInfo;
	movementInfo.m_ComponentInfo.m_Name = "MovementComponent";
	movementInfo.m_Speed = 400.f;
	movement->Setting(movementInfo);

	// 콜리전 컴포넌트
	shared_ptr<SphereComponent> sphere = CollisionManager::GetInstance()->CreateCollision<SphereComponent>();
	SphereComponentInfo sphereComponentInfo;
	sphereComponentInfo.m_CollisionComponentInfo.m_CollisionNotify = this;
	sphereComponentInfo.m_CollisionComponentInfo.m_CollisionType = CollisionType::Block;
	sphereComponentInfo.m_CollisionComponentInfo.m_ComponentInfo.m_Name = "SphereComponent";
	//sphereComponentInfo.m_CollisionComponentInfo.m_ComponentInfo.m_Local = SimpleMath::Matrix::CreateTranslation(Vector3(0.f, 5.f, 100.f));
	sphereComponentInfo.m_BoundingSphere = DirectX::BoundingSphere();
	sphereComponentInfo.m_BoundingSphere.Radius = 5.f;
	sphere->Setting(sphereComponentInfo);

	// 오브젝트에 컴포넌트 세팅
	SetComponent<CameraComponent>(camera);
	SetComponent<SphereComponent>(sphere);
	SetComponent<MovementComponent>(movement);

	// 폰 컨트롤러 세팅
	shared_ptr<PawnController> pawnController = make_shared<PawnController>();
	pawnController->SetPawn(shared_from_this());

	RenderManager::GetInstance()->SetCamera(camera);
}

void Pawn::Init()
{
	__super::Init();

}

void Pawn::Update()
{
	__super::Update();

	if (m_pController == nullptr)
		return;

	shared_ptr<CameraComponent> camera = GetComponent<CameraComponent>()[0];
	shared_ptr<MovementComponent> movement = GetComponent<MovementComponent>()[0];
	shared_ptr<SphereComponent> sphere = GetComponent<SphereComponent>()[0];

	if (ImGuiMenu::IsBlockNotOverlap)
	{
		sphere->SetCollisionType(CollisionType::Block);
	}
	else
	{
		sphere->SetCollisionType(CollisionType::Overlap);
	}

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
		movement->AddInputVector(forward);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::S))
	{
		movement->AddInputVector(-forward);
	}

	if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::A))
	{
		movement->AddInputVector(-right);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::D))
	{
		movement->AddInputVector(right);
	}

	if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::Q))
	{
		movement->AddInputVector(-Vector3::Up);
	}
	else if (InputManager::GetInstance()->GetKeyboardState().IsKeyDown(DirectX::Keyboard::Keys::E))
	{
		movement->AddInputVector(Vector3::Up);
	}

	InputManager::GetInstance()->m_Mouse->SetMode(InputManager::GetInstance()->GetMouseState().rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (InputManager::GetInstance()->GetMouseState().positionMode == Mouse::MODE_RELATIVE)
	{
		Vector3 delta = Vector3(float(InputManager::GetInstance()->GetMouseState().x), float(InputManager::GetInstance()->GetMouseState().y), 0.f) * ROTATION_GAIN;
		m_pController->AddPitch(delta.y);
		m_pController->AddYaw(delta.x);

		SetRotation(Vector3(m_pController->GetPitch(), m_pController->GetYaw(), 0.0f));
	}

	ImGuiMenu::CameraBeforePosition = movement->GetBeforePosition();
	ImGuiMenu::CameraAfterPosition = movement->GetAfterPosition();
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

void Pawn::OnBeginOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraBeginOverlapCount++;
}

void Pawn::OnEndOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraEndOverlapCount++;
}

void Pawn::OnOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraOverlapCount++;
}

void Pawn::OnBlock(unsigned int _otherObjectid)
{
	shared_ptr<Object> object = ObjectManager::GetInstance()->FindObject(_otherObjectid);
	Vector3 otherObjectPosition = object->GetComponent<BoxComponent>()[0]->GetGeometry().Center;
	Vector3 otherExtents = object->GetComponent<BoxComponent>()[0]->GetGeometry().Extents;

	shared_ptr<SphereComponent> sphere = GetComponent<SphereComponent>()[0];
	Vector3 sphereCenter = sphere->GetGeometry().Center;
	float radius = sphere->GetGeometry().Radius;

	Vector3 distance = sphereCenter - otherObjectPosition;
	float penetrationDepthX = (sphere->GetGeometry().Radius + otherExtents.x) - abs(distance.x);
	float penetrationDepthY = (sphere->GetGeometry().Radius + otherExtents.y) - abs(distance.y);
	float penetrationDepthZ = (sphere->GetGeometry().Radius + otherExtents.z) - abs(distance.z);

	Vector3 pushVector = { penetrationDepthX, penetrationDepthY, penetrationDepthZ };
	distance.Normalize();
	pushVector *= distance;

	shared_ptr<MovementComponent> move = GetComponent<MovementComponent>()[0];

	if (penetrationDepthX < penetrationDepthY && penetrationDepthX < penetrationDepthZ && sphereCenter.x > otherObjectPosition.x)
	{
		move->AddInputVector(Vector3(penetrationDepthX, 0.f, 0.f));
	}
	else if (penetrationDepthX < penetrationDepthY && penetrationDepthX < penetrationDepthZ && sphereCenter.x < otherObjectPosition.x)
	{
		move->AddInputVector(Vector3(-penetrationDepthX, 0.f, 0.f));
	}
	else if (penetrationDepthY < penetrationDepthX && penetrationDepthY < penetrationDepthZ && sphereCenter.y > otherObjectPosition.y)
	{
		move->AddInputVector(Vector3(0.f, penetrationDepthY, 0.f));
	}
	else if (penetrationDepthY < penetrationDepthX && penetrationDepthY < penetrationDepthZ && sphereCenter.y < otherObjectPosition.y)
	{
		move->AddInputVector(Vector3(0.f, -penetrationDepthY, 0.f));
	}
	else if (penetrationDepthZ < penetrationDepthX && penetrationDepthZ < penetrationDepthY && sphereCenter.z > otherObjectPosition.z)
	{
		move->AddInputVector(Vector3(0.f, 0.f, penetrationDepthZ));
	}
	else if (penetrationDepthZ < penetrationDepthX && penetrationDepthZ < penetrationDepthY && sphereCenter.z < otherObjectPosition.z)
	{
		move->AddInputVector(Vector3(0.f, 0.f, -penetrationDepthZ));
	}
}
