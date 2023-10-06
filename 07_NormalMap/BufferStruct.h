#pragma once
#include "../Engine/Types.h"
#include <directxtk/SimpleMath.h>

struct ShaderVertex
{
	Vector3 Position;
	Vector2 UV;
	Color Color;
	Vector3 Normal;
	Vector3 Tangent;
};

struct CBCoordinateData
{
	DirectX::SimpleMath::Matrix World;
	DirectX::SimpleMath::Matrix View;
	DirectX::SimpleMath::Matrix Projection;
	Vector4 MeshColor;
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

struct CBNormalMap
{
	int UseNormalMap;
	Vector3 dummy;
};