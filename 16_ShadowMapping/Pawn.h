#pragma once
#include "Object.h"

class PawnController;

struct PawnInfo
{
	ObjectInfo m_ObjectDesc;
};

class Pawn : public Object
{
public:
	Pawn(unsigned int _id);
	virtual ~Pawn();

public:
	void Setting(PawnInfo info);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

public:
	virtual void OnBeginOverlap(unsigned int _otherObjectid) override;
	virtual void OnEndOverlap(unsigned int _otherObjectid) override;
	virtual void OnOverlap(unsigned int _otherObjectid) override;
	virtual void OnBlock(unsigned int _otherObjectid) override;

public:
	inline shared_ptr<PawnController> GetController();

	virtual void OnController(shared_ptr<PawnController> controller);
	virtual void UnController();

private:
	shared_ptr<PawnController> m_pController;
};

shared_ptr<PawnController> Pawn::GetController()
{
	return m_pController;
}

