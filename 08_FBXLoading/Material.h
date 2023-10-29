#pragma once
#include "../Engine/Header.h"
#include "Texture.h"
#include "enum.h"

class Material
{
public:
	Material();
	~Material();

public:
	inline Texture GetTextures(TextureType type);
	void SetTexture(Texture texture, TextureType type);

private:
	Texture m_Textures[static_cast<unsigned int>(TextureType::END)];
};

Texture Material::GetTextures(TextureType type)
{
	return m_Textures[static_cast<unsigned int>(type)];
}