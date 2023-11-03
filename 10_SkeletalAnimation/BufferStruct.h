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

	// ������ �޴� ���� ������ �ִ� 4���� �����Ѵ�.
	int m_BlendIndeces[4] = {};		// �����ϴ� ���� �ε����� ������ �ִ� 128���� �ϴ� ó��
	float m_BlendWeights[4] = {};		// ����ġ�� ������ 1�� �Ǿ�� �Ѵ�.

	void AddBoneData(int boneIndex, float weight)
	{
		// ��� �ϳ��� �����Ͱ� ����־�� �Ѵ�.
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