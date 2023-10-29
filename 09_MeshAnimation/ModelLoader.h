#pragma once
#include "../Engine/Header.h"
#include "Mesh.h"

// 진짜 억지 코드.. 수정해야함.. node class 만들어서 수정 예정
class ModelLoader
{
public:
	ModelLoader();
	~ModelLoader();

	bool Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName);
	void Draw(ID3D11DeviceContext* deviceContext);

	void Close();

public:
	inline void SetCBMeshData(ComPtr<ID3D11Buffer> meshbuffer);

private:
	void processNode(aiNode* node, const aiScene* scene);
	Mesh* processMesh(aiMesh* mesh, const aiScene* scene, const string name, const string parentname);
	vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture);

	void processAnimation(aiAnimation* srcAnimation);
	asAnimationNode ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode);

	void SetMeshParent();

private:
	ID3D11Device*			m_Device;
	ID3D11DeviceContext*	m_DeviceContext;
	vector<Mesh*>			m_Meshes;
	vector<Texture>			m_Textures;
	asAnimation*			m_Animation = nullptr;
	string					m_Directory;
	HWND					m_Hwnd;
	ComPtr<ID3D11Buffer>	m_pCBMeshData;

	Mesh*					m_HeadMesh;
};

void ModelLoader::SetCBMeshData(ComPtr<ID3D11Buffer> meshbuffer)
{
	m_pCBMeshData = meshbuffer;
}