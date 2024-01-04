#include "Node.h"

#include "Mesh.h"
#include "Material.h"
#include "ImGuiMenu.h"
#include "../Engine/TimeManager.h"

Node::Node()
	: m_Name()
	, m_ParentName()
	, m_Parent(nullptr)
	, m_Children()
	, m_Transform()
	, m_Mesh()
	, m_AnimationNode(nullptr)
	, m_Bone(nullptr)
	, m_NodeBuffer(nullptr)
	, m_CurrentTime(0.f)
{
	m_CBNodeTransform.World = DirectX::SimpleMath::Matrix::Identity;
}

Node::~Node()
{
}

void Node::Init(ComPtr<ID3D11Device> device, shared_ptr<ModelCBBuffer> NodeBuffer)
{
	m_NodeBuffer = NodeBuffer->m_pCBNodelData;

	for (auto mesh : m_Mesh)
		mesh->SetupMesh(device, NodeBuffer);

	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Init(device, NodeBuffer);
		}
	}
}

void Node::Update(ComPtr<ID3D11DeviceContext> deviceContext, SimpleMath::Matrix modelTransform)
{
	Math::Vector3 position, scaling;
	Math::Quaternion rotation;

	// ��� ��ġ ���ϱ�
	{
		if (m_AnimationNode != nullptr)
		{
			m_CurrentTime += TimeManager::GetInstance()->GetfDT() * ImGuiMenu::ModelAnimationSpeed * 30.f;

			if (m_CurrentTime > m_AnimationNode->m_Animation->m_FrameCount)
			{
				m_CurrentTime -= m_AnimationNode->m_Animation->m_FrameCount;
			}

			// �ִϸ��̼� ����
			interpolateAnimationData(m_CurrentTime, position, scaling, rotation);

			m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);
		}
		else
		{
			m_Local = GetNodeTransform();
		}

		//�θ� ������ �ش� �θ��� Ʈ������ ���ϱ�
		if (m_Parent != nullptr)
		{
			m_World = m_Local * m_Parent->m_World;
			m_CBNodeTransform.World = m_World;
		}
		else
		{
			m_World = m_Local * modelTransform;
			m_CBNodeTransform.World = m_World;
		}
	}

	// �ڽ� ��� ������Ʈ
	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Update(deviceContext, modelTransform);
		}
	}
}

void Node::Draw(ComPtr<ID3D11DeviceContext> deviceContext)
{
	for (auto mesh : m_Mesh)
	{
		Math::Matrix matrix = m_CBNodeTransform.World.Transpose();

		deviceContext->UpdateSubresource(m_NodeBuffer.Get(), 0, nullptr, &matrix, 0, 0);

		deviceContext->VSSetConstantBuffers(6, 1, m_NodeBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(6, 1, m_NodeBuffer.GetAddressOf());

		mesh->Draw(deviceContext);
	}

	// �ڽ� ��� ����
	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Draw(deviceContext);
		}
	}
}

void Node::interpolateAnimationData(float currentTime, Vector3& outPosition, Vector3& outScaling, Math::Quaternion& outRotation)
{
	aiVector3D currentPosition;
	aiVector3D currentScale;
	aiQuaternion currentRotation;
	aiVector3D nextPosition;
	aiVector3D nextScale;
	aiQuaternion nextRotation;

	// �ִϸ��̼� �������� Ű������ ������ ������
	unsigned int numFrames = m_AnimationNode->m_Animation->m_FrameCount;

	// ù ��° Ű������ ������
	unsigned int frameIndex = 0;

	// �� ��° Ű������ ������
	unsigned int nextFrameIndex = 1;

	// ���� �ð��� ���� ����� Ű���������� ����
	for (unsigned int i = 0; i < numFrames - 1; ++i) {
		if (currentTime < m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Time) {
			break;
		}

		frameIndex = i;
		nextFrameIndex = i + 1;
	}

	// ���� Ű�����Ӱ� ���� Ű�������� �ð� ���� ���
	float deltaTime = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Time - m_AnimationNode->m_KeyFrame[frameIndex].m_Time;

	// ���� ���� Ű�����Ӱ� ���� Ű�������� �ð� ���̰� ���ٸ� 1�� ���� ( ��� ������ �ִϸ��̼� ���� ������ ���ư��� ��� �����ϱ� ���� )
	if (deltaTime == 0)
		deltaTime = 1;

	// ���� �ð��� ���� Ű�����Ӱ� ���� Ű������ ������ ���� ���
	float factor = 0;
	if (m_AnimationNode->m_KeyFrame[frameIndex].m_Time < currentTime)
		factor = (currentTime - m_AnimationNode->m_KeyFrame[frameIndex].m_Time) / deltaTime;

	currentPosition.x = m_AnimationNode->m_KeyFrame[frameIndex].m_Transtation.x;
	currentPosition.y = m_AnimationNode->m_KeyFrame[frameIndex].m_Transtation.y;
	currentPosition.z = m_AnimationNode->m_KeyFrame[frameIndex].m_Transtation.z;
	currentScale.x = m_AnimationNode->m_KeyFrame[frameIndex].m_Scale.x;
	currentScale.y = m_AnimationNode->m_KeyFrame[frameIndex].m_Scale.y;
	currentScale.z = m_AnimationNode->m_KeyFrame[frameIndex].m_Scale.z;
	currentRotation.x = m_AnimationNode->m_KeyFrame[frameIndex].m_Rotation.x;
	currentRotation.y = m_AnimationNode->m_KeyFrame[frameIndex].m_Rotation.y;
	currentRotation.z = m_AnimationNode->m_KeyFrame[frameIndex].m_Rotation.z;
	currentRotation.w = m_AnimationNode->m_KeyFrame[frameIndex].m_Rotation.w;

	nextPosition.x = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Transtation.x;
	nextPosition.y = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Transtation.y;
	nextPosition.z = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Transtation.z;
	nextScale.x = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Scale.x;
	nextScale.y = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Scale.y;
	nextScale.z = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Scale.z;
	nextRotation.x = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Rotation.x;
	nextRotation.y = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Rotation.y;
	nextRotation.z = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Rotation.z;
	nextRotation.w = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Rotation.w;

	// Ű������ �������� ��ȯ�� ���� �����Ͽ� ���
	aiVector3D fianlPosition = currentPosition + factor * (nextPosition - currentPosition);
	aiVector3D fianlScale = currentScale + factor * (nextScale - currentScale);
	aiQuaternion fianlRotation;
	aiQuaternion::Interpolate(fianlRotation, currentRotation, nextRotation, factor);

	outPosition.x = fianlPosition.x;
	outPosition.y = fianlPosition.y;
	outPosition.z = fianlPosition.z;
	outScaling.x = fianlScale.x;
	outScaling.y = fianlScale.y;
	outScaling.z = fianlScale.z;
	outRotation.x = fianlRotation.x;
	outRotation.y = fianlRotation.y;
	outRotation.z = fianlRotation.z;
	outRotation.w = fianlRotation.w;
}