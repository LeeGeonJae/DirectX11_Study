#include "ModelLoader.h"
#include "ImGuiMenu.h"

#include <Directxtk/WICTextureLoader.h>

ModelLoader* ModelLoader::pInstance = nullptr;

ModelLoader::ModelLoader()
	: m_Device(nullptr)
	, m_DeviceContext(nullptr)
	, m_Meshes()
	, m_Directory()
	, m_Textures()
	, m_Hwnd(nullptr)
{
	pInstance = this;
}

ModelLoader::~ModelLoader()
{
}

ModelLoader* ModelLoader::GetInstance()
{
	assert(pInstance != nullptr);
	return pInstance;
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName, Model* model)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
	m_Hwnd = hwnd;
	m_Animation = new asAnimation;

	Assimp::Importer importer;
	unsigned int importFalgs = aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded;

	const aiScene* pScene = importer.ReadFile(fileName, importFalgs);

	if (pScene == nullptr)
		return false;

	m_Directory = fileName.substr(0, fileName.find_last_of("/\\"));

	processNode(pScene->mRootNode, pScene);

	if (pScene->HasAnimations())
	{
		processAnimation(*pScene->mAnimations);
	}

	SetMeshParent();

	aiMatrix4x4 root;
	aiMatrix4x4::Translation(aiVector3D{0.f, 0.f, 0.f}, root);

	m_HeadMesh->SetVertices(root, m_Device);

	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext* deviceContext)
{
	m_HeadMesh->Draw(deviceContext, m_pCBMeshData);
}

void ModelLoader::Close()
{

}

void ModelLoader::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		m_Meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene, node));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh* ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene, const aiNode* node)
{
	// Data to fill
	Mesh* myMesh = new Mesh;
	vector<Texture> textures;

	myMesh->SetName(node->mName.C_Str());
	myMesh->SetParentName(node->mParent->mName.C_Str());
	myMesh->m_NodeMatrix = node->mTransformation;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		ShaderVertex vertex;

		if (mesh->HasPositions())
		{
			vertex.m_Position.x = mesh->mVertices[i].x;
			vertex.m_Position.y = mesh->mVertices[i].y;
			vertex.m_Position.z = mesh->mVertices[i].z;
		}

		if (mesh->mTextureCoords[0])
		{
			vertex.m_Texcoord.x = static_cast<float>(mesh->mTextureCoords[0][i].x);
			vertex.m_Texcoord.y = static_cast<float>(mesh->mTextureCoords[0][i].y);
		}

		if (mesh->HasNormals())
		{
			vertex.m_Normal.x = mesh->mNormals[i].x;
			vertex.m_Normal.y = mesh->mNormals[i].y;
			vertex.m_Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.m_Tangent.x = mesh->mTangents[i].x;
			vertex.m_Tangent.y = mesh->mTangents[i].y;
			vertex.m_Tangent.z = mesh->mTangents[i].z;

			vertex.m_BiTangetns.x = mesh->mBitangents[i].x;
			vertex.m_BiTangetns.y = mesh->mBitangents[i].y;
			vertex.m_BiTangetns.z = mesh->mBitangents[i].z;
		}

		myMesh->m_Vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			myMesh->m_Indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		if (diffuseMaps.size() > 0)
		{
			myMesh->m_Textures.insert(make_pair(static_cast<int>(TextureType::DIFFUSE), diffuseMaps[0]));
			myMesh->m_CBIsValidTextureMap.bIsValidDiffuseMap = true;
		}

		vector<Texture> NormalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normals", scene);
		if (NormalMaps.size() > 0)
		{
			myMesh->m_Textures.insert(make_pair(static_cast<int>(TextureType::NORMAL), NormalMaps[0]));
			myMesh->m_CBIsValidTextureMap.bIsValidNormalMap = true;
		}

		vector<Texture> SpecularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		if (SpecularMaps.size() > 0)
		{
			myMesh->m_Textures.insert(make_pair(static_cast<int>(TextureType::SPECULAR), SpecularMaps[0]));
			myMesh->m_CBIsValidTextureMap.bIsValidSpecularMap = true;
		}

		vector<Texture> OpacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity", scene);
		if (OpacityMaps.size() > 0)
		{
			myMesh->m_Textures.insert(make_pair(static_cast<int>(TextureType::OPACITY), OpacityMaps[0]));
			myMesh->m_CBIsValidTextureMap.bIsValidOpcityMap = true;
		}
	}

	return myMesh;
}

vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene)
{
	vector<Texture> textures;

	// ��Ƽ����(material)���� Ư�� Ÿ��(type)�� �ؽ�ó ������ŭ �ݺ��մϴ�.
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		bool skip = false;

		// �̹� �ε��� �ؽ�ó�� �����ϴ� m_Textures ���͸� ��ȸ�Ͽ� ���� �ؽ�ó�� �̹� �ε�Ǿ����� Ȯ���մϴ�.
		for (UINT j = 0; j < m_Textures.size(); j++)
		{
			if (strcmp(m_Textures[j].m_Path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(m_Textures[j]);
				skip = true;
				break;
			}
		}

		// �̹� �ε���� ���� ���ο� �ؽ�ó�� ���
		if (!skip)
		{
			HRESULT hr;
			Texture texture;

			// ���� �ؽ�ó(embedded texture)�� ����� ���
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				// ���� �ؽ�ó�� �ε��ϰ� �ش� �ؽ�ó�� texture.m_Texture�� �����մϴ�.
				texture.m_Texture = loadEmbeddedTexture(embeddedTexture);
			}
			// ���Ͽ��� �ؽ�ó�� �ε��ؾ� �ϴ� ���
			else
			{
				DirectX::TexMetadata md;
				DirectX::ScratchImage img;

				string filename = str.C_Str();
				filename.erase(filename.begin(), filename.begin() + filename.find_last_of('\\') + 1);
				filename = m_Directory + '/' + filename;
				wstring filenamews = wstring(filename.begin(), filename.end());

				hr = ::LoadFromWICFile(filenamews.c_str(), WIC_FLAGS_NONE, &md, img);
				assert(SUCCEEDED(hr));

				hr = ::CreateShaderResourceView(m_Device, img.GetImages(), img.GetImageCount(), md, &(texture.m_Texture));
				assert(SUCCEEDED(hr));
			}

			texture.m_Type = typeName;
			texture.m_Path = str.C_Str();
			textures.push_back(texture);
			m_Textures.push_back(texture);
		}
	}

	return textures;
}

// ���� �ؽ�ó(embeddedTexture)�� �ε��ϰ� ID3D11ShaderResourceView* ���·� ��ȯ�ϴ� �Լ��Դϴ�.
ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	HRESULT hr;
	ID3D11ShaderResourceView* texture = nullptr;

	// ���� �ؽ�ó�� ���̰� 0�� �ƴ� ���(��, 2D �ؽ�ó�� ���) ó���մϴ�.
	if (embeddedTexture->mHeight != 0)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;  // �ؽ�ó�� �ʺ�
		desc.Height = embeddedTexture->mHeight;  // �ؽ�ó�� ����
		desc.MipLevels = 1;  // ���� ���� ��
		desc.ArraySize = 1;  // �迭 ũ��
		desc.SampleDesc.Count = 1;  // ���ø� ��
		desc.SampleDesc.Quality = 0;  // ���ø� ǰ��
		desc.Usage = D3D11_USAGE_DEFAULT;  // ��� ��� (�⺻ ���)
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // �ؽ�ó ���� (8��Ʈ Red, Green, Blue �� 8��Ʈ Alpha ä��)
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // ���ε� �÷��� (Shader Resource�� ���ε�)
		desc.CPUAccessFlags = 0;  // CPU ������ �÷��� (����)
		desc.MiscFlags = 0;  // ��Ÿ �÷��� (����)

		// �ؽ�ó �����͸� �����ϴ� ���긮�ҽ� ������ ����ü�� �����ϰ� �ʱ�ȭ�մϴ�.
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;  // �ؽ�ó �������� ������
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;  // �ؽ�ó�� �� �� ũ�� (4����Ʈ = RGBA 4��)
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;  // ��ü �ؽ�ó ������ ũ��

		ID3D11Texture2D* texture2D = nullptr;
		hr = m_Device->CreateTexture2D(&desc, &subresourceData, &texture2D);
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateShaderResourceView(texture2D, nullptr, &texture);
		assert(SUCCEEDED(hr));

		return texture;
	}

	const size_t size = embeddedTexture->mWidth;

	hr = DirectX::CreateWICTextureFromMemory(m_Device, m_DeviceContext, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);

	return texture;
}

