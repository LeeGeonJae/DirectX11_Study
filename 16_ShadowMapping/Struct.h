#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"
#include "enum.h"
#include "BufferStruct.h"

class Node;

struct Texture
{
	Texture()
	{
		int a = 0;
	}

	string m_Type;
	string m_Path;
	ComPtr<ID3D11ShaderResourceView> m_Texture;
};

struct Bone
{
	string m_Name;
	DirectX::SimpleMath::Matrix m_OffsetMatrix;

	shared_ptr<Node> m_Owner;
};

struct Skeletal
{
	string m_Name;
	vector<shared_ptr<Bone>> m_Bones;
};

struct NodeData
{
	string m_Name;
	string m_Parent;
	string m_MeshName;
	string m_MaterialName;

	bool GetRootNode()
	{
		if (m_Parent.empty())
			return true;

		return false;
	}
};

struct ModelData
{
	string m_Name;
	vector<shared_ptr<NodeData>> m_Nodes;
};