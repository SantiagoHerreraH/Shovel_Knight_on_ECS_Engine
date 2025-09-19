#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FOptimization.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Structs/FSceneGraphStructure.h"
#include "../../../include/Systems/CollisionSystems/URectCollision.h"

namespace Pillar {

	class UCalculateOptimizationRectCollision {
	public:
		UCalculateOptimizationRectCollision(URegistry& registry);
		void Initialize();
		void Update();

	private:

		URegistry& m_Registry;
		UEntityHandle m_PlayerEntityHandle;

		UComponentIterator<FSceneObject>								m_AllSceneObjects;
		UComponentIterator<FSceneObject>								m_OptimizationSceneObjects;

		UComponentIterator<FRectCollider>								m_AllRectColliders;
		UComponentIterator<FRectCollider>								m_PlayerRectCollider;

		UComponentIterator<FRectCollider>								m_OptimizationRectColliders;
		UComponentIterator<FEntityResolveOverlapInfo>					m_OptimizationShapeOverlapInfos;
	};

	class UOptimizationBoxSystem {

	public:
		UOptimizationBoxSystem(URegistry& registry);
		void Initialize();
		void Update();

	private:

		URegistry& m_Registry;
		UEntityHandle m_PlayerEntityHandle;
		UComponentIterator<FEntityResolveOverlapInfo> m_OptimizationOverlapInfos;
		UComponentIterator<FOptimizationGridCellState> m_OptimizationStates;

		UEntityHandleMap<int> m_Entities_To_NumberOfTimesActivated;//0 equals deactivation

		UEntityHandleContainer m_EntitiesToActivate;
		UEntityHandleContainer m_EntitiesToDeactivate;
	};

	class UOptimizationSystem {
	public:
		UOptimizationSystem(URegistry& registry);
		void Initialize();
		void Update();

	private:

		UCalculateOptimizationRectCollision m_CalculateOptimizationRectCollision;
		UOptimizationBoxSystem m_OptimizationBoxSystem;
	};
	
}