// �ִϸ��̼� ��������
void ModelLoader::processAnimation(aiAnimation* srcAnimation)
{
	m_Animation->m_Name = srcAnimation->mName.C_Str();
	m_Animation->m_FrameRate = static_cast<float>(srcAnimation->mTicksPerSecond);
	m_Animation->m_FrameCount = static_cast<unsigned int>(srcAnimation->mDuration + 1);
	
	// ��� ������ŭ �ִϸ��̼� ��������
	for (int i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* node = srcAnimation->mChannels[i];

		// �ִϸ��̼� ��� ������ �Ľ�
		asAnimationNode nodeanimation = ParseAnimationNode(m_Animation, node);
		m_Animation->m_Nodes.push_back(nodeanimation);
	}
}

// �ִϸ��̼� ������ ��������
asAnimationNode ModelLoader::ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode)
{
	asAnimationNode node;
	node.m_Duration = animation->m_Duration;
	node.m_FrameCount = animation->m_FrameCount;
	node.m_FrameRate = animation->m_FrameRate;
	node.m_Name = srcNode->mNodeName.C_Str();

	int keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	for (int i = 0; i < keyCount; i++)
	{
		asKeyFrameData frameData;

		bool found = false;
		unsigned int t = node.m_KeyFrame.size();

		// Position
		if (fabsf((float)srcNode->mPositionKeys[i].mTime - (float)t) <= 0.0001f)		// �Ǽ��� ���밪�� ��ȯ
		{
			aiVectorKey key = srcNode->mPositionKeys[i];
			frameData.m_Time = (float)key.mTime;

			frameData.m_Transtation.x = key.mValue.x;
			frameData.m_Transtation.y = key.mValue.y;
			frameData.m_Transtation.z = key.mValue.z;

			found = true;
		}

		// Rotation
		if (fabsf((float)srcNode->mRotationKeys[i].mTime - (float)t) <= 0.0001f)
		{
			aiQuatKey key = srcNode->mRotationKeys[i];
			frameData.m_Time = (float)key.mTime;

			frameData.m_Rotation.x = key.mValue.x;
			frameData.m_Rotation.y = key.mValue.y;
			frameData.m_Rotation.z = key.mValue.z;
			frameData.m_Rotation.w = key.mValue.w;

			found = true;
		}

		// Scale
		if (fabsf((float)srcNode->mScalingKeys[i].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[i];
			frameData.m_Time = (float)key.mTime;

			frameData.m_Scale.x = key.mValue.x;
			frameData.m_Scale.y = key.mValue.y;
			frameData.m_Scale.z = key.mValue.z;
		}

		if (found == true)
			node.m_KeyFrame.push_back(frameData);
	}

	// Keyframe �÷��ֱ�
	if (node.m_KeyFrame.size() < animation->m_FrameCount)
	{
		unsigned int count = animation->m_FrameCount - node.m_KeyFrame.size();
		asKeyFrameData keyFrame = node.m_KeyFrame.back();

		for (int i = 0; i < count; i++)
		{
			node.m_KeyFrame.push_back(keyFrame);
		}
	}

	return node;
}

void ModelLoader::SetMeshParent()
{
	// ��¥ ���� �ڵ�.. �����ؾ���.. node class ���� ���� ����
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		// �θ� ������ �ֻ��� ���� ����
		if (m_Meshes[i]->GetParentName() == "RootNode")
		{
			m_HeadMesh = m_Meshes[i];

			XMMATRIX mTranslate = XMMatrixTranslation(ImGuiMenu::CubePosition.x, ImGuiMenu::CubePosition.y, ImGuiMenu::CubePosition.z);
			m_HeadMesh->m_Local = mTranslate;
			continue;
		}

		// �޽��� �θ� �̸��� ã�� ���� ã���� �θ� ��� �� �ڽ� ���
		for (int j = 0; j < m_Meshes.size(); j++)
		{
			if (m_Meshes[i]->GetParentName() == m_Meshes[j]->GetName())
			{
				m_Meshes[i]->SetParent(m_Meshes[j]);
				break;
			}
		}
	}

	for (int i = 0; i < m_Animation->m_Nodes.size(); i++)
	{
		for (int j = 0; j < m_Meshes.size(); j++)
		{
			if (m_Animation->m_Nodes[i].m_Name == m_Meshes[j]->GetName())
			{
				m_Meshes[j]->SetAnimationNode(&m_Animation->m_Nodes[i]);
			}
		}
	}
}