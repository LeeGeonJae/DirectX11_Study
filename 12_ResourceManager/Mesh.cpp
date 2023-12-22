#include "Mesh.h"
#include "ImGuiMenu.h"

namespace Math = DirectX::SimpleMath;

Mesh::Mesh()
	: m_Material(nullptr)
	, m_bisTextureMapBuffer(nullptr)
{
	m_CBIsValidTextureMap.bIsValidDiffuseMap = false;
	m_CBIsValidTextureMap.bIsValidNormalMap = false;
	m_CBIsValidTextureMap.bIsValidOpcityMap = false;
	m_CBIsValidTextureMap.bIsValidSpecularMap = false;
	m_CBIsValidTextureMap.bIsValidBone = false;
	m_CBMaterial.BaseColor = Color(1.f, 1.f, 1.f, 1.f);
	m_CBMaterial.EmissiveColor = Color(1.f, 1.f, 1.f, 1.f);
}

Mesh::~Mesh()
{
}