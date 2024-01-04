#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"

class Object;

struct ComponentDesc
{
	string m_Name;
	SimpleMath::Matrix m_Local = SimpleMath::Matrix::Identity;
};

class Component
{
public:
	Component();
	virtual ~Component();

public:
	void Setting(ComponentDesc desc);

public:
	virtual void Init() abstract;
	virtual void Update();
	virtual void LateUpdate() abstract;
	virtual void FixedUpdate() abstract;
	virtual void Render() abstract;

private:
	void UpdateMatrix();

public:
	inline weak_ptr<Object> GetOwner();
	inline void SetOwner(weak_ptr<Object> owner);
	inline string GetName();
	inline void SetName(string name);
	inline SimpleMath::Matrix GetLocalTransform();
	inline void SetLocalTransform(SimpleMath::Matrix local);
	inline SimpleMath::Matrix GetWorldTransform();

protected:
	string m_Name;
	weak_ptr<Object> m_pOwner;

	SimpleMath::Matrix m_Local;
	SimpleMath::Matrix m_World;
};

weak_ptr<Object> Component::GetOwner()
{
	return m_pOwner;
}
void Component::SetOwner(weak_ptr<Object> owner)
{
	m_pOwner = owner;
}

string Component::GetName()
{
	return m_Name;
}

void Component::SetName(string name)
{
	m_Name = name;
}

SimpleMath::Matrix Component::GetLocalTransform()
{
	return m_Local;
}

void Component::SetLocalTransform(SimpleMath::Matrix local)
{
	m_Local = local;
}

SimpleMath::Matrix Component::GetWorldTransform()
{
	return m_World;
}