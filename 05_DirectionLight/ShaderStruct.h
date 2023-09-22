#pragma once
#include "../Engine/Types.h"
#include <directxtk/SimpleMath.h>

struct ShaderVertex
{
	Vector3 position;
	Vector2 uv;
	Color color;
	Vector3 normal;
};

struct ConstantData
{
	DirectX::SimpleMath::Matrix World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix View;			// 뷰좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.

	Vector3 LightDir;
	Vector4 LightColor;
};

struct LightData
{
	Vector4 LightDir[2];
	Vector4 LightColor[2];
	Vector4 OutputColor;
};