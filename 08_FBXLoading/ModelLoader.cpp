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

// ���� ��Ƽ���󿡼� Ư�� Ÿ��(type)�� �ؽ�ó�� �ε��ϴ� �Լ��Դϴ�. �ε�� �ؽ�ó���� textures ���Ϳ� ����ǰ� ��ȯ�˴ϴ�.
vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* material, aiTextureType type, string typeName, const aiScene* scene)
{
	// textures ���͸� �ʱ�ȭ�մϴ�.
	vector<Texture> textures;

	// ��Ƽ����(material)���� Ư�� Ÿ��(type)�� �ؽ�ó ������ŭ �ݺ��մϴ�.
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		// �ؽ�ó ���� ��θ� ���� aiString ���� str�� �����մϴ�.
		aiString str;
		// ��Ƽ���󿡼� i��° �ؽ�ó�� ���� ��θ� ���� str�� �����մϴ�.
		material->GetTexture(type, i, &str);

		// �̹� �ε��� �ؽ�ó�� �ǳʶ��� ���θ� ��Ÿ���� �÷���(skip)�� �ʱ�ȭ�մϴ�.
		bool skip = false;

		// �̹� �ε��� �ؽ�ó�� �����ϴ� m_Textures ���͸� ��ȸ�Ͽ� ���� �ؽ�ó�� �̹� �ε�Ǿ����� Ȯ���մϴ�.
		for (UINT j = 0; j < m_Textures.size(); j++)
		{
			// �̹� �ε��� �ؽ�ó�� ���� ��ο� ���� �ؽ�ó�� ���� ��θ� ���մϴ�.
			if (strcmp(m_Textures[j].m_Path.c_str(), str.C_Str()) == 0)
			{
				// �̹� �ε��� �ؽ�ó�� ���, textures ���Ϳ� �ش� �ؽ�ó�� �߰��ϰ� skip �÷��׸� �����Ͽ� ���� �ؽ�ó�� �Ѿ�ϴ�.
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

				// �ؽ�ó ������ ��θ� �����մϴ�.
				string filename = str.C_Str();
				filename.erase(filename.begin(), filename.begin() + filename.find_last_of('\\') + 1);
				filename = m_Directory + '/' + filename;
				wstring filenamews = wstring(filename.begin(), filename.end());

				// WIC�� ����Ͽ� �ؽ�ó ������ �ε��ϰ� �̹��� ��Ÿ�����Ϳ� �̹��� �����͸� ����ϴ�.
				hr = ::LoadFromWICFile(filenamews.c_str(), WIC_FLAGS_NONE, &md, img);
				assert(SUCCEEDED(hr));

				// �ε��� �̹��� �����ͷ� Shader Resource View�� �����ϰ� �̸� texture.m_Texture�� �����մϴ�.
				hr = ::CreateShaderResourceView(m_Device, img.GetImages(), img.GetImageCount(), md, &(texture.m_Texture));
				assert(SUCCEEDED(hr));
			}

			// �ؽ�ó�� Ÿ�԰� ���� ��θ� �����ϰ� textures ���Ϳ� m_Textures ���Ϳ� �ش� �ؽ�ó�� �߰��մϴ�.
			texture.m_Type = typeName;
			texture.m_Path = str.C_Str();
			textures.push_back(texture);
			m_Textures.push_back(texture);
		}
	}

	// ��� �ؽ�ó�� �ε��ϰ� ���� textures ���͸� ��ȯ�մϴ�.
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
		// 2D �ؽ�ó�� ����(desc) ����ü�� �����ϰ� �ʱ�ȭ�մϴ�.
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
		// 2D �ؽ�ó�� �����մϴ�.
		hr = m_Device->CreateTexture2D(&desc, &subresourceData, &texture2D);
		assert(SUCCEEDED(hr));

		// 2D �ؽ�ó���� Shader Resource View(SRV)�� �����մϴ�.
		hr = m_Device->CreateShaderResourceView(texture2D, nullptr, &texture);
		assert(SUCCEEDED(hr));

		// ������ SRV�� ��ȯ�մϴ�.
		return texture;
	}

	// mHeight is 0, so try to load a compressed texture of mWidth bytes
	const size_t size = embeddedTexture->mWidth;

	hr = DirectX::CreateWICTextureFromMemory(m_Device, m_DeviceContext, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture);

	// ���� �ؽ�ó�� 2D �ؽ�ó�� �ƴ� ���, NULL�� ��ȯ�մϴ�.
	return texture;
}