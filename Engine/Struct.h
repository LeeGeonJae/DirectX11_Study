#pragma once
#include "Types.h"
#include <directxtk/SimpleMath.h>


struct Vertex
{
	Vec3 position;
	Vec2 uv;
	Color color;
};

struct TransformData
{
	Vec3 offset;
	float dummy;	// 16바이트로 맞추기 위해서 더미 값 추가
	DirectX::SimpleMath::Matrix World;		// 월드좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix View;			// 뷰좌표계 공간으로 변환을 위한 행렬.
	DirectX::SimpleMath::Matrix Projection;	// 단위장치좌표계( Normalized Device Coordinate) 공간으로 변환을 위한 행렬.
};