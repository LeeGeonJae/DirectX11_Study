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

	// 리소스가 없으면 로드 있으면 모델의 노드 데이터에 따라 데이터 파싱
	if (modelData == nullptr)
	{
		model = ModelLoadManager::GetInstance()->Load(path);
		return model;
	}

	// 리소스 있으면
	// model Setting
	model = make_shared<Model>();

	// 스켈레탈
	auto findSkeletal = FindSkeletal(path);
	// 애니메이션
	auto animation = FindAnimation(path);

	// 애니메이션 등록
	model->SetAnimation(animation);

	// 해당 모델의 노드 갯수만큼 순회
	for (auto nodeData : modelData->m_Nodes)
	{
		// node Setting
		shared_ptr<Node> node = make_shared<Node>();

		node->SetName(nodeData->m_Name);

		// 1. 루트 노드인가?
		if (nodeData->GetRootNode())
			model->SetHeadNode(node);

		// 2. 메시가 있는가?
		if (!nodeData->m_MeshName.empty())
		{

			// 2-1. 본이 있는가? 
			if (findSkeletal != nullptr)			// Skeltal Mesh
			{
				auto findSkeletalMesh = FindSkeletalMesh(nodeData->m_MeshName);

				model->SetBoundingBox(findSkeletalMesh->GetAABBmin(), findSkeletalMesh->GetAABBmax());

				// 2-2. 머터리얼이 있는가?
				if (!nodeData->m_MaterialName.empty())
				{
					auto findMaterial = FindMaterial(nodeData->m_MaterialName);
					findSkeletalMesh->SetMaterial(findMaterial);
				}

				// 스켈레탈 메쉬 맞는 노드에 등록
				if (findSkeletalMesh != nullptr)
				{
					node->SetMesh(dynamic_pointer_cast<Mesh>(findSkeletalMesh));
				}
			}
			else									// Static Mesh
			{
				// 스태틱 메쉬 맞는 노드에 등록
				auto findStaticMesh = FindStaticMesh(nodeData->m_MeshName);

				model->SetBoundingBox(findStaticMesh->GetAABBmin(), findStaticMesh->GetAABBmax());

				// 2-2. 머터리얼이 있는가?
				if (!nodeData->m_MaterialName.empty())
				{
					auto findMaterial = FindMaterial(nodeData->m_MaterialName);
					findStaticMesh->SetMaterial(findMaterial);
				}

				// 스태틱 메쉬 맞는 노드에 등록
				if (findStaticMesh != nullptr)
				{
					node->SetMesh(dynamic_pointer_cast<Mesh>(findStaticMesh));
				}
			}
		}

		// 3. 애니메이션이 있는가?
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
		
		// 4. 노드 등록
		model->SetNode(node);
	}

	// 5. 모델에 본 등록
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

	// 5. 노드 계층구조 세팅
	for (auto nodeData : modelData->m_Nodes)
	{
		shared_ptr<Node> currentNode;
		shared_ptr<Node> parentNode;

		// 루트 노드면 다음 노드로
		if (nodeData->m_Parent.empty())
			continue;

		// 노드 순회 찾기 ( 현재 노드, 현재 노드의 부모 노드 )
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

		// 부모 노드로 세팅 ( 부모 노드는 현재 노드를 자식 노드로 세팅 )
		currentNode->SetParentNode(parentNode);
	}

	return model;
}