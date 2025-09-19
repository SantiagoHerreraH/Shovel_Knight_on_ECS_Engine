#include "../../../include/Systems/OptimizationSystems/UOptimization.h"
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Structs/FPlayer.h"
#include "../../../include/Structs/FCamera.h"

#include "../../../../Engine/utils.h"

namespace Pillar {
	
	
	UCalculateOptimizationRectCollision::UCalculateOptimizationRectCollision(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FRectCollider{}, FEntityResolveOverlapInfo{}, ECollisionBodyType{}, FOptimizationGridCellState{}, FSceneObject{});

		registry.GetComponents(m_OptimizationRectColliders, filter);
		registry.GetComponents(m_OptimizationShapeOverlapInfos, filter);
		registry.GetComponents(m_OptimizationSceneObjects, filter);

		filter.SetIncludingFilter(FRectCollider{}, FEntityResolveOverlapInfo{}, ECollisionBodyType{}, FSceneObject{});
		filter.SetExcludingFilter(FOptimizationGridCellState{});

		registry.GetComponents(m_AllRectColliders, filter);
		registry.GetComponents(m_AllSceneObjects, filter);

		filter.SetIncludingFilter(FRectCollider{}, FPlayer{});
		filter.DeleteExcludingFilter();

		registry.GetComponents(m_PlayerRectCollider, filter);
	}

	void UCalculateOptimizationRectCollision::Initialize()
	{
		UComponentIterator<FPlayer> player{};
		m_Registry.GetComponents(player);
		m_Registry.GetEntity<FPlayer>(m_PlayerEntityHandle, player.GetArchetype(0), 0);

		UEntityHandle firstEntityHandle{};
		UEntityHandle secondEntityHandle{};

		FResolveOverlapInfo firstResolveOverlapInfo{};
		FResolveOverlapInfo secondResolveOverlapInfo{};

		for (size_t archIdx = 0; archIdx < m_OptimizationRectColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_OptimizationRectColliders.NumOfComponents(archIdx); compIdx++)
			{
				for (size_t otherArchIdx = 0; otherArchIdx < m_AllRectColliders.NumOfArchetypes(); otherArchIdx++)
				{
					for (size_t otherCompIdx = 0; otherCompIdx < m_AllRectColliders.NumOfComponents(otherArchIdx); otherCompIdx++)
					{
						if (m_OptimizationSceneObjects.At(archIdx, compIdx).SceneIndex !=
							m_AllSceneObjects.At(otherArchIdx, otherCompIdx).SceneIndex)
						{
							continue;
						}

						m_Registry.GetEntity<FEntityResolveOverlapInfo>(secondEntityHandle, m_AllRectColliders.GetArchetype(otherArchIdx), otherCompIdx);

						firstResolveOverlapInfo.OtherIsOptimized = m_AllRectColliders.At(otherArchIdx, otherCompIdx).Optimize;

						if (SCollision::IsOverlapping(
							m_OptimizationRectColliders.At(archIdx, compIdx).CurrentRect,
							m_AllRectColliders.At(otherArchIdx, otherCompIdx).CurrentRect,
							firstResolveOverlapInfo,
							secondResolveOverlapInfo))
						{

							m_OptimizationShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.AddOrSet(secondEntityHandle, firstResolveOverlapInfo);
						}
						else
						{
							m_OptimizationShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.SwapRemove(secondEntityHandle);
						}
					}
				}

			}
		}
	}

	void UCalculateOptimizationRectCollision::Update()
	{

		FResolveOverlapInfo firstResolveOverlapInfo{};
		FResolveOverlapInfo secondResolveOverlapInfo{};

		for (size_t archIdx = 0; archIdx < m_OptimizationRectColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_OptimizationRectColliders.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t otherArchIdx = 0; otherArchIdx < m_PlayerRectCollider.NumOfArchetypes(); otherArchIdx++)
				{
					for (size_t otherCompIdx = 0; otherCompIdx < m_PlayerRectCollider.NumOfActivatedComponents(otherArchIdx); otherCompIdx++)
					{
						firstResolveOverlapInfo.OtherIsOptimized = false;//player is not optimized

						if (SCollision::IsOverlapping(
							m_OptimizationRectColliders.At(archIdx, compIdx).CurrentRect,
							m_PlayerRectCollider.At(otherArchIdx, otherCompIdx).CurrentRect,
							firstResolveOverlapInfo,
							secondResolveOverlapInfo))
						{

							m_OptimizationShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.AddOrSet(m_PlayerEntityHandle, firstResolveOverlapInfo);
						}
						else
						{
							m_OptimizationShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.SwapRemove(m_PlayerEntityHandle);
						}
					}
				}
			}
		}
	}	

	UOptimizationBoxSystem::UOptimizationBoxSystem(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FEntityResolveOverlapInfo{}, FOptimizationGridCellState{}, FRectCollider{});

		m_Registry.GetComponents(m_OptimizationOverlapInfos, filter);
		m_Registry.GetComponents(m_OptimizationStates, filter);

	}

	void UOptimizationBoxSystem::Initialize()
	{

		UComponentIterator<FPlayer> player{};
		m_Registry.GetComponents(player);
		m_Registry.GetEntity<FPlayer>(m_PlayerEntityHandle, player.GetArchetype(0), 0);

		for (size_t archIdx = 0; archIdx < m_OptimizationOverlapInfos.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_OptimizationOverlapInfos.NumOfComponents(archIdx); compIdx++)
			{
				m_OptimizationStates.At(archIdx, compIdx).Active = 
					!m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Contains(m_PlayerEntityHandle);

				m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.Reserve(m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities());

				for (size_t enttIdx = 0; enttIdx < m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
				{
					if (m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ValueAt(enttIdx).OtherIsOptimized)
					{
						const UEntityHandle& currentHandle{ m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx) };
						m_Entities_To_NumberOfTimesActivated.Add(currentHandle, 0);
						m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.
							Add(currentHandle, EActivationStateForOptimization::ActiveBeforeDeactivating);
					}	
				}
			}
		}
	}

	void UOptimizationBoxSystem::Update()
	{
		m_EntitiesToActivate.Clear();
		m_EntitiesToDeactivate.Clear();

		for (size_t archIdx = 0; archIdx < m_OptimizationOverlapInfos.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_OptimizationOverlapInfos.NumOfActivatedComponents(archIdx); compIdx++)
			{
				//if contains camera collider on
				if (m_OptimizationOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Contains(m_PlayerEntityHandle))
				{
					if (!m_OptimizationStates.At(archIdx, compIdx).Active)
					{
						m_OptimizationStates.At(archIdx, compIdx).Active = true;

						const UEntityHandle* currentEntityHandle{ nullptr };

						for (size_t enttIdx = 0; enttIdx < m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.NumOfEntities(); enttIdx++)
						{
							currentEntityHandle = &m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.ConstEntityAt(enttIdx);

							if (m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.ConstValueAt(enttIdx) 
								== EActivationStateForOptimization::ActiveBeforeDeactivating)
							{
								m_EntitiesToActivate.Add(*currentEntityHandle);
								++m_Entities_To_NumberOfTimesActivated.ValueAt(*currentEntityHandle);
							}
						}
					}
				}
				else
				{
					if (m_OptimizationStates.At(archIdx, compIdx).Active)
					{
						m_OptimizationStates.At(archIdx, compIdx).Active = false;

						const UEntityHandle* currentEntityHandle{ nullptr };

						for (size_t enttIdx = 0; enttIdx < m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.NumOfEntities(); enttIdx++)
						{
							currentEntityHandle = &m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.ConstEntityAt(enttIdx);

							if (m_Registry.IsActivated(currentEntityHandle->GetConstEntity()))
							{
								m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.ValueAt(enttIdx) = EActivationStateForOptimization::ActiveBeforeDeactivating;
							}
							else
							{
								m_OptimizationStates.At(archIdx, compIdx).EntitiesInBounds_To_ActivationStateForOptimization.ValueAt(enttIdx) = EActivationStateForOptimization::UnactiveBeforeDeactivating;
							}
							m_EntitiesToDeactivate.Add(*currentEntityHandle);
							
							int& numberOfTimesActivated{ m_Entities_To_NumberOfTimesActivated.ValueAt(*currentEntityHandle) };
							
							if (numberOfTimesActivated > 0)
							{
								--numberOfTimesActivated;
							}
						}
					}
				}
			}
		}



		for (int enttIdx = m_EntitiesToDeactivate.NumOfEntities() - 1; enttIdx >= 0 ; --enttIdx)
		{
			if (!m_EntitiesToActivate.Contains(m_EntitiesToDeactivate.ConstEntityAt(enttIdx)) &&
				(m_Entities_To_NumberOfTimesActivated.ConstValueAt(m_EntitiesToDeactivate.ConstEntityAt(enttIdx)) == 0))
			{
				m_Registry.DeactivateEntity(m_EntitiesToDeactivate.ConstEntityAt(enttIdx).GetConstEntity());
			}
		}

		for (int enttIdx = 0; enttIdx < m_EntitiesToActivate.NumOfEntities(); enttIdx++)
		{
			m_Registry.ActivateEntity(m_EntitiesToActivate.ConstEntityAt(enttIdx).GetConstEntity());
		}
	}

	UOptimizationSystem::UOptimizationSystem(URegistry& registry):
		m_CalculateOptimizationRectCollision(registry),
		m_OptimizationBoxSystem(registry){}

	void UOptimizationSystem::Initialize()
	{
		m_CalculateOptimizationRectCollision.Initialize();
		m_OptimizationBoxSystem.Initialize();
	}

	void UOptimizationSystem::Update()
	{
		m_CalculateOptimizationRectCollision.Update();
		m_OptimizationBoxSystem.Update();
	}

}