#include "ModelLoadManager.h"
#include "ImGuiMenu.h"
#include "Model.h"
#include "Node.h"

#include <Directxtk/WICTextureLoader.h>

ModelLoadManager* ModelLoadManager::pInstance = nullptr;

ModelLoadManager::ModelLoadManager()
	: m_Device(nullptr)
	, m_DeviceContext(nullptr)
	, m_Directory()
	, m_Hwnd(nullptr)
	, m_pLoadModel(nullptr)
{
	pInstance = this;
}

ModelLoadManager::~ModelLoadManager()
{
}

ModelLoadManager* ModelLoadManager::GetInstance()
{
	assert(pInstance != nullptr);
	return pInstance;
}

bool ModelLoadManager::Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName, Model* model)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
	m_Hwnd = hwnd;
	m_pLoadModel = model;
	m_Directory = fileName.substr(0, fileName.find_last_of("/\\"));

	// ������
	{
		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0); // $assimp_fbx$ ��� ��������	

		unsigned int importFalgs = 
			aiProcess_Triangulate |			// �ﰢ������ ��ȯ
			aiProcess_GenNormals |			// �븻 ����
			aiProcess_GenUVCoords |			// UV ����
			aiProcess_CalcTangentSpace |	// ź��Ʈ ����
			aiProcess_LimitBoneWeights |	// ���� ������ �޴� ������ �ִ� ������ 4���� ����
			aiProcess_ConvertToLeftHanded;	// �޼� ��ǥ��� ��ȯ

		const aiScene* pScene = importer.ReadFile(fileName, importFalgs);

		if (pScene == nullptr)
			return false;

		processNode(pScene->mRootNode, pScene, nullptr);

		// �ִϸ��̼� �ε�
		if (pScene->HasAnimations())
		{
			processAnimation(*pScene->mAnimations);
		}
	}

	return true;
}

void ModelLoadManager::processNode(const aiNode* aiNode, const aiScene* scene, Node* headnode)
{
	Node* tempNode = new Node;
	tempNode->SetName(aiNode->mName.C_Str());
	tempNode->SetTransform(aiNode->mTransformation);

	// ��尡 nullptr�� �ƴϸ� �θ� ���� ����
	// ��尡 nullptr�̸� ��Ʈ ���� ����
	if (headnode != nullptr)
	{
		tempNode->SetParentNode(headnode);
	}
	else
	{
		m_pLoadModel->SetHeadNode(tempNode);
	}

	// �޽� �ε�
	for (int i = 0; i < aiNode->mNumMeshes; i++)
	{
		tempNode->SetMesh(processMesh(scene->mMeshes[aiNode->mMeshes[i]], scene, tempNode));
	}

	// �ڽ� ��� ����
	for (int i = 0; i < aiNode->mNumChildren; i++)
	{
		processNode(aiNode->mChildren[i], scene, tempNode);
	}

	m_pLoadModel->SetNode(tempNode);
}

