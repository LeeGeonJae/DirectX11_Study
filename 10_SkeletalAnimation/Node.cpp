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
	, m_Mesh(nullptr)
	, m_Material(nullptr)
	, m_Animation(nullptr)
	, m_Bone(nullptr)
	, m_NodeBuffer(nullptr)
	, m_bisTextureMapBuffer(nullptr)
{
	m_CBIsValidTextureMap.bIsValidDiffuseMap = false;
	m_CBIsValidTextureMap.bIsValidNormalMap = false;
	m_CBIsValidTextureMap.bIsValidOpcityMap = false;
	m_CBIsValidTextureMap.bIsValidSpecularMap = false;
	m_CBNodeTransform.World = DirectX::SimpleMath::Matrix::Identity;
}

void Node::Init(ID3D11Device* device, ComPtr<ID3D11Buffer> nodeBuffer, ComPtr<ID3D11Buffer> bisTextureMapBuffer)
{
	m_NodeBuffer = nodeBuffer;
	m_bisTextureMapBuffer = bisTextureMapBuffer;

	if (m_Mesh != nullptr)
		m_Mesh->SetupMesh(device);

	for (auto child : m_Children)
	{
		if (child != nullptr)
		{
			child->Init(device, nodeBuffer, bisTextureMapBuffer);
		}
	}
}

void Node::Update(ID3D11DeviceContext* deviceContext)
{
	Math::Vector3 position, scaling;
	Math::Quaternion rotation;

	// 노드 위치 곱하기
	{
		if (m_Animation != nullptr)
		{
			static float currentTime = 0.f;
			currentTime += TimeManager::GetInstance()->GetfDT() / 10;

			if (currentTime > m_Animation->m_FrameCount)
			{
				currentTime -= m_Animation->m_FrameCount;
			}

			// 애니메이션 보간
			interpolateAnimationData(currentTime, position, scaling, rotation);

			m_Local = Math::Matrix::CreateScale(scaling) * Math::Matrix::CreateFromQuaternion(rotation) * Math::Matrix::CreateTranslation(position);
		}
		else
		{
			m_Local = GetNodeTransform();
			m_Local = m_Local;
		}

		//부모가 있으면 해당 부모의 트랜스폼 곱하기
		if (m_Parent != nullptr)
		{
			m_World = m_Local * m_Parent->m_World;
			m_CBNodeTransform.World = m_World;
		}
		else
		{
			m_World = m_Local;
			m_CBNodeTransform.World = m_World;
		}
	}

	// 자식 노드 업데이트
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
	if (m_Mesh != nullptr)
	{
		for (int i = 0; i < static_cast<int>(TextureType::END); i++)
		{
			auto find = m_Material->GetTexture(static_cast<TextureType>(i));
			if (find != nullptr)
			{
				deviceContext->PSSetShaderResources(i, 1, &find->m_Texture);
			}
		}

		auto matrix = m_CBNodeTransform.World.Transpose();
		deviceContext->UpdateSubresource(m_bisTextureMapBuffer.Get(), 0, nullptr, &m_CBIsValidTextureMap, 0, 0);
		deviceContext->UpdateSubresource(m_NodeBuffer.Get(), 0, nullptr, &matrix, 0, 0);


		deviceContext->VSSetConstantBuffers(4, 1, m_bisTextureMapBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(4, 1, m_bisTextureMapBuffer.GetAddressOf());
		deviceContext->VSSetConstantBuffers(5, 1, m_NodeBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(5, 1, m_NodeBuffer.GetAddressOf());


		m_Mesh->Draw(deviceContext);
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

void Node::interpolateAnimationData(float currentTime, Vector3& outPosition, Vector3& outScaling, Math::Quaternion& outRotation)
{
	aiVector3D currentPosition;
	aiVector3D currentScale;
	aiQuaternion currentRotation;
	aiVector3D nextPosition;
	aiVector3D nextScale;
	aiQuaternion nextRotation;

	// 애니메이션 데이터의 키프레임 개수를 가져옴
	unsigned int numFrames = m_Animation->m_FrameCount;

	// 첫 번째 키프레임 데이터
	unsigned int frameIndex = 0;

	// 두 번째 키프레임 데이터
	unsigned int nextFrameIndex = 1;

	// 현재 시간을 가장 가까운 키프레임으로 보간
	for (unsigned int i = 0; i < numFrames - 1; ++i) {
		if (currentTime < m_Animation->m_KeyFrame[nextFrameIndex].m_Time) {
			break;
		}

		frameIndex = i;
		nextFrameIndex = i + 1;
	}

	// 현재 키프레임과 다음 키프레임의 시간 차이 계산
	float deltaTime = m_Animation->m_KeyFrame[nextFrameIndex].m_Time - m_Animation->m_KeyFrame[frameIndex].m_Time;

	// 만약 다음 키프레임과 현재 키프레임의 시간 차이가 없다면 1로 보정 ( 계산 오류로 애니메이션 보간 정보가 날아가는 경우 방지하기 위해 )
	if (deltaTime == 0)
		deltaTime = 1;

	// 현재 시간이 현재 키프레임과 다음 키프레임 사이의 비율 계산
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