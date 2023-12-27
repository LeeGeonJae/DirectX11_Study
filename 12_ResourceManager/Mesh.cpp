#include "Mesh.h"
#include "ImGuiMenu.h"

namespace Math = DirectX::SimpleMath;

Mesh::Mesh()
	: m_Material(nullptr)
	, m_bisTextureMapBuffer(nullptr)
{
	//m_CBIsValidTextureMap.bIsValidTextureMap = false;
	m_CBIsValidTextureMap.bIsValidBone = false;
	m_CBMaterial.BaseColor = Color(1.f, 1.f, 1.f, 1.f);
	m_CBMaterial.EmissiveColor = Color(1.f, 1.f, 1.f, 1.f);
}

Mesh::~Mesh()
{
}

void Mesh::LoadShaderFromFile(const wstring& path, const string& name, const string& version, ComPtr<ID3DBlob>& blob)
{
	const uint32 compileFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

	HRESULT hr = ::D3DCompileFromFile(
		path.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		name.c_str(),
		version.c_str(),
		compileFlag,
		0,
		blob.GetAddressOf(),
		nullptr);

	assert(SUCCEEDED(hr));
}