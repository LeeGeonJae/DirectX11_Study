#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"

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
