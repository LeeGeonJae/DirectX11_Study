#include "ResourceManager.h"

#include "ModelLoadManager.h"
#include "Model.h"
#include "Node.h"
#include "SkeletalMesh.h"
#include "StaticMesh.h"
#include "Animation.h"

void ResourceManager::Init(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	m_Device = device;
	m_DeviceContext = deviceContext;
}

shared_ptr<Model> ResourceManager::CreateModel(string path)
{
	shared_ptr<ModelData> modelData = FindModelData(path);
	shared_ptr<Model> model;

	// ���ҽ��� ������ �ε� ������ ���� ��� �����Ϳ� ���� ������ �Ľ�
	if (modelData == nullptr)
	{
		model = ModelLoadManager::GetInstance()->Load(path);
		return model;
	}

	// ���ҽ� ������
	// model Setting
	model = make_shared<Model>();

	// ���̷�Ż
	auto findSkeletal = FindSkeletal(path);
	// �ִϸ��̼�
	auto animation = FindAnimation(path);

	// �ִϸ��̼� ���
	model->SetAnimation(animation);

	// �ش� ���� ��� ������ŭ ��ȸ
	for (auto nodeData : modelData->m_Nodes)
	{
		// node Setting
		shared_ptr<Node> node = make_shared<Node>();

		node->SetName(nodeData->m_Name);

		// 1. ��Ʈ ����ΰ�?
		if (nodeData->GetRootNode())
			model->SetHeadNode(node);

		// 2. �޽ð� �ִ°�?
		if (!nodeData->m_MeshName.empty())
		{

			// 2-1. ���� �ִ°�? 
			if (findSkeletal != nullptr)			// Skeltal Mesh
			{
				auto findSkeletalMesh = FindSkeletalMesh(nodeData->m_MeshName);

				model->SetBoundingBox(findSkeletalMesh->GetAABBmin(), findSkeletalMesh->GetAABBmax());

				// 2-2. ���͸����� �ִ°�?
				if (!nodeData->m_MaterialName.empty())
				{
					auto findMaterial = FindMaterial(nodeData->m_MaterialName);
					findSkeletalMesh->SetMaterial(findMaterial);
				}

				// ���̷�Ż �޽� �´� ��忡 ���
				if (findSkeletalMesh != nullptr)
				{
					node->SetMesh(dynamic_pointer_cast<Mesh>(findSkeletalMesh));
				}
			}
			else									// Static Mesh
			{
				// ����ƽ �޽� �´� ��忡 ���
				auto findStaticMesh = FindStaticMesh(nodeData->m_MeshName);

				model->SetBoundingBox(findStaticMesh->GetAABBmin(), findStaticMesh->GetAABBmax());

				// 2-2. ���͸����� �ִ°�?
				if (!nodeData->m_MaterialName.empty())
				{
					auto findMaterial = FindMaterial(nodeData->m_MaterialName);
					findStaticMesh->SetMaterial(findMaterial);
				}

				// ����ƽ �޽� �´� ��忡 ���
				if (findStaticMesh != nullptr)
				{
					node->SetMesh(dynamic_pointer_cast<Mesh>(findStaticMesh));
				}
			}
		}

		// 3. �ִϸ��̼��� �ִ°�?
		if (animation)
		{
			for (auto animationNode : animation->m_Nodes)
			{
				if (animationNode->m_Name == nodeData->m_Name)
				{
					node->SetAnimation(animationNode);
				}
			}
		}
		
		// 4. ��� ���
		model->SetNode(node);
	}

	// 5. �𵨿� �� ���
	if (findSkeletal != nullptr)
	{
		for (auto bone : findSkeletal->m_Bones)
		{
			shared_ptr<Bone> myBone = make_shared<Bone>();
			myBone->m_Name = bone->m_Name;
			myBone->m_OffsetMatrix = bone->m_OffsetMatrix;

			for (auto node : model->GetNode())
			{
				if (node->GetName() == bone->m_Name)
				{
					myBone->m_Owner = node;
					node->SetBone(myBone);
					model->SetBone(myBone);
					break;
				}
			}
		}
	}

	// 5. ��� �������� ����
	for (auto nodeData : modelData->m_Nodes)
	{
		shared_ptr<Node> currentNode;
		shared_ptr<Node> parentNode;

		// ��Ʈ ���� ���� ����
		if (nodeData->m_Parent.empty())
			continue;

		// ��� ��ȸ ã�� ( ���� ���, ���� ����� �θ� ��� )
		for (auto node : model->GetNode())
		{
			if (nodeData->m_Name == node->GetName())
			{
				currentNode = node;
			}
			if (nodeData->m_Parent == node->GetName())
			{
				parentNode = node;
			}
		}

		// �θ� ���� ���� ( �θ� ���� ���� ��带 �ڽ� ���� ���� )
		currentNode->SetParentNode(parentNode);
	}

	return model;
}