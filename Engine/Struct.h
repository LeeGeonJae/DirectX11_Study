#pragma once
#include "Types.h"
#include <directxtk/SimpleMath.h>


struct Vertex
{
	Vec3 position;
	Color color;
	Vec3 normal;
	Vec2 uv;
};

struct TransformData
{
	DirectX::SimpleMath::Matrix World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix View;			// ����ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.
};