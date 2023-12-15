#pragma once
#include "../Engine/pch.h"
#include "Mesh.h"

class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	bool Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName);
	void Draw(ID3D11DeviceContext* deviceContext);

	void Close();

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	vector<Mesh>			m_Meshes;
	string					m_Directory;
	vector<Texture>			m_Textures;
	HWND					m_Hwnd;
};

