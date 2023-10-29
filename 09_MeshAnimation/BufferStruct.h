#pragma once
#include "../Engine/Types.h"
#include <directxtk/SimpleMath.h>

struct ShaderVertex
{
	Vector3 m_Position;
	Vector2 m_Texcoord;
	Vector3 m_Normal;
	Vector3 m_Tangent;
	Vector3 m_BiTangetns;
};

struct CBCoordinateData
{
	DirectX::SimpleMath::Matrix World;
	DirectX::SimpleMath::Matrix View;
	DirectX::SimpleMath::Matrix Projection;
	Vector4 MeshColor;
};

struct CBMeshTransform
{
	DirectX::SimpleMath::Matrix World;
};

struct CBLightData
{
	Vector4 LightDir;
	Vector4 LightColor;
	float SpecularPower;
	Vector3 AmbientColor;
};

struct CBCameraData
{
	Vector4 CameraPosition;
};

struct CBUseTextureMap
{
	int UseNormalMap;
	int UseSpecularMap;
	int UseGammaCorrection;
	int dummy;
};

struct CBIsValidTextureMap
{
	bool bIsValidDiffuseMap;
	bool bIsValidNormalMap;
	bool bIsValidSpecularMap;
	bool bIsValidOpcityMap;
};