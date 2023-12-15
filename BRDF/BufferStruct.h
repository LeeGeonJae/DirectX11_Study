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
	//DirectX::SimpleMath::Matrix World;
	DirectX::SimpleMath::Matrix View;
	DirectX::SimpleMath::Matrix Projection;
	Vector4 MeshColor;
};

struct CBNodeTransform
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
	int UseNormalMap;
	int UseSpecularMap;
	int UseEmissiveMap;
	int UseGammaCorrection;
};

struct CBMaterial
{
	Color BaseColor;
	Color EmissiveColor;
	float OpacityValue;
	float EmissivePower;
	Vector2 dummy;
};

struct CBIsValidTextureMap
{
	int bIsValidDiffuseMap;
	int bIsValidNormalMap;
	int bIsValidMetalnessMap;
	int bIsValidRoughnessMap;
	int bIsValidSpecularMap;
	int bIsValidOpcityMap;
	int bIsValidEmissiveMap;
	int bIsValidBone;
};

struct CBMatrixPallete
{
	DirectX::SimpleMath::Matrix Array[128];
};

struct ModelCBBuffer
{
	ComPtr<ID3D11Buffer> m_pCBBoneTransformData = nullptr;
	ComPtr<ID3D11Buffer> m_pCBbisTextureMap = nullptr;
	ComPtr<ID3D11Buffer> m_pCBMaterialData = nullptr;
	ComPtr<ID3D11Buffer> m_pCBNodelData = nullptr;
};