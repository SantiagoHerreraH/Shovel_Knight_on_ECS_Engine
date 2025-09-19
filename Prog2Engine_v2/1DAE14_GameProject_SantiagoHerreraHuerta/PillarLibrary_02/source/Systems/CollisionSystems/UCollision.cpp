
#include <cmath>

#include "../../../include/Systems/CollisionSystems/UCollision.h"
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FWindow.h"
#include "../../../include/Objects/FCoordinateHasher.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/OptimizationSystems/UOptimization.h"

namespace Pillar {

#pragma region CreateLevelPartition

	UCreateLevelPartition::UCreateLevelPartition(URegistry& registry) :
		m_Registry(registry)
	{
		m_Registry.GetComponents(m_LevelPartition_Params);
	}

	bool UCreateLevelPartition::Create()
	{
		m_Registry.GetComponents(m_LevelPartitions); 
		m_Registry.GetComponents(m_LevelPartitionsCollisionInformation);

		FParams_Grids& gridParams = m_LevelPartition_Params.At(0,0);

		if (gridParams.LevelPartitionParams.empty())
		{
			gridParams.LevelPartitionParams.emplace_back();
		}

		std::vector<FEntity> entities{};
		TActivatableVector<FGridCell> grid{};
		TActivatableVector<FObjectCollision> gridCollInfo{};
		size_t numberOfGrids{ gridParams.LevelPartitionParams.size() };

		if (m_LevelPartitions.IsEmpty())
		{
			m_Registry.CreateEntities(entities, numberOfGrids, grid, gridCollInfo);
		}
		else if (
			m_LevelPartitions.TotalNumOfComponents() != numberOfGrids || 
			m_LevelPartitionsCollisionInformation.TotalNumOfComponents() != numberOfGrids)
		{
			m_Registry.ClearComponentsOfType<TActivatableVector<FGridCell>>();
			m_Registry.ClearComponentsOfType<TActivatableVector<FObjectCollision>>();
			m_Registry.CreateEntities(entities, numberOfGrids, grid, gridCollInfo);
		}

		int smallestArea{ int(INFINITY) };
		int biggestCellNumber{ int(-INFINITY)};
		size_t collisionGridIndex{};
		const FParams_Grid* params{ nullptr };

		for (size_t paramIdx = 0; paramIdx < gridParams.LevelPartitionParams.size(); paramIdx++)
		{
			params = &gridParams.LevelPartitionParams[paramIdx];

			FGridCell boundary{};
			
			boundary.Boundary.Height = gridParams.LevelPartitionParams[paramIdx].BoundaryHeight;
			boundary.Boundary.Width = gridParams.LevelPartitionParams[paramIdx].BoundaryWidth;

			FPoint2f bottomLeftCorner{};

			FCoordinateHasher coordinateHasher{ gridParams.LevelPartitionParams[paramIdx] };

			size_t maxNumOfRows{ gridParams.LevelPartitionParams[paramIdx].NumberOfRows };

			for (size_t currentRow = 0; currentRow < maxNumOfRows; currentRow++)
			{
				for (size_t currentColumn = 0; currentColumn < gridParams.LevelPartitionParams[paramIdx].NumberOfColumns; currentColumn++)
				{
					boundary.Boundary.Left = bottomLeftCorner.X + (boundary.Boundary.Width * currentColumn);
					boundary.Boundary.Top = bottomLeftCorner.Y + boundary.Boundary.Height + (boundary.Boundary.Height * currentRow);
					boundary.Center = FPoint2f{ boundary.Boundary.Left + (boundary.Boundary.Width / 2.f), boundary.Boundary.Top - (boundary.Boundary.Height / 2.f) };
					boundary.Coordinate_Index = (currentRow * maxNumOfRows) + currentColumn;
					boundary.LevelPartition_Index = paramIdx;

					m_LevelPartitions.At(0, paramIdx).Add(boundary);
					m_LevelPartitionsCollisionInformation.At(0, paramIdx).Add(FObjectCollision{});
				}
			}

			if ((params->BoundaryHeight * params->BoundaryWidth) < smallestArea &&
				(params->NumberOfColumns * params->NumberOfRows) > biggestCellNumber)
			{
				collisionGridIndex = paramIdx;
			}
		}

		m_Registry.GetEntity<TActivatableVector<FGridCell>>(
			gridParams.CollisionGridEntityHandle, entities[collisionGridIndex].Archetype_Id, collisionGridIndex );

		m_Registry.LockActivationAndDeactivationOfComponentType<TActivatableVector<FGridCell>>();
		m_Registry.LockActivationAndDeactivationOfComponentType<TActivatableVector<FObjectCollision>>();

		return true;
	}

#pragma endregion

#pragma region Classify Shapes Into Level Partition

