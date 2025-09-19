
#include "../../../include/Systems/CollisionSystems/URectCollision.h"
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Structs/FOptimization.h"

namespace Pillar {

	UCalculateMovableRectCollision::UCalculateMovableRectCollision(URegistry& registry) :
		m_Registry(registry),
		m_SceneNodes(registry)
	{

		UComponentFilter filter{};
		filter.SetIncludingFilter(FRectCollider{}, FEntityResolveOverlapInfo{}, ECollisionBodyType{});
		filter.SetExcludingFilter(FStaticColliderFlag{}, FOptimizationGridCellState{});

		registry.GetComponents(m_RectColliders, filter);
		registry.GetComponents(m_ShapeOverlapInfos, filter);
		registry.GetComponents(m_CollisionBodyTypes, filter);
	}

	void UCalculateMovableRectCollision::Update()
	{
		ResetShapeOverlapInfo();

		UEntityHandle firstEntityHandle{};
		UEntityHandle secondEntityHandle{};

		FResolveOverlapInfo firstResolveOverlapInfo{};
		FResolveOverlapInfo secondResolveOverlapInfo{};

		for (size_t archIdx = 0; archIdx < m_RectColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_RectColliders.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_Registry.GetEntity<FEntityResolveOverlapInfo>(firstEntityHandle, m_RectColliders.GetArchetype(archIdx), compIdx);

				if (m_CollisionBodyTypes.At(archIdx, compIdx) == ECollisionBodyType::IgnoreOnlyMovable || 
					m_CollisionBodyTypes.At(archIdx, compIdx) == ECollisionBodyType::IgnoreAll)
				{
					continue;
				}

				for (size_t otherArchIdx = archIdx; otherArchIdx < m_RectColliders.NumOfArchetypes(); otherArchIdx++)
				{
					//this has to start from 0 because each arch component doesn't have the same size
					for (size_t otherCompIdx = 0; otherCompIdx < m_RectColliders.NumOfActivatedComponents(otherArchIdx); otherCompIdx++)
					{
						if (archIdx == otherArchIdx && compIdx == otherCompIdx)
						{
							continue;
						}

						if (m_CollisionBodyTypes.At(otherArchIdx, otherCompIdx) == ECollisionBodyType::IgnoreOnlyMovable)
						{
							continue;
						}

						m_Registry.GetEntity<FEntityResolveOverlapInfo>(secondEntityHandle, m_RectColliders.GetArchetype(otherArchIdx), otherCompIdx);

						//if same scene object hierarchy
						if (m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.Contains(firstEntityHandle) &&
							m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.Contains(secondEntityHandle) &&
							m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(firstEntityHandle) ==
							m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(secondEntityHandle))
						{
							continue;
						}

						if (m_ShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Contains(secondEntityHandle))
						{
							continue;
						}

						firstResolveOverlapInfo.OtherIsOptimized = m_RectColliders.At(otherArchIdx, otherCompIdx).Optimize;
						secondResolveOverlapInfo.OtherIsOptimized = m_RectColliders.At(archIdx, compIdx).Optimize;

						if (SCollision::IsOverlapping(
							m_RectColliders.At(archIdx, compIdx).CurrentRect,
							m_RectColliders.At(otherArchIdx, otherCompIdx).CurrentRect,
							firstResolveOverlapInfo,
							secondResolveOverlapInfo))
						{
							
							m_ShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.AddOrSet(secondEntityHandle, firstResolveOverlapInfo);
							m_ShapeOverlapInfos.At(otherArchIdx, otherCompIdx).Entities_To_ResolveOverlapInfo.AddOrSet(firstEntityHandle, secondResolveOverlapInfo);
						}
						else
						{
							m_ShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.SwapRemove(secondEntityHandle);
							m_ShapeOverlapInfos.At(otherArchIdx, otherCompIdx).Entities_To_ResolveOverlapInfo.SwapRemove(firstEntityHandle);
						}
					}
				}
			}
		}
	}

	void UCalculateMovableRectCollision::ResetShapeOverlapInfo()
	{
		for (size_t archIdx = 0; archIdx < m_ShapeOverlapInfos.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ShapeOverlapInfos.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_ShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Clear();
			}
		}
	}

	URectCollision::URectCollision(URegistry& registry) :
		m_CalculateMovableRectCollision(registry),
		m_ExecuteSolidCollision(registry),
		m_CalculateStaticRectCollision(registry){}

	void URectCollision::Update(float deltaSeconds)
	{
		m_CalculateMovableRectCollision.Update();
		m_CalculateStaticRectCollision.Update();
		m_ExecuteSolidCollision.Update(deltaSeconds);
	}

	UCalculateStaticRectCollision::UCalculateStaticRectCollision(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FRectCollider{}, FEntityResolveOverlapInfo{}, ECollisionBodyType{}, FStaticColliderFlag{});
		filter.SetExcludingFilter(FOptimizationGridCellState{});

		registry.GetComponents(m_StaticRectColliders, filter);
		registry.GetComponents(m_StaticShapeOverlapInfos, filter);


		filter.SetIncludingFilter(FRectCollider{}, FEntityResolveOverlapInfo{}, ECollisionBodyType{});
		filter.SetExcludingFilter(FStaticColliderFlag{}, FOptimizationGridCellState{});

		registry.GetComponents(m_MovableShapeOverlapInfos, filter);
		registry.GetComponents(m_MovableRectColliders, filter);
		registry.GetComponents(m_MovableCollisionBodyTypes, filter);

	}
	void UCalculateStaticRectCollision::Update()
	{
		UEntityHandle firstEntityHandle{};
		UEntityHandle secondEntityHandle{};

		FResolveOverlapInfo firstResolveOverlapInfo{};
		FResolveOverlapInfo secondResolveOverlapInfo{};

		for (size_t archIdx = 0; archIdx < m_MovableRectColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_MovableRectColliders.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_Registry.GetEntity<FEntityResolveOverlapInfo>(firstEntityHandle, m_MovableRectColliders.GetArchetype(archIdx), compIdx);

				if (m_MovableCollisionBodyTypes.At(archIdx, compIdx) == ECollisionBodyType::IgnoreAll)
				{
					continue;
				}

				for (size_t otherArchIdx = 0; otherArchIdx < m_StaticRectColliders.NumOfArchetypes(); otherArchIdx++)
				{
					//this has to start from 0 because each arch component doesn't have the same size
					for (size_t otherCompIdx = 0; otherCompIdx < m_StaticRectColliders.NumOfActivatedComponents(otherArchIdx); otherCompIdx++)
					{
						m_Registry.GetEntity<FEntityResolveOverlapInfo>(secondEntityHandle, m_StaticRectColliders.GetArchetype(otherArchIdx), otherCompIdx);

						if (m_MovableShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Contains(secondEntityHandle))
						{
							continue;
						}

						firstResolveOverlapInfo.OtherIsOptimized = m_StaticRectColliders.At(otherArchIdx, otherCompIdx).Optimize;
						secondResolveOverlapInfo.OtherIsOptimized = m_MovableRectColliders.At(archIdx, compIdx).Optimize;

						if (SCollision::IsOverlapping(
							m_MovableRectColliders.At(archIdx, compIdx).CurrentRect,
							m_StaticRectColliders.At(otherArchIdx, otherCompIdx).CurrentRect,
							firstResolveOverlapInfo,
							secondResolveOverlapInfo))
						{
						
							m_MovableShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.AddOrSet(secondEntityHandle, firstResolveOverlapInfo);
							m_StaticShapeOverlapInfos.At(otherArchIdx, otherCompIdx).Entities_To_ResolveOverlapInfo.AddOrSet(firstEntityHandle, secondResolveOverlapInfo);
						}
						else
						{
							m_MovableShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.SwapRemove(secondEntityHandle);
							m_StaticShapeOverlapInfos.At(otherArchIdx, otherCompIdx).Entities_To_ResolveOverlapInfo.SwapRemove(firstEntityHandle);
						}
					}
				}
			}
		}
	}
	
}