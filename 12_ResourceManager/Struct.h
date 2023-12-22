#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"
#include "enum.h"
#include "BufferStruct.h"

class Node;

struct Texture
{
	string m_Type;
	string m_Path;
	ComPtr<ID3D11ShaderResourceView> m_Texture;
};

struct Bone
{
	string m_Name;
	DirectX::SimpleMath::Matrix m_OffsetMatrix;
	unsigned int m_NumWeights;

	shared_ptr<Node> m_Owner;
};

struct NodeData
{
	string m_ModelName;
	string m_Name;
	string m_Parent;
	string m_MeshName;
	string m_MaterialName;
	string m_BoneName;

	bool GetRootNode()
	{
		if (m_Parent.empty())
			return true;

		return false;
	}
	string* GetMeshName()
	{
		if (m_MeshName.empty())
			return nullptr;

		return &m_MeshName;
	}
	string* GetMaterialName()
	{
		if (m_MaterialName.empty())
			return nullptr;

		return &m_MaterialName;
	}
	string* GetBoneName()
	{
		if (m_BoneName.empty())
			return nullptr;

		return &m_BoneName;
	}
};