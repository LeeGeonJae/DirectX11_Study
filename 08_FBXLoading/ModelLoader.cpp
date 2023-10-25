#include "ModelLoader.h"

#include <Directxtk/WICTextureLoader.h>

ModelLoader::ModelLoader()
	: m_Device(nullptr)
	, m_DeviceContext(nullptr)
	, m_Meshes()
	, m_Directory()
	, m_Textures()
	, m_Hwnd(nullptr)
{
	// empty
}

ModelLoader::~ModelLoader()
{
	// empty
}

bool ModelLoader::Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext, string fileName)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
	m_Hwnd = hwnd;

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

	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext* deviceContext)
{
	for (auto mesh : m_Meshes)
	{
		mesh.Draw(deviceContext);
	}
}

void ModelLoader::Close()
{

}

void ModelLoader::processNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		m_Meshes.push_back(processMesh(scene->mMeshes[node->mMeshes[i]], scene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	Mesh myMesh;

	vector<Texture> textures;

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

		myMesh.m_Vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			myMesh.m_Indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		if (diffuseMaps.size() > 0)
		{
			myMesh.m_Textures.insert(make_pair(static_cast<int>(TextureType::DIFFUSE), diffuseMaps[0]));
			myMesh.m_CBIsValidTextureMap.bIsValidDiffuseMap = true;
		}

		vector<Texture> NormalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normals", scene);
		if (NormalMaps.size() > 0)
		{
			myMesh.m_Textures.insert(make_pair(static_cast<int>(TextureType::NORMAL), NormalMaps[0]));
			myMesh.m_CBIsValidTextureMap.bIsValidNormalMap = true;
		}

		vector<Texture> SpecularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		if (SpecularMaps.size() > 0)
		{
			myMesh.m_Textures.insert(make_pair(static_cast<int>(TextureType::SPECULAR), SpecularMaps[0]));
			myMesh.m_CBIsValidTextureMap.bIsValidSpecularMap = true;
		}

		vector<Texture> OpacityMaps = loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity", scene);
		if (OpacityMaps.size() > 0)
		{
			myMesh.m_Textures.insert(make_pair(static_cast<int>(TextureType::OPACITY), OpacityMaps[0]));
			myMesh.m_CBIsValidTextureMap.bIsValidOpcityMap = true;
		}
	}

	myMesh.SetupMesh(m_Device);

	return myMesh;
}

// 모델의 머티리얼에서 특정 타입(type)의 텍스처를 로드하는 함수입니다. 로드된 텍스처들은 textures 벡터에 저장되고 반환됩니다.
vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene)
{
	// textures 벡터를 초기화합니다.
	vector<Texture> textures;

	// 머티리얼(material)에서 특정 타입(type)의 텍스처 개수만큼 반복합니다.
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		// 텍스처 파일 경로를 담을 aiString 변수 str을 생성합니다.
		aiString str;
		// 머티리얼에서 i번째 텍스처의 파일 경로를 얻어와 str에 저장합니다.
		material->GetTexture(type, i, &str);

		// 이미 로드한 텍스처를 건너뛸지 여부를 나타내는 플래그(skip)를 초기화합니다.
		bool skip = false;

		// 이미 로드한 텍스처를 저장하는 m_Textures 벡터를 순회하여 현재 텍스처가 이미 로드되었는지 확인합니다.
		for (UINT j = 0; j < m_Textures.size(); j++)
		{
			// 이미 로드한 텍스처의 파일 경로와 현재 텍스처의 파일 경로를 비교합니다.
			if (strcmp(m_Textures[j].m_Path.c_str(), str.C_Str()) == 0)
			{
				// 이미 로드한 텍스처인 경우, textures 벡터에 해당 텍스처를 추가하고 skip 플래그를 설정하여 다음 텍스처로 넘어갑니다.
				textures.push_back(m_Textures[j]);
				skip = true;
				break;
			}
		}

		// 이미 로드되지 않은 새로운 텍스처인 경우
		if (!skip)
		{
			HRESULT hr;
			Texture texture;

			// 내장 텍스처(embedded texture)를 사용할 경우
			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());
			if (embeddedTexture != nullptr)
			{
				// 내장 텍스처를 로드하고 해당 텍스처를 texture.m_Texture에 저장합니다.
				texture.m_Texture = loadEmbeddedTexture(embeddedTexture);
			}
			// 파일에서 텍스처를 로드해야 하는 경우
			else
			{
				DirectX::TexMetadata md;
				DirectX::ScratchImage img;

				// 텍스처 파일의 경로를 구성합니다.
				string filename = str.C_Str();
				filename.erase(filename.begin(), filename.begin() + filename.find_last_of('\\') + 1);
				filename = m_Directory + '/' + filename;
				wstring filenamews = wstring(filename.begin(), filename.end());

				// WIC를 사용하여 텍스처 파일을 로드하고 이미지 메타데이터와 이미지 데이터를 얻습니다.
				hr = ::LoadFromWICFile(filenamews.c_str(), WIC_FLAGS_NONE, &md, img);
				assert(SUCCEEDED(hr));

				// 로드한 이미지 데이터로 Shader Resource View를 생성하고 이를 texture.m_Texture에 저장합니다.
				hr = ::CreateShaderResourceView(m_Device, img.GetImages(), img.GetImageCount(), md, &(texture.m_Texture));
				assert(SUCCEEDED(hr));
			}

			// 텍스처의 타입과 파일 경로를 설정하고 textures 벡터와 m_Textures 벡터에 해당 텍스처를 추가합니다.
			texture.m_Type = typeName;
			texture.m_Path = str.C_Str();
			textures.push_back(texture);
			m_Textures.push_back(texture);
		}
	}

	// 모든 텍스처를 로드하고 나면 textures 벡터를 반환합니다.
	return textures;
}