	UClassifyObjectsIntoLevelPartition::UClassifyObjectsIntoLevelPartition(URegistry& registry) :
		m_Registry(registry)
	{
		m_Registry.GetComponents(m_LevelPartitionsCollisionInformation);
		m_Registry.GetComponents(m_LevelPartition_Params);

		FObjectMaxCollisionRadiusPerGrid collRadius{};
		URealTransform2D worldTransform{};
		UComponentFilter filter{};
		filter.SetIncludingFilter(collRadius, worldTransform);

		m_Registry.GetComponents(m_RealTransforms, filter);
		m_Registry.GetComponents(m_ObjectsMaxCollisionRadiusPerGrid, filter);
	}

	void UClassifyObjectsIntoLevelPartition::Update()
	{

		int currentObjectRadius{};
		size_t centerCellIndex{};
		size_t startingCellIndex{};
		size_t numOfRows_InCollisionArea{};
		size_t numOfColumns_InCollisionArea{};
		size_t maxGridIndex{};

		FParams_Grids& gridParams{ m_LevelPartition_Params.At(0,0)};

		for (size_t gridIdx = 0; gridIdx < gridParams.LevelPartitionParams.size(); gridIdx++)
		{
			const FParams_Grid& currentGrid = gridParams.LevelPartitionParams[gridIdx];

			//first clear the cells (you can do it differently later)
			for (size_t cellIdx = 0; cellIdx < m_LevelPartitionsCollisionInformation.At(0, gridIdx).NumOfActivatedComponents(); cellIdx++)
			{
				m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.Clear();
			}

			//this works if the grid order stays the same
			FCoordinateHasher hasher{ gridParams.LevelPartitionParams[gridIdx]}; //optimize this later

			for (size_t archIdx = 0; archIdx < m_RealTransforms.NumOfArchetypes(); archIdx++)
			{
				for (size_t compIdx = 0; compIdx < m_RealTransforms.NumOfComponents(archIdx); compIdx++)
				{
					centerCellIndex = hasher.Hash(m_RealTransforms.At(archIdx, compIdx).GetConstRealWorldTransform().Position);
					currentObjectRadius = m_ObjectsMaxCollisionRadiusPerGrid.At(archIdx, compIdx).Radiuses.at(gridIdx);

					numOfRows_InCollisionArea = (currentObjectRadius * 2) + 1;
					numOfColumns_InCollisionArea = (currentObjectRadius * 2) + 1;

					startingCellIndex = centerCellIndex - (currentGrid.NumberOfColumns * currentObjectRadius) - currentObjectRadius; // top left corner
					
					maxGridIndex = (currentGrid.NumberOfColumns * currentGrid.NumberOfRows) - 1;

					startingCellIndex = 
						startingCellIndex < 0 ? 0 :
						startingCellIndex >  maxGridIndex ? maxGridIndex : 
						startingCellIndex;

					m_Registry.GetEntity<FLocalTransform2D>(m_EntityHandleForOperations, m_RealTransforms.GetArchetype(archIdx), compIdx); //check if this works -> because you are constantly changing the handle value

					for (size_t currentRowIdx = 0; currentRowIdx < numOfRows_InCollisionArea; currentRowIdx++)
					{
						for (size_t currentColumnIdx = 0; currentColumnIdx < numOfColumns_InCollisionArea; currentColumnIdx++)
						{
							m_LevelPartitionsCollisionInformation.At(0, gridIdx).
								At((startingCellIndex + currentColumnIdx + (currentRowIdx * currentGrid.NumberOfColumns)) % maxGridIndex)
								.EntitiesCollidedWith.Add(m_EntityHandleForOperations);
						}
					}

				}
			}
			
			
		}
	}

#pragma endregion

#pragma region ExecuteSolidCollision

