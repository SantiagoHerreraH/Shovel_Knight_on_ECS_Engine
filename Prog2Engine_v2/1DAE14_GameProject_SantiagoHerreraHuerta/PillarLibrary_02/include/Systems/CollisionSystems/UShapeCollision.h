#pragma once

#include "../../../include/Systems/CollisionSystems/UCollision.h"
#include "../../../include/Structs/FSceneGraphStructure.h"

namespace Pillar {

	/*
		Revise the code afterwards to see if it is sound
		- remember to apply filters even if the entities are supposed to have the same components

	*/

	class UCalculateShapeBoundsForEachGrid {

	public:
		UCalculateShapeBoundsForEachGrid(URegistry& registry);

		void Calculate();

	private:

		URegistry& m_Registry;

		UComponentIterator<FParams_Grids>	 m_LevelPartition_Params;
		UComponentIterator<FObjectMaxCollisionRadiusPerGrid> m_ObjectsMaxCollisionRadiusPerGrid; // change this from a vector to something else that takes the entity into account -> some sort of object that you create along side
		UComponentIterator<FShape2Df> m_Shapes;
	};

	//there is an out of bounds error when I go out of bounds from the grid -> investigate
	class UCalculateShapeCollision {

	public:
		UCalculateShapeCollision(URegistry& registry);
		void Debug_Update();
		void Update();

	private:
		void ResetShapeOverlapInfo();
		void ResetObjectCollisionInfo(); 
		void ResetObjectDetailedCollisionInfo();

		URegistry& m_Registry;
		URegistrySingleton<FSceneRegistry>				m_SceneNodes;
		UComponentIterator<TActivatableVector<FObjectCollision>>	m_LevelPartitionsCollisionInformation;
		UComponentIterator<FEntityResolveOverlapInfo>				m_ShapeOverlapInfos;
		UComponentIterator<FDetailedEntityResolveOverlapInfo>		m_DetailedShapeOverlapInfos;
		UComponentIterator<FObjectCollision>						m_ObjectCollisions;
	};

	class UShapeCollision {

	public:
		UShapeCollision(URegistry& registry);
		void Initialize();
		void Update(float deltaSeconds);

	private:
		UCreateLevelPartition m_CreateLevelPartition;
		UCalculateShapeBoundsForEachGrid m_CalculateObjectBounds;
		UClassifyObjectsIntoLevelPartition m_ClassifyObjectsIntoLevelPartition;
		UCalculateShapeCollision m_CalculateShapeCollision;
		UExecuteSolidCollision m_ExecuteSolidCollision;
	};
}