// 내장 텍스처(embeddedTexture)를 로드하고 ID3D11ShaderResourceView* 형태로 반환하는 함수입니다.
ID3D11ShaderResourceView* ModelLoader::loadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	HRESULT hr;
	ID3D11ShaderResourceView* texture = nullptr;

	// 내장 텍스처의 높이가 0이 아닌 경우(즉, 2D 텍스처인 경우) 처리합니다.
	if (embeddedTexture->mHeight != 0)
	{
		// 2D 텍스처의 설명(desc) 구조체를 생성하고 초기화합니다.
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;  // 텍스처의 너비
		desc.Height = embeddedTexture->mHeight;  // 텍스처의 높이
		desc.MipLevels = 1;  // 미입 레벨 수
		desc.ArraySize = 1;  // 배열 크기
		desc.SampleDesc.Count = 1;  // 샘플링 수
		desc.SampleDesc.Quality = 0;  // 샘플링 품질
		desc.Usage = D3D11_USAGE_DEFAULT;  // 사용 방식 (기본 사용)
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // 텍스처 형식 (8비트 Red, Green, Blue 및 8비트 Alpha 채널)
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // 바인딩 플래그 (Shader Resource로 바인딩)
		desc.CPUAccessFlags = 0;  // CPU 엑세스 플래그 (없음)
		desc.MiscFlags = 0;  // 기타 플래그 (없음)

		// 텍스처 데이터를 저장하는 서브리소스 데이터 구조체를 생성하고 초기화합니다.
		D3D11_SUBRESOURCE_DATA subresourceData;
		subresourceData.pSysMem = embeddedTexture->pcData;  // 텍스처 데이터의 포인터
		subresourceData.SysMemPitch = embeddedTexture->mWidth * 4;  // 텍스처의 한 행 크기 (4바이트 = RGBA 4개)
		subresourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;  // 전체 텍스처 데이터 크기

		ID3D11Texture2D* texture2D = nullptr;
		// 2D 텍스처를 생성합니다.
		hr = m_Device->CreateTexture2D(&desc, &subresourceData, &texture2D);
		assert(SUCCEEDED(hr));

		// 2D 텍스처에서 Shader Resource View(SRV)를 생성합니다.
		hr = m_Device->CreateShaderResourceView(texture2D, nullptr, &texture);
		assert(SUCCEEDED(hr));

		// 생성된 SRV를 반환합니다.
		return texture;
	}

	// mHeight is 0, so try to load a compressed texture of mWidth bytes
	const size_t size = embeddedTexture->mWidth;

	hr = DirectX::CreateWICTextureFromMemory(m_Device, m_DeviceContext, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);

	// 내장 텍스처가 2D 텍스처가 아닌 경우, NULL을 반환합니다.
	return texture;
}