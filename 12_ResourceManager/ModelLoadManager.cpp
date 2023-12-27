#include "ModelLoadManager.h"
#include "ImGuiMenu.h"

#include "ResourceManager.h"

#include "Model.h"
#include "Mesh.h"
#include "StaticMesh.h"
#include "SkeletalMesh.h"
#include "Node.h"
#include "Material.h"
#include "Animation.h"

#include <Directxtk/WICTextureLoader.h>

void ModelLoadManager::Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
}

shared_ptr<Model> ModelLoadManager::Load(string fileName)
{
	m_Directory = fileName.substr(0, fileName.find_last_of("/\\"));
	m_pLoadModel = make_shared<Model>();
	m_pLoadModel->SetName(fileName);

	// 임포터
	{
		Assimp::Importer importer;
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0); // $assimp_fbx$ 노드 생성안함	

		unsigned int importFalgs =
			aiProcess_Triangulate |			// 삼각형으로 변환
			aiProcess_GenNormals |			// 노말 생성
			aiProcess_GenUVCoords |			// UV 생성
			aiProcess_CalcTangentSpace |	// 탄젠트 생성
			aiProcess_LimitBoneWeights |	// 본의 영향을 받는 장점의 최대 갯수를 4개로 제한
			aiProcess_GenBoundingBoxes |	// AABB바운딩 박스 생성 
			aiProcess_ConvertToLeftHanded;	// 왼손 좌표계로 변환

		const aiScene* pScene = importer.ReadFile(fileName, importFalgs);

		if (pScene == nullptr)
			return nullptr;

		processNode(pScene->mRootNode, pScene, nullptr);

		// 애니메이션 로드
		if (pScene->HasAnimations())
		{
			processAnimation(*pScene->mAnimations);
		}
	}

	return m_pLoadModel;
}

void ModelLoadManager::processNode(const aiNode* aiNode, const aiScene* scene, shared_ptr<Node> headnode)
{
	shared_ptr<Node> tempNode = make_shared<Node>();
	shared_ptr<NodeData> nodeData = make_shared<NodeData>();
	tempNode->SetName(aiNode->mName.C_Str());
	nodeData->m_Name = tempNode->GetName();
	tempNode->SetTransform(aiNode->mTransformation);

	// 노드가 nullptr이 아니면 부모 노드로 저장
	// 노드가 nullptr이면 루트 노드로 저장
	if (headnode != nullptr)
	{
		tempNode->SetParentNode(headnode);
		nodeData->m_Parent = headnode->GetName();
	}
	else
	{
		m_pLoadModel->SetHeadNode(tempNode);
	}

	// 메시 로드
	for (int i = 0; i < aiNode->mNumMeshes; i++)
	{
		auto aimesh = scene->mMeshes[aiNode->mMeshes[i]];

		if (scene->mMeshes[0]->HasBones())
		{
			tempNode->SetMesh(processSkeletalMesh(aimesh, scene, nodeData));
		}
		else
		{
			tempNode->SetMesh(processStaticMesh(aimesh, scene, nodeData));
		}

		nodeData->m_MeshName = aimesh->mName.C_Str();
	}

	// 자식 노드 실행
	for (int i = 0; i < aiNode->mNumChildren; i++)
	{
		processNode(aiNode->mChildren[i], scene, tempNode);
	}

	ResourceManager::GetInstance()->SetNodeData(m_pLoadModel->GetName(), nodeData);
	m_pLoadModel->SetNode(tempNode);
}

shared_ptr<StaticMesh> ModelLoadManager::processStaticMesh(const aiMesh* aimesh, const aiScene* scene, shared_ptr<NodeData> nodeData)
{
	shared_ptr<StaticMesh> myMesh;

	myMesh = ResourceManager::GetInstance()->FindStaticMesh(aimesh->mName.C_Str());

	if (myMesh != nullptr)
		return myMesh;
	else
		myMesh = make_shared<StaticMesh>();


	myMesh->SetName(aimesh->mName.C_Str());

	// 메시 버텍스 가져오기
	for (UINT i = 0; i < aimesh->mNumVertices; i++)
	{
		Vertex vertex;

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

		myMesh->m_Vertices.push_back(vertex);
	}

	// 메시 인덱스 가져오기
	for (UINT i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			myMesh->m_Indices.push_back(face.mIndices[j]);
		}
	}

	// 메시 머터리얼 가져오기
	if (aimesh->mMaterialIndex >= 0)
	{
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		// 리소스 매니저에서 찾아보기
		shared_ptr<Material> material = ResourceManager::GetInstance()->FindMaterial(aimaterial->GetName().C_Str());

		// 리소스 매니저에 없으면 머터리얼 로드
		if (material == nullptr)
		{
			material = processMaterialsData(aimesh, scene, myMesh);

			ResourceManager::GetInstance()->SetMaterial(material->m_Name, material);
			nodeData->m_MaterialName = material->m_Name;
		}

		myMesh->SetMaterial(material);
	}

	ResourceManager::GetInstance()->SetStaticMesh(myMesh->GetName(), myMesh);
	return myMesh;
}

