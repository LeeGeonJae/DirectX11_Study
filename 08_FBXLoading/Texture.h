#pragma once
#include "../Engine/Header.h"

struct Texture {
	string m_Type;
	string m_Path;
	ID3D11ShaderResourceView* m_Texture;

	void Release() {
		if (m_Texture != nullptr)
		{
			delete m_Texture;
			m_Texture = nullptr;
		}
	}
};