#include "Object.h"

#include "Material.h"
#include "Mesh.h"

Object::Object()
{
}

Object::~Object()
{
}

void Object::Init()
{
}

void Object::Update()
{
}

void Object::Render(ID3D11DeviceContext* deviceContext)
{
	for (auto mesh : m_Meshes)
	{
		mesh.Draw(deviceContext);
	}
}
