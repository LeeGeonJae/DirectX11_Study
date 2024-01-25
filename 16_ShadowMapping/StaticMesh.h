#pragma once
#include "../Engine/Header.h"
#include "Mesh.h"
#include "Struct.h"
#include "Material.h"

class Vertex;

class StaticMesh : public Mesh
{
public:
	StaticMesh();
	~StaticMesh();

public:
	void ShadowDraw(ComPtr<ID3D11DeviceContext> deviceContext);
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext);
	void Close();

public:
	void SetupMesh(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer);

private:
	virtual void createVS() override;
	virtual void createPS() override;
	virtual void createInputLayout() override;

public:
	vector<Vertex> m_Vertices;
	vector<UINT> m_Indices;
};

