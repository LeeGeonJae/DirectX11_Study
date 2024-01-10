#pragma once
#include "Component.h"
#include "BufferStruct.h"

struct CameraComponentInfo
{
	ComponentInfo m_ComponentInfo;
};

class CameraComponent : public Component
{
public:
	CameraComponent();
	virtual ~CameraComponent();

public:
	void Setting(CameraComponentInfo info);

public:
	virtual void Init() ;
	virtual void Update() ;
	virtual void LateUpdate() ;
	virtual void FixedUpdate() ;
	virtual void Render() ;

public:
	inline const SimpleMath::Matrix& GetViewMatrix();
	inline const SimpleMath::Matrix& GetProjectionMatrix();

private:
	SimpleMath::Matrix m_View;
	SimpleMath::Matrix m_Projection;


};

const SimpleMath::Matrix& CameraComponent::GetViewMatrix()
{
	return m_View;
}
const SimpleMath::Matrix& CameraComponent::GetProjectionMatrix()
{
	return m_Projection;
}