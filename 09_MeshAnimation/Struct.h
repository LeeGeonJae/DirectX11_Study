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

	// 텍스처가 있는지
	bool HasTexture(TextureType type)
	{
		return m_Textures.find(static_cast<int>(type)) != m_Textures.end();
	}

	// 텍스처 타입의 텍스처 찾기
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
	vector<asAnimationNode> m_Nodes;
};
