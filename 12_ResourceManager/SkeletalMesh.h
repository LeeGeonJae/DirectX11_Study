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
	void SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer);

public:
	vector<BoneWeightVertex>	m_BoneWeightVertices;
	vector<UINT>				m_Indices;
};