Mesh* ModelLoadManager::processMesh(const aiMesh* aimesh, const aiScene* scene, Node* node)
{
	// Data to fill
	Mesh* myMesh = new Mesh;
	myMesh->SetName(aimesh->mName.C_Str());

	// �޽� ���ؽ� ��������
	for (UINT i = 0; i < aimesh->mNumVertices; i++)
	{
		BoneWeightVertex vertex;

		if (aimesh->HasPositions())
		{
			vertex.m_Position.x = aimesh->mVertices[i].x;
			vertex.m_Position.y = aimesh->mVertices[i].y;
			vertex.m_Position.z = aimesh->mVertices[i].z;
		}

		if (aimesh->mTextureCoords[0])
		{
			vertex.m_Texcoord.x = static_cast<float>(aimesh->mTextureCoords[0][i].x);
			vertex.m_Texcoord.y = static_cast<float>(aimesh->mTextureCoords[0][i].y);
		}

		if (aimesh->HasNormals())
		{
			vertex.m_Normal.x = aimesh->mNormals[i].x;
			vertex.m_Normal.y = aimesh->mNormals[i].y;
			vertex.m_Normal.z = aimesh->mNormals[i].z;
		}

		if (aimesh->HasTangentsAndBitangents())
		{
			vertex.m_Tangent.x = aimesh->mTangents[i].x;
			vertex.m_Tangent.y = aimesh->mTangents[i].y;
			vertex.m_Tangent.z = aimesh->mTangents[i].z;

			vertex.m_BiTangetns.x = aimesh->mBitangents[i].x;
			vertex.m_BiTangetns.y = aimesh->mBitangents[i].y;
			vertex.m_BiTangetns.z = aimesh->mBitangents[i].z;
		}

		myMesh->m_BoneWeightVertices.push_back(vertex);
	}

	// �޽� �ε��� ��������
	for (UINT i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			myMesh->m_Indices.push_back(face.mIndices[j]);
		}
	}
	
	// �޽� �� ��������
	if (aimesh->HasBones())
	{
		node->GetIsValidTextureMap()->bIsValidBone = true;

		UINT meshBoneCount = aimesh->mNumBones;
		UINT boneIndexCounter = 0;
		map<string, int> BoneMapping;

		for (int i = 0; i < m_pLoadModel->GetBones().size(); i++)
		{
			BoneMapping.insert(make_pair(m_pLoadModel->GetBones()[i]->m_Name, i));
		}

		m_pLoadModel->GetBones().resize(meshBoneCount);

		// ���� ������ŭ �� ����
		for (UINT i = 0; i < meshBoneCount; i++)
		{
			aiBone* aibone = aimesh->mBones[i];
			UINT boneIndex = 0;
			
			if (BoneMapping.find(aibone->mName.C_Str()) == BoneMapping.end())
			{
				Bone* bone = new Bone;

				boneIndex = boneIndexCounter;
				boneIndexCounter++;
				bone->m_Name = aibone->mName.C_Str();
				bone->m_NumWeights = aibone->mNumWeights;

				bone->m_OffsetMatrix = DirectX::SimpleMath::Matrix(
					aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.d1,
					aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.d2,
					aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.d3,
					aibone->mOffsetMatrix.a4, aibone->mOffsetMatrix.b4, aibone->mOffsetMatrix.c4, aibone->mOffsetMatrix.d4
				);

				for (auto node : m_pLoadModel->GetNode())
				{
					if (node->GetName() == bone->m_Name)
					{
						node->SetBone(bone);
						bone->m_Owner = node;
					}
				}

				m_pLoadModel->SetBone(bone);
			}
			else
			{
				boneIndex = BoneMapping[aibone->mName.C_Str()];
			}

			for (UINT j = 0; j < aibone->mNumWeights; j++)
			{
				UINT vertexID = aibone->mWeights[j].mVertexId;
				float weight = aibone->mWeights[j].mWeight;

				myMesh->m_BoneWeightVertices[vertexID].AddBoneData(boneIndex, weight);
			}
		}
	}

	// �޽� ���͸��� ��������
	if (aimesh->mMaterialIndex >= 0)
	{
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		Material* material = new Material;

		aiColor3D basecolor(0.f, 0.f, 0.f);
		aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, basecolor);
		material->basecolor = Vector3(basecolor.r, basecolor.g, basecolor.b);

		vector<Texture*> diffuseMaps = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		if (diffuseMaps.size() > 0)
		{
			material->m_Textures.insert(make_pair(static_cast<int>(TextureType::DIFFUSE), diffuseMaps[0]));
			node->GetIsValidTextureMap()->bIsValidDiffuseMap = true;
		}

		vector<Texture*> NormalMaps = loadMaterialTextures(aimaterial, aiTextureType_NORMALS, "texture_normals", scene);
		if (NormalMaps.size() > 0)
		{
			material->m_Textures.insert(make_pair(static_cast<int>(TextureType::NORMAL), NormalMaps[0]));
			node->GetIsValidTextureMap()->bIsValidNormalMap = true;
		}

		vector<Texture*> SpecularMaps = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR, "texture_specular", scene);
		if (SpecularMaps.size() > 0)
		{
			material->m_Textures.insert(make_pair(static_cast<int>(TextureType::SPECULAR), SpecularMaps[0]));
			node->GetIsValidTextureMap()->bIsValidSpecularMap = true;
		}

		vector<Texture*> OpacityMaps = loadMaterialTextures(aimaterial, aiTextureType_OPACITY, "texture_opacity", scene);
		if (OpacityMaps.size() > 0)
		{
			material->m_Textures.insert(make_pair(static_cast<int>(TextureType::OPACITY), OpacityMaps[0]));
			node->GetIsValidTextureMap()->bIsValidOpcityMap = true;
		}

		node->SetMaterial(material);
	}

	return myMesh;
}

void ModelLoadManager::processAnimation(aiAnimation* srcAnimation)
{
	// �ִϸ��̼� �����ϱ�
	asAnimation* animation = new asAnimation;

	animation->m_Name = srcAnimation->mName.C_Str();
	animation->m_FrameRate = static_cast<float>(srcAnimation->mTicksPerSecond);
	animation->m_FrameCount = static_cast<unsigned int>(srcAnimation->mDuration + 1);

	// ��� ������ŭ �ִϸ��̼� ��������
	for (int i = 0; i < srcAnimation->mNumChannels; i++)
	{
		aiNodeAnim* node = srcAnimation->mChannels[i];

		// �ִϸ��̼� ��� ������ �Ľ�
		asAnimationNode* nodeanimation = ParseAnimationNode(animation, node);
		animation->m_Nodes.push_back(nodeanimation);

		// �̸��� �´� ��带 ã�Ƽ� �ش� ��忡 �ִϸ��̼� ���
		for (int i = 0; i < m_pLoadModel->GetNode().size(); i++)
		{
			if (m_pLoadModel->GetNode()[i]->GetName() == nodeanimation->m_Name)
				m_pLoadModel->GetNode()[i]->SetAnimation(nodeanimation);
		}
	}

	m_pLoadModel->SetAnimation(animation);
}

