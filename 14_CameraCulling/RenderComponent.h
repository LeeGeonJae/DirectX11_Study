#pragma once
#include "Struct.h"
#include "Component.h"

class Model;

struct RenderComponentDesc
{
	ComponentDesc m_ComponentDesc;
	ComPtr<ID3D11Device> m_Device;
	ComPtr<ID3D11DeviceContext> m_DeviceContext;
	shared_ptr<ModelCBBuffer> m_ModelBuffer;

	string m_Path;
};

class RenderComponent : public Component
{
public:
	RenderComponent();
	virtual ~RenderComponent();

public:
	void Setting(RenderComponentDesc desc);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

private:
	shared_ptr<Model> m_pModel;

	ComPtr<ID3D11Device>		m_Device;
	ComPtr<ID3D11DeviceContext> m_DeviceContext;
	shared_ptr<ModelCBBuffer>	m_ModelBuffer;
	string m_Path;
};

