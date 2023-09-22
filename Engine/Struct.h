#pragma once
#include "Types.h"
#include <directxtk/SimpleMath.h>


struct Vertex
{
	Vec3 position;
	Vec2 uv;
	Color color;
	Vec3 normal;
};

struct TransformData
{
	DirectX::SimpleMath::Matrix World;		// ������ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix View;			// ����ǥ�� �������� ��ȯ�� ���� ���.
	DirectX::SimpleMath::Matrix Projection;	// ������ġ��ǥ��( Normalized Device Coordinate) �������� ��ȯ�� ���� ���.
};