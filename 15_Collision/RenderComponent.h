#pragma once
#include "Struct.h"
#include "Component.h"

class Model;

struct RenderComponentInfo
{
	ComponentInfo m_ComponentDesc;
	shared_ptr<ModelCBBuffer> m_ModelBuffer;

	string m_Path;
};

class RenderComponent : public Component
{
public:
	RenderComponent();
	virtual ~RenderComponent();

public:
	void Setting(RenderComponentInfo info);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

public:
	inline shared_ptr<Model> GetModel();
	inline bool GetIsCulled();
	inline void SetIsCulled(bool isTrue);

	DirectX::BoundingBox GetBoundingBox();

private:
	bool m_bIsCulled;

	shared_ptr<Model> m_pModel;

	shared_ptr<ModelCBBuffer>	m_ModelBuffer;
	string m_Path;
};

shared_ptr<Model> RenderComponent::GetModel()
{
	return m_pModel;
}

bool RenderComponent::GetIsCulled()
{
	return m_bIsCulled;
}
void RenderComponent::SetIsCulled(bool isTrue)
{
	m_bIsCulled = isTrue;
}