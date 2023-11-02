#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"
#include "enum.h"

struct Texture
{
	string m_Type;
	string m_Path;
	ID3D11ShaderResourceView* m_Texture;

	void Release()
	{
		if (m_Texture != nullptr)
		{
			delete m_Texture;
			m_Texture = nullptr;
		}
	}
};

struct Material
{
	map<int, Texture*> m_Textures;
	string m_Name;

	// �ؽ�ó�� �ִ���
	bool HasTexture(TextureType type)
	{
		return m_Textures.find(static_cast<int>(type)) != m_Textures.end();
	}

	// �ؽ�ó Ÿ���� �ؽ�ó ã��
	Texture* GetTexture(TextureType type)
	{
		if (m_Textures.find(static_cast<int>(type)) != m_Textures.end())
			return m_Textures.find(static_cast<int>(type))->second;

		return nullptr;
	}
};

struct asKeyFrameData
{
	float m_Time;
	Vector3 m_Scale;
	Quaternion m_Rotation;
	Vector3 m_Transtation;
};

struct asKeyFrame
{
	string m_boneName;
	vector<asKeyFrameData> m_Transform;
};

struct asAnimationNode
{
	string m_Name;
	unsigned int m_FrameCount;
	float m_FrameRate;
	float m_Duration;
	vector<asKeyFrameData> m_KeyFrame;
};

struct asAnimation
{
	string m_Name;
	unsigned int m_FrameCount;
	float m_FrameRate;
	float m_Duration;
	vector<asAnimationNode*> m_Nodes;
};

struct BoneWeightVertex
{
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

struct Bone
{
	string m_Name;
	DirectX::SimpleMath::Matrix m_OffsetMatrix;
	unsigned int m_NumWeights;
	BoneWeightVertex m_Weights;
};