#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"

struct Animation;

struct KeyFrameData
{
	float m_Time;
	Vector3 m_Scale;
	Quaternion m_Rotation;
	Vector3 m_Transtation;
};

struct AnimationNode
{
	string m_Name;
	shared_ptr<Animation> m_Animation;

	unsigned int m_FrameCount;
	float m_FrameRate;
	float m_Duration;

	vector<KeyFrameData> m_KeyFrame;
};

struct Animation
{
	string m_Name;
	unsigned int m_FrameCount;
	float m_FrameRate;
	float m_Duration;
	vector<shared_ptr<AnimationNode>> m_Nodes;
};