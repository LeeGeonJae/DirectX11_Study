#pragma once
#include "../Engine/Types.h"
#include <directxtk/SimpleMath.h>

struct BoneWeightVertex
{
	Vector3 m_Position;
	Vector2 m_Texcoord;
	Vector3 m_Normal;
	Vector3 m_Tangent;
	Vector3 m_BiTangetns;

	// 영향을 받는 본의 갯수는 최댕 4개로 제한한다.
	int m_BlendIndeces[4] = {};		// 참조하는 본의 인덱스의 범위는 최대 128개로 일단 처리
	float m_BlendWeights[4] = {};		// 가중치의 총합은 1이 되어야 한다.

	void AddBoneData(int boneIndex, float weight)
	{
		// 적어도 하나의 데이터가 비어있어야 한다.
		assert(m_BlendWeights[0] == 0.0f || m_BlendWeights[1] == 0.f || m_BlendWeights[2] == 0.f || m_BlendWeights[3] == 0.f);
		for (int i = 0; i < 4; i++)
		{
			if (m_BlendWeights[i] == 0.f)
			{
				m_BlendIndeces[i] = boneIndex;
				m_BlendWeights[i] = weight;
				return;
			}
		}
	}
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