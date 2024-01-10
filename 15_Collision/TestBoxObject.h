#pragma once
#include "Object.h"

struct TestBoxObjectInfo
{
	ObjectInfo m_ObjectInfo;
	string m_RenderName;
	string m_RenderPath;
};

class TestBoxObject : public Object
{
public:
	TestBoxObject(unsigned int _Id);
	virtual ~TestBoxObject();

public:
	virtual void Setting(TestBoxObjectInfo _info);

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void LateUpdate() override;
	virtual void FixedUpdate() override;
	virtual void Render() override;

public:
	virtual void OnBeginOverlap(unsigned int _otherObjectid) override;
	virtual void OnEndOverlap(unsigned int _otherObjectid) override;
	virtual void OnOverlap(unsigned int _otherObjectid) override;
};

