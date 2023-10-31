#pragma once
#include "../Engine/Header.h"
#include "Mesh.h"

class Model;
class Node;

class ModelLoadManager
{
private:
	static ModelLoadManager* pInstance;

public:
	static ModelLoadManager* GetInstance();

public:
	ModelLoadManager();
	~ModelLoadManager();

	bool Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName, Model* model);

private:
	void processNode(const aiNode* headNode, const aiScene* scene, Node* headnode);		// 노드
	Mesh* processMesh(const aiMesh* aimesh, const aiScene* scene);						// 메시

	// 애니메이션
	void processAnimation(aiAnimation* srcAnimation);
	asAnimationNode ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode);

	// 텍스쳐 파일 가져오기
	vector<Texture*> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture); // 내장된 텍스쳐 파일 가져오기

private:
	ID3D11Device* m_Device;
	ID3D11DeviceContext* m_DeviceContext;
	string					m_Directory;
	HWND					m_Hwnd;
	ComPtr<ID3D11Buffer>	m_pCBMeshData;

private:
	vector<Texture*>		m_Textures;
	Model* m_pLoadModel;
};