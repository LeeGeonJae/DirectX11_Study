#pragma once
#include "../Engine/Header.h"

//#include "Object.h"
//#include "Pawn.h"

class Object;
class Pawn;

class ObjectManager
{
private:
	ObjectManager() {}
public:
	static ObjectManager* GetInstance()
	{
		static ObjectManager instance;
		return &instance;
	}

public:
	inline shared_ptr<Object> FindObject(unsigned int _id);

public:
	template <typename T>
	shared_ptr<T> CreateObject();

private:
	unsigned int m_ObjectId = 0;
	unordered_map<unsigned int, shared_ptr<Object>> m_ObjectMap;
};

template <typename T>
inline shared_ptr<T> ObjectManager::CreateObject()
{
	// 해당 클래스가 오브젝트를 상속받지 않는 클래스라면 오류
	bool bIsBase = std::is_base_of<Object, T>::value;
	assert(bIsBase == true);

	shared_ptr<T> object = make_shared<T>(m_ObjectId);
	m_ObjectMap.insert(make_pair(m_ObjectId++, object));

	return object;
}

shared_ptr<Object> ObjectManager::FindObject(unsigned int _id)
{
	auto object = m_ObjectMap.find(_id);
	assert(object != m_ObjectMap.end());

	return object->second;
}