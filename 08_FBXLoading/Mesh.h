#pragma once

#include "../Engine/Header.h"
#include "BufferStruct.h"
#include "Texture.h"
#include "enum.h"

class Mesh
{
public:
	//Mesh(ID3D11Device* device, const vector<ShaderVertex>& vertices, const vector<UINT>& indices, const vector<Texture>* texture);

public:
	void Draw(ID3D11DeviceContext* deviceContext);
	void Close();

	void SetupMesh(ID3D11Device* device);

public:
	vector<ShaderVertex> m_Vertices;
	vector<UINT> m_Indices;
	map<int, Texture> m_Textures;
	ID3D11Device* m_Device;
	CBIsValidTextureMap		m_CBIsValidTextureMap;

private:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;
};