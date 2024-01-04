#pragma once
#include "Object.h"

class PawnController;

struct PawnDesc
{
	ObjectDesc m_ObjectDesc;
};

class Pawn : public Object
{
public:
	Pawn();
	~Pawn();

public:
	void Setting(PawnDesc desc);

public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void FixedUpdate();
	virtual void Render();

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