shared_ptr<SkeletalMesh> ModelLoadManager::processSkeletalMesh(const aiMesh* aimesh, const aiScene* scene, shared_ptr<NodeData> nodeData)
{
	shared_ptr<SkeletalMesh> myMesh;

	myMesh = ResourceManager::GetInstance()->FindSkeletalMesh(aimesh->mName.C_Str());

	if (myMesh != nullptr)
		return myMesh;
	else
		myMesh = make_shared<SkeletalMesh>();
	

	myMesh->SetName(aimesh->mName.C_Str());

	// 메시 버텍스 가져오기
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

	// 메시 인덱스 가져오기
	for (UINT i = 0; i < aimesh->mNumFaces; i++)
	{
		aiFace face = aimesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			myMesh->m_Indices.push_back(face.mIndices[j]);
		}
	}

	// 메시 본 가져오기
	if (aimesh->HasBones())
	{
		myMesh->GetIsValidTextureMap()->bIsValidBone = true;

		UINT meshBoneCount = aimesh->mNumBones;
		UINT boneIndexCounter = 0;
		map<string, int> BoneMapping;

		for (int i = 0; i < m_pLoadModel->GetBones().size(); i++)
		{
			BoneMapping.insert(make_pair(m_pLoadModel->GetBones()[i]->m_Name, i));
		}

		m_pLoadModel->GetBones().resize(meshBoneCount);

		// 본의 갯수만큼 본 생성
		for (UINT i = 0; i < meshBoneCount; i++)
		{
			aiBone* aibone = aimesh->mBones[i];
			UINT boneIndex = 0;

			if (BoneMapping.find(aibone->mName.C_Str()) == BoneMapping.end())
			{
				shared_ptr<Bone> bone = make_shared<Bone>();

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
				ResourceManager::GetInstance()->SetSkeletal(m_pLoadModel->GetName(), bone);
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

	// 메시 머터리얼 가져오기
	if (aimesh->mMaterialIndex >= 0)
	{
		aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

		// 리소스 매니저에서 찾아보기
		shared_ptr<Material> material = ResourceManager::GetInstance()->FindMaterial(aimaterial->GetName().C_Str());

		// 리소스 매니저에 없으면 머터리얼 로드
		if (material == nullptr)
		{
			material = processMaterialsData(aimesh, scene, myMesh);

			ResourceManager::GetInstance()->SetMaterial(material->m_Name, material);
			nodeData->m_MaterialName = material->m_Name;
		}

		myMesh->SetMaterial(material);
	}

	ResourceManager::GetInstance()->SetSkeletalMesh(myMesh->GetName(), myMesh);
	return myMesh;
}

shared_ptr<Material> ModelLoadManager::processMaterialsData(const aiMesh* aimesh, const aiScene* scene, shared_ptr<Mesh> mesh)
{
	aiMaterial* aimaterial = scene->mMaterials[aimesh->mMaterialIndex];

	shared_ptr<Material> material = make_shared<Material>();
	material->m_Name = aimaterial->GetName().C_Str();

	aiColor3D color(0.f, 0.f, 0.f);
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	material->m_BaseColor = Vector3(color.r, color.g, color.b);
	aimaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color);
	material->m_EmissiveColor = Vector3(color.r, color.g, color.b);

	vector<shared_ptr<Texture>> diffuseMaps = loadMaterialTextures(aimaterial, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	if (diffuseMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::DIFFUSE), diffuseMaps[0]));
	}

	vector<shared_ptr<Texture>> NormalMaps = loadMaterialTextures(aimaterial, aiTextureType_NORMALS, "texture_normals", scene);
	if (NormalMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::NORMAL), NormalMaps[0]));
	}

	vector<shared_ptr<Texture>> SpecularMaps = loadMaterialTextures(aimaterial, aiTextureType_SPECULAR, "texture_specular", scene);
	if (SpecularMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::SPECULAR), SpecularMaps[0]));
	}

	vector<shared_ptr<Texture>> OpacityMaps = loadMaterialTextures(aimaterial, aiTextureType_OPACITY, "texture_opacity", scene);
	if (OpacityMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::OPACITY), OpacityMaps[0]));
	}

	vector<shared_ptr<Texture>> EmissiveMaps = loadMaterialTextures(aimaterial, aiTextureType_EMISSIVE, "texture_emissive", scene);
	if (EmissiveMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::EMISSIVE), EmissiveMaps[0]));
	}

	vector<shared_ptr<Texture>> MetalnessMap = loadMaterialTextures(aimaterial, aiTextureType_METALNESS, "texture_metalness", scene);
	if (MetalnessMap.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::METALNESS), MetalnessMap[0]));
	}

	vector<shared_ptr<Texture>> RoughnessMaps = loadMaterialTextures(aimaterial, aiTextureType_SHININESS, "texture_roughness", scene);
	if (RoughnessMaps.size() > 0)
	{
		material->m_Textures.insert(make_pair(static_cast<int>(TextureType::ROUGHNESS), RoughnessMaps[0]));
	}

	return material;
}

