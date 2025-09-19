#pragma once

#include "../../../include/Structs/FLevelPartition.h"
#include "../../../include/Structs/FMesh.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FObjectCollision.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Systems/TemplateSystems/TActivatableVector.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {

	/*
		Revise the code afterwards to see if it is sound
		- remember to apply filters even if the entities are supposed to have the same components

	*/


	class UCreateLevelPartition {
	public:
		UCreateLevelPartition(URegistry& registry);
		bool Create();

	private:

		URegistry& m_Registry;
		
		UComponentIterator<FParams_Grids>	 m_LevelPartition_Params;
		UComponentIterator<TActivatableVector<FGridCell>> m_LevelPartitions; 
		UComponentIterator<TActivatableVector<FObjectCollision>> m_LevelPartitionsCollisionInformation;

	};

	class UClassifyObjectsIntoLevelPartition final{
	public:
		UClassifyObjectsIntoLevelPartition(URegistry& registry);
		void Update();

	private:

		URegistry& m_Registry;
		UEntityHandle m_EntityHandleForOperations;
		
		UComponentIterator<FParams_Grids> m_LevelPartition_Params;

		UComponentIterator<TActivatableVector<FObjectCollision>> m_LevelPartitionsCollisionInformation;
		UComponentIterator<FObjectMaxCollisionRadiusPerGrid> m_ObjectsMaxCollisionRadiusPerGrid;
		UComponentIterator<URealTransform2D> m_RealTransforms;

	};

	class UExecuteSolidCollision {

	public:
		UExecuteSolidCollision(URegistry& registry);
		void Debug_Update();
		void Update(float deltaSeconds);

	private:
		URegistry& m_Registry;

		UComponentIterator<FDetailedEntityResolveOverlapInfo> m_DetailedEntityCollisionInfo;
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityCollisionInfo;
		UComponentIterator<FWorldTransform2D> m_WorldTransforms2D;
		UComponentIterator<ECollisionBodyType> m_CollisionBodyTypes;
		UComponentIterator<FVelocity> m_Velocities;

		const float m_Max_Translation_Length_To_Smooth_Translation{3};

	};

}