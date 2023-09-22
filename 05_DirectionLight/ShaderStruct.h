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
	DirectX::SimpleMath::Matrix World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix View;			// ����ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.

	Vector3 LightDir;
	Vector4 LightColor;
};

struct LightData
{
	Vector4 LightDir[2];
	Vector4 LightColor[2];
	Vector4 OutputColor;
};