void ModelLoadManager::processAnimation(aiAnimation* srcAnimation)
{
	// 애니메이션 세팅하기
	shared_ptr<Animation> animation = ResourceManager::GetInstance()->FindAnimation(srcAnimation->mName.C_Str());

	if (animation == nullptr)
	{
		animation = make_shared<Animation>();

		animation->m_Name = srcAnimation->mName.C_Str();
		animation->m_FrameRate = static_cast<float>(srcAnimation->mTicksPerSecond);
		animation->m_FrameCount = static_cast<unsigned int>(srcAnimation->mDuration + 1);

		// 노드 개수만큼 애니메이션 가져오기
		for (int i = 0; i < srcAnimation->mNumChannels; i++)
		{
			aiNodeAnim* node = srcAnimation->mChannels[i];

			// 애니메이션 노드 데이터 파싱
			shared_ptr<AnimationNode> nodeanimation = ParseAnimationNode(animation, node);
			animation->m_Nodes.push_back(nodeanimation);

			// 이름이 맞는 노드를 찾아서 해당 노드에 애니메이션 등록
			for (int i = 0; i < m_pLoadModel->GetNode().size(); i++)
			{
				if (m_pLoadModel->GetNode()[i]->GetName() == nodeanimation->m_Name)
					m_pLoadModel->GetNode()[i]->SetAnimation(nodeanimation);
			}
		}
	}

	ResourceManager::GetInstance()->SetAnimation(m_pLoadModel->GetName(), animation);
	m_pLoadModel->SetAnimation(animation);
}