	UExecuteSolidCollision::UExecuteSolidCollision(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(
			FWorldTransform2D{},
			ECollisionBodyType{},
			FEntityResolveOverlapInfo{},
			FVelocity{});
		filter.SetExcludingFilter(FStaticColliderFlag{}, FOptimizationGridCellState{});

		m_Registry.GetComponents(m_WorldTransforms2D, filter);
		m_Registry.GetComponents(m_EntityCollisionInfo, filter);
		m_Registry.GetComponents(m_CollisionBodyTypes, filter);
		m_Registry.GetComponents(m_Velocities, filter);

		/*
			UNCOMMENT THIS IF YOU WANT DEBUG MODE
		*/
		//FDetailedEntityResolveOverlapInfo detailedEntityOverlapInfo{};
		//FHierarchyIndependentWorldTransform2D independentWorldtransform{};
		//EPhysicsBodyType physicsBodyType{};
		//
		//UComponentFilter filter{};
		//filter.SetIncludingFilter(independentWorldtransform, physicsBodyType, detailedEntityOverlapInfo);
		//
		//m_Registry.GetComponents(m_Transforms, filter);
		//m_Registry.GetComponents(m_DetailedEntityCollisionInfo, filter);
		//m_Registry.GetComponents(m_PhysicsBodyType, filter);
	}

