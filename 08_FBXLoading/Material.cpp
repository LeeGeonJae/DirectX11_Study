#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::SetTexture(Texture texture, TextureType type)
{
	m_Textures[static_cast<unsigned int>(type)] = texture;
}