#pragma once
#include "Types.h"


struct Vertex
{
	Vector3 position;
	Vector2 uv;
	Color color;
	Vector3 normal;
};

struct TransformData
{
	DirectX::SimpleMath::Matrix World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix View;			// 뷰좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.
};