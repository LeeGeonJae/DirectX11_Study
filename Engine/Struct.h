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
	DirectX::SimpleMath::Matrix World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix View;			// 뷰좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.
};