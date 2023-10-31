#pragma once
#include "../Engine/Header.h"
#include "Mesh.h"

class Model;

class ModelLoader
{
private:
	static ModelLoader* pInstance;

public:
	static ModelLoader* GetInstance();

public:
	ModelLoader();
	~ModelLoader();

	bool Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName, Model* model);
	void Draw(ID3D11DeviceContext* deviceContext);

	void Close();

public:
	inline void SetCBMeshData(ComPtr<ID3D11Buffer> meshbuffer);

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene, const aiNode* node);
	vector<Texture*> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

	void processAnimation(aiAnimation* srcAnimation);
	asAnimationNode ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode);

	void SetMeshParent();

private:
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	vector<Mesh*>			m_Meshes;
	vector<Texture*>		m_Textures;
	asAnimation*			m_Animation = nullptr;
	string					m_Directory;
	HWND					m_Hwnd;
	ComPtr<ID3D11Buffer>	m_pCBMeshData;

	Mesh*					m_HeadMesh;

private:
	Model* m_pLoadModel;
};

void ModelLoader::SetCBMeshData(ComPtr<ID3D11Buffer> meshbuffer)
{
	m_pCBMeshData = meshbuffer;
}