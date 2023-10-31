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
	void processNode(const aiNode* headNode, const aiScene* scene, Node* headnode);		// ���
	Mesh* processMesh(const aiMesh* aimesh, const aiScene* scene);						// �޽�

	// �ִϸ��̼�
	void processAnimation(aiAnimation* srcAnimation);
	asAnimationNode ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode);

	// �ؽ��� ���� ��������
	vector<Texture*> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* loadEmbeddedTexture(const aiTexture* embeddedTexture); // ����� �ؽ��� ���� ��������

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