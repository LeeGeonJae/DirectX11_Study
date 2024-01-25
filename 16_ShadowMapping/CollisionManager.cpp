#include "CollisionManager.h"
#include "CollisionComponent.h"

void CollisionManager::Init()
{

}

void CollisionManager::FixedUpdate()
{
	for (auto lhsIter = m_CollisionMap.begin(); lhsIter != m_CollisionMap.end(); lhsIter++)
	{
		shared_ptr<CollisionComponent> lhsCollsion = lhsIter->second;

		auto rhsIter = lhsIter;
		++rhsIter;

		for (; rhsIter != m_CollisionMap.end(); rhsIter++)
		{
			shared_ptr<CollisionComponent> rhsCollsion = rhsIter->second;

			if (lhsCollsion->GetCollisionType() == CollisionType::Block && rhsCollsion->GetCollisionType() == CollisionType::Block)
			{
				if (lhsCollsion->IsCollision(rhsCollsion->GetID()))
				{
					lhsCollsion->ProcessBlock(rhsCollsion->GetID());
					rhsCollsion->ProcessBlock(lhsCollsion->GetID());
				}
				else
				{
					lhsCollsion->SetCollisionState(CollisionState::None);
					rhsCollsion->SetCollisionState(CollisionState::None);
				}
			}
			else
			{
				if (lhsCollsion->IsCollision(rhsCollsion->GetID()))
				{
					lhsCollsion->ProcessOverlap(rhsCollsion->GetID());
					rhsCollsion->ProcessOverlap(lhsCollsion->GetID());
				}
				else
				{
					lhsCollsion->ProcessEscape(rhsCollsion->GetID());
					rhsCollsion->ProcessEscape(lhsCollsion->GetID());
				}
			}
		}
	}
}