asAnimationNode* ModelLoadManager::ParseAnimationNode(asAnimation* animation, aiNodeAnim* srcNode)
{
	// �ִϸ��̼� ��� �����ϱ�
	asAnimationNode* node = new asAnimationNode;
	node->m_Duration = animation->m_Duration;
	node->m_FrameCount = animation->m_FrameCount;
	node->m_FrameRate = animation->m_FrameRate;
	node->m_Name = srcNode->mNodeName.C_Str();

	int keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	for (int i = 0; i < keyCount; i++)
	{
		asKeyFrameData frameData;

		bool found = false;
		unsigned int t = node->m_KeyFrame.size();

		// Position
		{
			aiVectorKey key = srcNode->mPositionKeys[i];
			frameData.m_Time = (float)key.mTime;

			frameData.m_Transtation.x = key.mValue.x;
			frameData.m_Transtation.y = key.mValue.y;
			frameData.m_Transtation.z = key.mValue.z;

			found = true;
		}

		// Rotation
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
		//if (fabsf((float)srcNode->mScalingKeys[i].mTime - (float)t) <= 0.0001f)
		{
			aiVectorKey key = srcNode->mScalingKeys[i];
			frameData.m_Time = (float)key.mTime;

			frameData.m_Scale.x = key.mValue.x;
			frameData.m_Scale.y = key.mValue.y;
			frameData.m_Scale.z = key.mValue.z;
		}

		if (found == true)
			node->m_KeyFrame.push_back(frameData);
	}

	// Keyframe �÷��ֱ�
	if (node->m_KeyFrame.size() < animation->m_FrameCount)
	{
		unsigned int count = animation->m_FrameCount - node->m_KeyFrame.size();
		asKeyFrameData keyFrame = node->m_KeyFrame.back();

		for (unsigned int i = 0; i < count; i++)
		{
			node->m_KeyFrame.push_back(keyFrame);
		}
	}

	return node;
}

vector<Texture*> ModelLoadManager::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene)
{
	vector<Texture*> textures;

	// ��Ƽ����(material)���� Ư�� Ÿ��(type)�� �ؽ�ó ������ŭ �ݺ��մϴ�.
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		bool skip = false;

		// �̹� �ε��� �ؽ�ó�� �����ϴ� m_Textures ���͸� ��ȸ�Ͽ� ���� �ؽ�ó�� �̹� �ε�Ǿ����� Ȯ���մϴ�.
		for (UINT j = 0; j < m_Textures.size(); j++)
		{
			if (strcmp(m_Textures[j]->m_Path.c_str(), str.C_Str()) == 0)
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
			Texture* texture = new Texture;

			// ���� �ؽ�ó(embedded texture)�� ����� ���
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				// ���� �ؽ�ó�� �ε��ϰ� �ش� �ؽ�ó�� texture.m_Texture�� �����մϴ�.
				texture->m_Texture = loadEmbeddedTexture(embeddedTexture);
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

				hr = ::CreateShaderResourceView(m_Device, img.GetImages(), img.GetImageCount(), md, &(texture->m_Texture));
				assert(SUCCEEDED(hr));
			}

			texture->m_Type = typeName;
			texture->m_Path = str.C_Str();
			textures.push_back(texture);
			m_Textures.push_back(texture);
		}
	}

	return textures;
}

// ���� �ؽ�ó(embeddedTexture)�� �ε��ϰ� ID3D11ShaderResourceView* ���·� ��ȯ�ϴ� �Լ��Դϴ�.
ID3D11ShaderResourceView* ModelLoadManager::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	HRESULT hr;
	ID3D11ShaderResourceView* texture = nullptr;

	// ���� �ؽ�ó�� ���̰� 0�� �ƴ� ���(��, 2D �ؽ�ó�� ���) ó���մϴ�.
	if (embeddedTexture->mHeight != 0)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;			// �ؽ�ó�� �ʺ�
		desc.Height = embeddedTexture->mHeight;			// �ؽ�ó�� ����
		desc.MipLevels = 1;								// ���� ���� ��
		desc.ArraySize = 1;								// �迭 ũ��
		desc.SampleDesc.Count = 1;						// ���ø� ��
		desc.SampleDesc.Quality = 0;					// ���ø� ǰ��
		desc.Usage = D3D11_USAGE_DEFAULT;				// ��� ��� (�⺻ ���)
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;		// �ؽ�ó ���� (8��Ʈ Red, Green, Blue �� 8��Ʈ Alpha ä��)
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// ���ε� �÷��� (Shader Resource�� ���ε�)
		desc.CPUAccessFlags = 0;						// CPU ������ �÷��� (����)
		desc.MiscFlags = 0;								// ��Ÿ �÷��� (����)

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