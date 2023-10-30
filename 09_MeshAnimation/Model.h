#pragma once
#include "../Engine/Header.h"
#include "Struct.h"
#include "BufferStruct.h"

class Node;

class Model
{
public:
	Model();
	~Model();

public:
	void Draw();

private:


private:
	ComPtr<ID3D11Device>			m_Device;
	ComPtr<ID3D11DeviceContext>		m_DeviceContext;
	vector<Node*>					m_Meshes;
};