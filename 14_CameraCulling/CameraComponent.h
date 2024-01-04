#pragma once
#include "Component.h"
#include "BufferStruct.h"

struct CameraComponentDesc
{
	ComponentDesc m_ComponentDesc;

	ComPtr<ID3D11DeviceContext> m_DeviceContext;
	CBCoordinateData m_CBCoordinateData;
	CBCameraData m_CBCamera;
	ComPtr<ID3D11Buffer> m_pCBCoordinateData;
	ComPtr<ID3D11Buffer> m_pCBCamera;
};

class CameraComponent : public Component
{
public:
	CameraComponent();
	virtual ~CameraComponent();

public:
	void Setting(CameraComponentDesc desc);

public:
	virtual void Init() ;
	virtual void Update() ;
	virtual void LateUpdate() ;
	virtual void FixedUpdate() ;
	virtual void Render() ;

public:
	void GetViewMatrix(SimpleMath::Matrix& ViewMatrix);

private:
	ComPtr<ID3D11DeviceContext> m_DeviceContext;
	CBCoordinateData			m_CBCoordinateData;
	CBCameraData				m_CBCamera;
	ComPtr<ID3D11Buffer>		m_CBCoordinateBuffer;
	ComPtr<ID3D11Buffer>		m_CBCameraBuffer;
};

