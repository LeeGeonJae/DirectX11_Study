#pragma once

#include "../Engine/Header.h"

class Mesh;
class Material;

class Object
{
public:
	Object();
	~Object();

public:
	void Init();
	void Update();
	void Render(ID3D11DeviceContext* deviceContext);

public:
	vector<Mesh> m_Meshes;
	vector<Material> m_Materials;
};