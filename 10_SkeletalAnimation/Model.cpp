#include "Model.h"

#include "Node.h"

Model::Model()
	: m_Nodes()
	, m_HeadNode(nullptr)
	, m_Animation(nullptr)
	, m_Material(nullptr)
{
}

Model::~Model()
{
}

void Model::Init(ID3D11Device* device, ComPtr<ID3D11Buffer> modelBuffer, ComPtr<ID3D11Buffer> bisTextureMapBuffer, ComPtr<ID3D11Buffer> BoneTransformBuffer)
{
	m_HeadNode->Init(device, modelBuffer, bisTextureMapBuffer, BoneTransformBuffer);
}

void Model::Update(ID3D11DeviceContext* deviceContext)
{
	m_HeadNode->Update(deviceContext);
}