#include "Model.h"

#include "Node.h"

Model::Model()
	: m_Nodes()
	, m_HeadNode(nullptr)
	, m_Animation(nullptr)
	, m_Material(nullptr)
	, m_CBNodeData(nullptr)
{
}

Model::~Model()
{
}

void Model::Init(ID3D11Device* device, ComPtr<ID3D11Buffer> modelData)
{
	m_CBNodeData = modelData;
	m_HeadNode->Init(device, m_CBNodeData);
}

void Model::Update(ID3D11DeviceContext* deviceContext)
{
	m_HeadNode->Update(deviceContext);
}