#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"

class Object;

struct ComponentInfo
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
	void Setting(ComponentInfo info);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

private:
	void UpdateMatrix();

public:
	inline shared_ptr<Object> GetOwner();
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

shared_ptr<Object> Component::GetOwner()
{
	return m_pOwner.lock();
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