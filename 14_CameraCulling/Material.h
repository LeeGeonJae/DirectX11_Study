#pragma once
#include "../Engine/Header.h"
#include "../Engine/Types.h"
#include "enum.h"

struct Texture;

struct Material
{
	string m_Name;
	Vector3 m_BaseColor;
	Vector3 m_EmissiveColor;
	map<int, shared_ptr<Texture>> m_Textures;

	// �ؽ�ó�� �ִ���
	bool HasTexture(TextureType type)
	{
		return m_Textures.find(static_cast<int>(type)) != m_Textures.end();
	}

	// �ؽ�ó Ÿ���� �ؽ�ó ã��
	shared_ptr<Texture> GetTexture(TextureType type)
	{
		if (m_Textures.find(static_cast<int>(type)) != m_Textures.end())
			return m_Textures.find(static_cast<int>(type))->second;

		return nullptr;
	}
};