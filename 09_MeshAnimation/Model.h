#pragma once
#include "../Engine/Header.h"
#include "Struct.h"
#include "BufferStruct.h"

class Mesh;

class Model
{
public:
	Model();
	~Model();

private:


private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	vector<Mesh>			m_Meshes;
	vector<Texture>			m_Textures;
	shared_ptr<asAnimation> m_Animation;
};