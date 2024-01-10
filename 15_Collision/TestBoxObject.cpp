#include "TestBoxObject.h"

#include "RenderManager.h"
#include "CollisionManager.h"

#include "RenderComponent.h"
#include "BoxComponent.h"

#include "ImGuiMenu.h"

TestBoxObject::TestBoxObject(unsigned int _Id)
	: Object::Object(_Id)
{
}

TestBoxObject::~TestBoxObject()
{
}

void TestBoxObject::Setting(TestBoxObjectInfo _info)
{
	__super::Setting(_info.m_ObjectInfo);


	// 렌더 컴포넌트 세팅
	shared_ptr<RenderComponent> renderComponent = make_shared<RenderComponent>();
	RenderComponentInfo RenderComponentInfo;
	RenderComponentInfo.m_ComponentDesc.m_Name = _info.m_RenderName;
	RenderComponentInfo.m_ModelBuffer = RenderManager::GetInstance()->GetModelCB();
	RenderComponentInfo.m_Path = _info.m_RenderPath;
	renderComponent->Setting(RenderComponentInfo);

	// 콜리전 컴포넌트 세팅
	shared_ptr<BoxComponent> boxComponent = CollisionManager::GetInstance()->CreateCollision<BoxComponent>();
	BoxComponentInfo boxComponentInfo;
	boxComponentInfo.m_BoundingBox = renderComponent->GetBoundingBox();
	boxComponentInfo.m_CollisionComponentInfo.m_ComponentInfo.m_Name = "BoxCollisionComponent";
	boxComponentInfo.m_CollisionComponentInfo.m_CollisionNotify = this;
	boxComponentInfo.m_CollisionComponentInfo.m_CollisionType = CollisionType::Block;
	boxComponentInfo.m_CollisionComponentInfo.m_ComponentInfo.m_Local = SimpleMath::Matrix::CreateTranslation(Vector3(0.f, boxComponentInfo.m_BoundingBox.Extents.y, 0.f));
	boxComponent->Setting(boxComponentInfo);

	// 오브젝트에 컴포넌트 세팅
	SetComponent<RenderComponent>(renderComponent);
	SetComponent<BoxComponent>(boxComponent);
}

void TestBoxObject::Init()
{
	__super::Init();
}

void TestBoxObject::Update()
{
	__super::Update();
}

void TestBoxObject::LateUpdate()
{
	__super::LateUpdate();
}

void TestBoxObject::FixedUpdate()
{
	__super::FixedUpdate();
}

void TestBoxObject::Render()
{
	__super::Render();
}

void TestBoxObject::OnBeginOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraBeginOverlapCount++;
}

void TestBoxObject::OnEndOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraEndOverlapCount++;
}

void TestBoxObject::OnOverlap(unsigned int _otherObjectid)
{
	ImGuiMenu::CameraOverlapCount++;
}