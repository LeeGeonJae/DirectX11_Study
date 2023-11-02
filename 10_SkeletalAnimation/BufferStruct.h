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
	int		m_BlendIndices[4];
	Vector4 m_BlendWeights;
};

struct CBCoordinateData
{
	DirectX::SimpleMath::Matrix World;
	DirectX::SimpleMath::Matrix View;
	DirectX::SimpleMath::Matrix Projection;
	Vector4 MeshColor;
};

struct CBModelTransform
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
	bool UseNormalMap;
	bool UseSpecularMap;
	bool UseGammaCorrection;
	bool dummy;
	Vector3 dummy2;
};

struct CBIsValidTextureMap
{
	bool bIsValidDiffuseMap;
	bool bIsValidNormalMap;
	bool bIsValidSpecularMap;
	bool bIsValidOpcityMap;
	Vector3 dummy;
};

struct CBMatrixPallete
{
	DirectX::SimpleMath::Matrix Array[128];
};