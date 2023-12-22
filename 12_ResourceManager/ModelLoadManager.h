#pragma once
#include "../Engine/Header.h"

#include "Animation.h"
#include "Struct.h"

class Model;
class Node;
class Mesh;
class StaticMesh;
class SkeletalMesh;

class ModelLoadManager
{
private:
	ModelLoadManager() {}
public:
	static ModelLoadManager* GetInstance()
	{
		static ModelLoadManager instance;
		return &instance;
	}

public:
	void Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);

	shared_ptr<Model> Load(string fileName);

private:
	void processNode(const aiNode* headNode, const aiScene* scene, shared_ptr<Node> headnode);			// 노드
	shared_ptr<StaticMesh> processStaticMesh(const aiMesh* aimesh, const aiScene* scene);				// 메시
	shared_ptr<SkeletalMesh> processSkeletalMesh(const aiMesh* aimesh, const aiScene* scene);				// 메시

	// 애니메이션
	void processAnimation(aiAnimation* srcAnimation);
	shared_ptr<AnimationNode> ParseAnimationNode(shared_ptr<Animation> animation, aiNodeAnim* srcNode);

	// 텍스쳐 파일 가져오기
	vector<shared_ptr<Texture>> loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene);
	ComPtr<ID3D11ShaderResourceView> loadEmbeddedTexture(const aiTexture* embeddedTexture); // 내장된 텍스쳐 파일 가져오기

private:
	ComPtr<ID3D11Device>			m_Device;
	ComPtr<ID3D11DeviceContext>	m_DeviceContext;
	string					m_Directory;
	ComPtr<ID3D11Buffer>	m_pCBMeshData;

private:
	vector<shared_ptr<Texture>>		m_Textures;
	shared_ptr<Model> m_pLoadModel;
};