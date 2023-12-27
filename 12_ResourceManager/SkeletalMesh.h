#pragma once
#include "../Engine/Header.h"
#include "Mesh.h"
#include "Struct.h"
#include "Material.h"

class BoneWeightVertex;

class SkeletalMesh : public Mesh
{
public:
	SkeletalMesh();
	~SkeletalMesh();

public:
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext);
	void Close();

public:
	virtual void SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer) override;

private:
	virtual void createVS() override;
	virtual void createPS() override;
	virtual void createInputLayout() override;

public:
	vector<BoneWeightVertex>	m_BoneWeightVertices;
	vector<UINT>				m_Indices;
};

