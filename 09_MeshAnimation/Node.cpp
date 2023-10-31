#include "Node.h"
#include "Mesh.h"

Node::Node()
	: m_Name()
	, m_ParentName()
	, m_Parent(nullptr)
	, m_Children()
	, m_Transform()
	, m_Mesh(nullptr)
	, m_Material(nullptr)
	, m_Animation(nullptr)
{
}

void Node::Draw(ID3D11DeviceContext* deviceContext)
{


}

Node::~Node()
{
	m_Parent = nullptr;

	for (auto child : m_Children)
	{
		if (child != nullptr)
			delete child;

		child = nullptr;
	}

	if (m_Mesh != nullptr)
		delete m_Mesh;
	if (m_Material != nullptr)
		delete m_Material;
	if (m_Animation != nullptr)
		delete m_Animation;
}