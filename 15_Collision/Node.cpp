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

	// 노드 위치 곱하기
	{
		if (m_AnimationNode != nullptr)
		{
			m_CurrentTime += TimeManager::GetInstance()->GetfDT() * ImGuiMenu::ModelAnimationSpeed * 30.f;

			if (m_CurrentTime > m_AnimationNode->m_Animation->m_FrameCount)
			{
				m_CurrentTime -= m_AnimationNode->m_Animation->m_FrameCount;
			}

			// 애니메이션 보간
			interpolateAnimationData(m_CurrentTime, position, scaling, rotation);

			m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);
		}
		else
		{
			m_Local = GetNodeTransform();
		}

		//부모가 있으면 해당 부모의 트랜스폼 곱하기
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

	// 자식 노드 업데이트
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

	// 자식 노드 렌더
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

	// 애니메이션 데이터의 키프레임 개수를 가져옴
	unsigned int numFrames = m_AnimationNode->m_Animation->m_FrameCount;

	// 첫 번째 키프레임 데이터
	unsigned int frameIndex = 0;

	// 두 번째 키프레임 데이터
	unsigned int nextFrameIndex = 1;

	// 현재 시간을 가장 가까운 키프레임으로 보간
	for (unsigned int i = 0; i < numFrames - 1; ++i) {
		if (currentTime < m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Time) {
			break;
		}

		frameIndex = i;
		nextFrameIndex = i + 1;
	}

	// 현재 키프레임과 다음 키프레임의 시간 차이 계산
	float deltaTime = m_AnimationNode->m_KeyFrame[nextFrameIndex].m_Time - m_AnimationNode->m_KeyFrame[frameIndex].m_Time;

	// 만약 다음 키프레임과 현재 키프레임의 시간 차이가 없다면 1로 보정 ( 계산 오류로 애니메이션 보간 정보가 날아가는 경우 방지하기 위해 )
	if (deltaTime == 0)
		deltaTime = 1;

	// 현재 시간이 현재 키프레임과 다음 키프레임 사이의 비율 계산
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

	// 키프레임 데이터의 변환을 선형 보간하여 계산
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