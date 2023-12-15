#include "Node.h"

#include "Mesh.h"
#include "ImGuiMenu.h"
#include "../Engine/TimeManager.h"

Node::Node()
	: m_Name()
	, m_ParentName()
	, m_Parent(nullptr)
	, m_Children()
	, m_Transform()
	, m_Mesh()
	, m_Animation(nullptr)
	, m_Bone(nullptr)
	, m_NodeBuffer(nullptr)
{

	m_CBNodeTransform.World = DirectX::SimpleMath::Matrix::Identity;

}

void Node::Init(ID3D11Device* device, shared_ptr<ModelCBBuffer> NodeBuffer)
{
	m_NodeBuffer = NodeBuffer->m_pCBNodelData;

	for(auto mesh : m_Mesh)
		mesh->SetupMesh(device, NodeBuffer);

	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Init(device, NodeBuffer);
		}
	}
}

void Node::Update(ID3D11DeviceContext* deviceContext)
{
	Math::Vector3 position, scaling;
	Math::Quaternion rotation;

	// ��� ��ġ ���ϱ�
	{
		if (m_Animation != nullptr)
		{
			static float currentTime = 0.f;
			currentTime += TimeManager::GetInstance()->GetfDT() / 1.6f * ImGuiMenu::ModelAnimationSpeed;

			if (currentTime > m_Animation->m_FrameCount)
			{
				currentTime -= m_Animation->m_FrameCount;
			}

			// �ִϸ��̼� ����
			interpolateAnimationData(currentTime, position, scaling, rotation);

			m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);
		}
		else
		{
			m_Local = GetNodeTransform();
			m_Local = m_Local;
		}

		//�θ� ������ �ش� �θ��� Ʈ������ ���ϱ�
		if (m_Parent != nullptr)
		{
			m_World = m_Local  * m_Parent->m_World;
			m_CBNodeTransform.World = m_World;
		}
		else
		{
			XMMATRIX mSpin1 = XMMatrixRotationX(ImGuiMenu::ModelRotation.x * 3.14f);
			XMMATRIX mSpin2 = XMMatrixRotationY(ImGuiMenu::ModelRotation.y * 3.14f);
			XMMATRIX mScale = XMMatrixScaling(ImGuiMenu::ModelScale.x, ImGuiMenu::ModelScale.y, ImGuiMenu::ModelScale.z);
			XMMATRIX mTranslate1 = XMMatrixTranslation(ImGuiMenu::ModelPosition.x, ImGuiMenu::ModelPosition.y, ImGuiMenu::ModelPosition.z);

			// Cube World Setting
			XMMATRIX world = mScale * mSpin1 * mSpin2 * mTranslate1;

			m_World = m_Local * world;
			m_CBNodeTransform.World = m_World;
		}
	}

	// �ڽ� ��� ������Ʈ
	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Update(deviceContext);
		}
	}
}

void Node::Draw(ID3D11DeviceContext* deviceContext)
{
	for(auto mesh : m_Mesh)
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

Node::~Node()
{
	m_Parent = nullptr;

	for (auto child : m_Children)
	{
		if (child != nullptr)
			delete child;

		child = nullptr;
	}

	for (auto mesh : m_Mesh)
	{
		if (mesh != nullptr)
			delete mesh;
	}

	if (m_Animation != nullptr)
		delete m_Animation;
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
	unsigned int numFrames = m_Animation->m_FrameCount;

	// ù ��° Ű������ ������
	unsigned int frameIndex = 0;

	// �� ��° Ű������ ������
	unsigned int nextFrameIndex = 1;

	// ���� �ð��� ���� ����� Ű���������� ����
	for (unsigned int i = 0; i < numFrames - 1; ++i) {
		if (currentTime < m_Animation->m_KeyFrame[nextFrameIndex].m_Time) {
			break;
		}

		frameIndex = i;
		nextFrameIndex = i + 1;
	}

	// ���� Ű�����Ӱ� ���� Ű�������� �ð� ���� ���
	float deltaTime = m_Animation->m_KeyFrame[nextFrameIndex].m_Time - m_Animation->m_KeyFrame[frameIndex].m_Time;

	// ���� ���� Ű�����Ӱ� ���� Ű�������� �ð� ���̰� ���ٸ� 1�� ���� ( ��� ������ �ִϸ��̼� ���� ������ ���ư��� ��� �����ϱ� ���� )
	if (deltaTime == 0)
		deltaTime = 1;

	// ���� �ð��� ���� Ű�����Ӱ� ���� Ű������ ������ ���� ���
	float factor = 0;
	if (m_Animation->m_KeyFrame[frameIndex].m_Time < currentTime)
		factor = (currentTime - m_Animation->m_KeyFrame[frameIndex].m_Time) / deltaTime;

	currentPosition.x = m_Animation->m_KeyFrame[frameIndex].m_Transtation.x;
	currentPosition.y = m_Animation->m_KeyFrame[frameIndex].m_Transtation.y;
	currentPosition.z = m_Animation->m_KeyFrame[frameIndex].m_Transtation.z;
	currentScale.x = m_Animation->m_KeyFrame[frameIndex].m_Scale.x;
	currentScale.y = m_Animation->m_KeyFrame[frameIndex].m_Scale.y;
	currentScale.z = m_Animation->m_KeyFrame[frameIndex].m_Scale.z;
	currentRotation.x = m_Animation->m_KeyFrame[frameIndex].m_Rotation.x;
	currentRotation.y = m_Animation->m_KeyFrame[frameIndex].m_Rotation.y;
	currentRotation.z = m_Animation->m_KeyFrame[frameIndex].m_Rotation.z;
	currentRotation.w = m_Animation->m_KeyFrame[frameIndex].m_Rotation.w;

	nextPosition.x = m_Animation->m_KeyFrame[nextFrameIndex].m_Transtation.x;
	nextPosition.y = m_Animation->m_KeyFrame[nextFrameIndex].m_Transtation.y;
	nextPosition.z = m_Animation->m_KeyFrame[nextFrameIndex].m_Transtation.z;
	nextScale.x = m_Animation->m_KeyFrame[nextFrameIndex].m_Scale.x;
	nextScale.y = m_Animation->m_KeyFrame[nextFrameIndex].m_Scale.y;
	nextScale.z = m_Animation->m_KeyFrame[nextFrameIndex].m_Scale.z;
	nextRotation.x = m_Animation->m_KeyFrame[nextFrameIndex].m_Rotation.x;
	nextRotation.y = m_Animation->m_KeyFrame[nextFrameIndex].m_Rotation.y;
	nextRotation.z = m_Animation->m_KeyFrame[nextFrameIndex].m_Rotation.z;
	nextRotation.w = m_Animation->m_KeyFrame[nextFrameIndex].m_Rotation.w;

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