shared_ptr<AnimationNode> ModelLoadManager::ParseAnimationNode(shared_ptr<Animation> animation, aiNodeAnim* srcNode)
{
	// 애니메이션 노드 세팅하기
	shared_ptr<AnimationNode> node = make_shared<AnimationNode>();

	node->m_Animation = animation;
	node->m_Name = srcNode->mNodeName.C_Str();
	node->m_FrameRate = animation->m_FrameRate;
	node->m_FrameCount = animation->m_FrameCount;

	int keyCount = max(max(srcNode->mNumPositionKeys, srcNode->mNumScalingKeys), srcNode->mNumRotationKeys);

	for (int i = 0; i < keyCount; i++)
	{
		KeyFrameData frameData;

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

	// Keyframe 늘려주기
	if (node->m_KeyFrame.size() < animation->m_FrameCount)
	{
		unsigned int count = animation->m_FrameCount - node->m_KeyFrame.size();
		KeyFrameData keyFrame = node->m_KeyFrame.back();

		for (unsigned int i = 0; i < count; i++)
		{
			node->m_KeyFrame.push_back(keyFrame);
		}
	}

	return node;
}

vector<shared_ptr<Texture>> ModelLoadManager::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene)
{
	vector<shared_ptr<Texture>> textures;

	// 머티리얼(material)에서 특정 타입(type)의 텍스처 개수만큼 반복합니다.
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		bool skip = false;

		// 이미 로드한 텍스처를 저장하는 m_Textures 벡터를 순회하여 현재 텍스처가 이미 로드되었는지 확인합니다.
		for (UINT j = 0; j < m_Textures.size(); j++)
		{
			if (strcmp(m_Textures[j]->m_Path.c_str(), str.C_Str()) == 0)
			{
				textures.push_back(m_Textures[j]);
				skip = true;
				break;
			}
		}

		// 이미 로드되지 않은 새로운 텍스처인 경우
		if (!skip)
		{
			HRESULT hr;
			shared_ptr<Texture> texture = make_shared<Texture>();

			// 내장 텍스처(embedded texture)를 사용할 경우
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				// 내장 텍스처를 로드하고 해당 텍스처를 texture.m_Texture에 저장합니다.
				texture->m_Texture = loadEmbeddedTexture(embeddedTexture);
			}
			// 파일에서 텍스처를 로드해야 하는 경우
			else
			{
				DirectX::TexMetadata md;
				DirectX::ScratchImage img;

				string filename = str.C_Str();
				if (filename.find('\\') != string::npos)
					filename.erase(filename.begin(), filename.begin() + filename.find_last_of('\\') + 1);
				filename = m_Directory + '/' + filename;
				wstring filenamews = wstring(filename.begin(), filename.end());

				if (filename.find(".tga") != string::npos)
				{
					hr = ::LoadFromTGAFile(filenamews.c_str(), TGA_FLAGS_NONE, &md, img);
					assert(SUCCEEDED(hr));
				}
				else
				{
					hr = ::LoadFromWICFile(filenamews.c_str(), WIC_FLAGS_NONE, &md, img);
					assert(SUCCEEDED(hr));
				}

				hr = ::CreateShaderResourceView(m_Device.Get(), img.GetImages(), img.GetImageCount(), md, texture->m_Texture.GetAddressOf());
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

// 내장 텍스처(embeddedTexture)를 로드하고 ID3D11ShaderResourceView* 형태로 반환하는 함수입니다.
ComPtr<ID3D11ShaderResourceView> ModelLoadManager::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	HRESULT hr;
	ComPtr<ID3D11ShaderResourceView> texture = nullptr;

	// 내장 텍스처의 높이가 0이 아닌 경우(즉, 2D 텍스처인 경우) 처리합니다.
	if (embeddedTexture->mHeight != 0)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;			// 텍스처의 너비
		desc.Height = embeddedTexture->mHeight;			// 텍스처의 높이
		desc.MipLevels = 1;								// 미입 레벨 수
		desc.ArraySize = 1;								// 배열 크기
		desc.SampleDesc.Count = 1;						// 샘플링 수
		desc.SampleDesc.Quality = 0;					// 샘플링 품질
		desc.Usage = D3D11_USAGE_DEFAULT;				// 사용 방식 (기본 사용)
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;		// 텍스처 형식 (8비트 Red, Green, Blue 및 8비트 Alpha 채널)
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;	// 바인딩 플래그 (Shader Resource로 바인딩)
		desc.CPUAccessFlags = 0;						// CPU 엑세스 플래그 (없음)
		desc.MiscFlags = 0;								// 기타 플래그 (없음)

		// 텍스처 데이터를 저장하는 서브리소스 데이터 구조체를 생성하고 초기화합니다.
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;  // 텍스처 데이터의 포인터
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;  // 텍스처의 한 행 크기 (4바이트 = RGBA 4개)
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;  // 전체 텍스처 데이터 크기

		ComPtr<ID3D11Texture2D> texture2D = nullptr;
		hr = m_Device->CreateTexture2D(&desc, &subresourceData, texture2D.GetAddressOf());
		assert(SUCCEEDED(hr));

		hr = m_Device->CreateShaderResourceView(texture2D.Get(), nullptr, texture.GetAddressOf());
		assert(SUCCEEDED(hr));

		return texture;
	}

	const size_t size = embeddedTexture->mWidth;

	hr = DirectX::CreateWICTextureFromMemory(m_Device.Get(), m_DeviceContext.Get(), reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, texture.GetAddressOf());

	return texture;
}