	void UExecuteSolidCollision::Debug_Update()
	{
		FPoint2f currentTranslationAmount{ 0,0 };
		FPoint2f verticeVector{ 0,0 };
		FPoint2f edgeVector{ 0,0 };
		FPoint2f projectedVector{ 0,0 };

		FPoint2f finalTranslationAmount{ 0,0 };

		ECollisionBodyType otherCompPhysicsBodyType{};
		double percentageOfScalar{};
		float scalarToApply{};

		for (size_t archIdx = 0; archIdx < m_DetailedEntityCollisionInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_DetailedEntityCollisionInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t enttIdx = 0; enttIdx < m_DetailedEntityCollisionInfo.At(archIdx, compIdx).Entities_To_ShapeOverlapInfo.NumOfEntities(); enttIdx++)
				{
					if (m_CollisionBodyTypes.At(archIdx, compIdx) != ECollisionBodyType::Solid)
					{
						break;
					}

					const FDetailedEntityResolveOverlapInfo& info = m_DetailedEntityCollisionInfo.At(archIdx, compIdx);

					const UEntityHandle& otherEntityHandle = info.Entities_To_ShapeOverlapInfo.ConstEntityAt(enttIdx);
					const FEdgesAndVerticesOverlapInfo& overlapInfo = info.Entities_To_ShapeOverlapInfo.ConstValueAt(enttIdx);

					m_Registry.GetComponent<ECollisionBodyType>(otherCompPhysicsBodyType, otherEntityHandle);

					if (otherCompPhysicsBodyType == ECollisionBodyType::OverlapAll)
					{
						continue;
					}

					percentageOfScalar = 0.5;

					size_t chosenIndex{};
					float currentSmallestScalar{ INFINITY };
					bool thisMeshHasShortestScalar{ true };

					for (size_t overlappingVrtxtIdx = 0; overlappingVrtxtIdx < overlapInfo.OverlappingVectorsFromCenterToVertice.size(); overlappingVrtxtIdx++)
					{
						if (overlapInfo.OverlappingVectorsFromCenterToVertice[overlappingVrtxtIdx].Scalar < currentSmallestScalar)
						{
							currentSmallestScalar = overlapInfo.OverlappingVectorsFromCenterToVertice[overlappingVrtxtIdx].Scalar;
							chosenIndex = overlappingVrtxtIdx;
							thisMeshHasShortestScalar = true;
						}
					}

					for (size_t overlappingEdgeIdx = 0; overlappingEdgeIdx < overlapInfo.OverlappingEdges.size(); overlappingEdgeIdx++)
					{
						if (overlapInfo.OverlappingEdges[overlappingEdgeIdx].LineScalar < currentSmallestScalar)
						{
							currentSmallestScalar = overlapInfo.OverlappingEdges[overlappingEdgeIdx].LineScalar;
							chosenIndex = overlappingEdgeIdx;
							thisMeshHasShortestScalar = false;
						}
					}

					if (thisMeshHasShortestScalar && !overlapInfo.OverlappingVectorsFromCenterToVertice.empty())
					{

						scalarToApply = 1 - overlapInfo.OverlappingVectorsFromCenterToVertice[chosenIndex].Scalar;

						verticeVector = SVectors::Subtract(
							overlapInfo.OverlappingVectorsFromCenterToVertice[chosenIndex].EndPoint,
							overlapInfo.OverlappingVectorsFromCenterToVertice[chosenIndex].StartPoint);

						edgeVector = SVectors::Subtract(
							overlapInfo.OverlappingVectorsFromCenterToVertice[chosenIndex].EdgeThatOverlapped.End,
							overlapInfo.OverlappingVectorsFromCenterToVertice[chosenIndex].EdgeThatOverlapped.Start);

						currentTranslationAmount = SVectors::Scale(
							verticeVector,
							(scalarToApply) * -percentageOfScalar);

						projectedVector = SVectors::ProjectedVector(edgeVector, currentTranslationAmount);

						currentTranslationAmount.X -= projectedVector.X;
						currentTranslationAmount.Y -= projectedVector.Y;

						finalTranslationAmount.X += currentTranslationAmount.X;
						finalTranslationAmount.Y += currentTranslationAmount.Y;

					}
					else if (!thisMeshHasShortestScalar && !overlapInfo.OverlappingEdges.empty())
					{
						scalarToApply = 1 - overlapInfo.OverlappingEdges[chosenIndex].LineScalar;

						verticeVector = SVectors::Subtract(
							overlapInfo.OverlappingEdges[chosenIndex].LineThatOverlapped.End,
							overlapInfo.OverlappingEdges[chosenIndex].LineThatOverlapped.Start);

						edgeVector = SVectors::Subtract(
							overlapInfo.OverlappingEdges[chosenIndex].EndPoint,
							overlapInfo.OverlappingEdges[chosenIndex].StartPoint);

						currentTranslationAmount = SVectors::Scale(
							verticeVector,
							(scalarToApply)*percentageOfScalar);

						projectedVector = SVectors::ProjectedVector(edgeVector, currentTranslationAmount);

						currentTranslationAmount.X -= projectedVector.X;
						currentTranslationAmount.Y -= projectedVector.Y;

						finalTranslationAmount.X += currentTranslationAmount.X;
						finalTranslationAmount.Y += currentTranslationAmount.Y;

					}

					m_WorldTransforms2D.At(archIdx, compIdx).Transform.Position.X += finalTranslationAmount.X;
					m_WorldTransforms2D.At(archIdx, compIdx).Transform.Position.Y += finalTranslationAmount.Y;

					finalTranslationAmount = { 0,0 };
				}
			}
		}
	}

	void UExecuteSolidCollision::Update(float deltaSeconds)
	{
		ECollisionBodyType otherCompPhysicsBodyType{};
		double percentageOfScalar{};
		FPoint2f finalTranslationAmount{};
		FStaticColliderFlag* otherStaticColliderFlag{ nullptr };

		for (size_t archIdx = 0; archIdx < m_EntityCollisionInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EntityCollisionInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t enttIdx = 0; enttIdx < m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
				{
					if (m_CollisionBodyTypes.At(archIdx, compIdx) == ECollisionBodyType::OverlapAll)
					{
						break;
					}

					const FEntityResolveOverlapInfo& entityCollisionInfo = m_EntityCollisionInfo.At(archIdx, compIdx);

					const FResolveOverlapInfo& overlapInfo = entityCollisionInfo.Entities_To_ResolveOverlapInfo.ConstValueAt(enttIdx);
					const UEntityHandle& otherEntityHandle = entityCollisionInfo.Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx);

					m_Registry.GetComponent<ECollisionBodyType>(otherCompPhysicsBodyType, otherEntityHandle);
					otherStaticColliderFlag = m_Registry.GetComponent<FStaticColliderFlag>(otherEntityHandle.GetConstEntity());

					if (otherCompPhysicsBodyType != ECollisionBodyType::Solid || 
						(!otherStaticColliderFlag && 
						m_CollisionBodyTypes.At(archIdx, compIdx) == ECollisionBodyType::OverlapOnlyMovable))
					{
						continue;
					}

					finalTranslationAmount =
						SVectors::Length(overlapInfo.TranslationVector) < m_Max_Translation_Length_To_Smooth_Translation ?
						SVectors::Scale(overlapInfo.TranslationVector, deltaSeconds) :
						overlapInfo.TranslationVector;

					m_WorldTransforms2D.At(archIdx, compIdx).Transform.Position.X += finalTranslationAmount.X;
					m_WorldTransforms2D.At(archIdx, compIdx).Transform.Position.Y += finalTranslationAmount.Y;

				}
			}
		}
	}

#pragma endregion

}