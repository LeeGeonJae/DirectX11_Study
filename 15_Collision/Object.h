#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"

#include "CameraComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"

class ObjectManager;

struct ObjectInfo
{
	string m_Name;
	Vector3 m_Position = Vector3::Zero;
	Vector3 m_Rotation = Vector3::Zero;
	Vector3 m_Scale = Vector3{ 1.f, 1.f, 1.f };
};

class Object : public enable_shared_from_this<Object>, public CollisionNotify
{
public:
	Object(unsigned int _Id);
	virtual ~Object();

public:
	virtual void Setting(ObjectInfo info);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

public:
	template <typename T>
	inline void SetComponent(shared_ptr<T> component);
	template <typename T>
	inline vector<shared_ptr<T>> GetComponent();

public:
	inline void SetName(string name);
	inline string GetName();
	inline void SetWorldTransform(SimpleMath::Matrix world);
	inline SimpleMath::Matrix GetWorldTransform();
	
	inline void SetPosition(Vector3 position);
	inline Vector3 GetPosition();
	inline void SetRotation(Vector3 rotation);
	inline Vector3 GetRotation();
	inline void SetScale(Vector3 scale);
	inline Vector3 GetScale();
	inline const unsigned int& GetID();

private:
	string m_Name;
	unsigned int m_ID;
	vector<shared_ptr<Component>> m_pMyComponents;

	SimpleMath::Matrix m_World;
	Vector3 m_Position;
	Vector3 m_Rotation;
	Vector3 m_Scale;

	friend ObjectManager;
};

template <typename T>
void Object::SetComponent(shared_ptr<T> component)
{
	bool bIsBase = std::is_base_of<Component, T>::value;
	assert(bIsBase == true);

	component->SetOwner(shared_from_this());
	m_pMyComponents.push_back(component);
}

template <typename T>
vector<shared_ptr<T>> Object::GetComponent()
{
	bool bIsBase = std::is_base_of<Component, T>::value;
	assert(bIsBase == true);

	vector<shared_ptr<T>> Components;

	for (auto component : m_pMyComponents)
	{
		shared_ptr<T> cameraComponent = dynamic_pointer_cast<T>(component);
		if (cameraComponent != nullptr)
			Components.push_back(cameraComponent);
	}

	return Components;
}

void Object::SetName(string name)
{
	m_Name = name;
}
string Object::GetName()
{
	return m_Name;
}
void Object::SetWorldTransform(SimpleMath::Matrix world)
{
	m_World = world;
}
SimpleMath::Matrix Object::GetWorldTransform()
{
	return m_World;
}

void Object::SetPosition(Vector3 position)
{
	m_Position = position;
}
Vector3 Object::GetPosition()
{
	return m_Position;
}
void Object::SetRotation(Vector3 rotation)
{
	m_Rotation = rotation;
}
Vector3 Object::GetRotation()
{
	return m_Rotation;
}
void Object::SetScale(Vector3 scale)
{
	m_Scale = scale;
}
Vector3 Object::GetScale()
{
	return m_Scale;
}
const unsigned int& Object::GetID()
{
	return m_ID;
}