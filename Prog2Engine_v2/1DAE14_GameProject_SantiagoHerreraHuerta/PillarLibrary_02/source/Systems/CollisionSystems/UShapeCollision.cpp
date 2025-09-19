
#include <cmath>

#include "../../../include/Systems/CollisionSystems/UShapeCollision.h"
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Objects/FCoordinateHasher.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"

namespace Pillar {

	/*
	
	*/

#pragma region CalculateObjectBounds

	UCalculateShapeBoundsForEachGrid::UCalculateShapeBoundsForEachGrid(URegistry& registry) :
		m_Registry(registry)
	{
		m_Registry.GetComponents(m_LevelPartition_Params);

		FObjectMaxCollisionRadiusPerGrid gridsCollisionRadius;
		FShape2Df shape;
		FLocalTransform2D transform;

		FMesh2Df mesh{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(gridsCollisionRadius, shape, transform);
		filter.SetExcludingFilter(mesh);

		registry.GetComponents(m_ObjectsMaxCollisionRadiusPerGrid, filter);
		registry.GetComponents(m_Shapes, filter);
	}
	void UCalculateShapeBoundsForEachGrid::Calculate()
	{
		FParams_Grids& gridParams{ m_LevelPartition_Params .At(0,0)};

		FPoint2f currentVertex;
		FPoint2f currentPosition;
		int length_Of_Radius_In_Number_Of_Grid_Cells_In_Current_Grid;
		float lowest_Between_Grid_Cell_Width_And_Height;

		for (size_t gridIdx = 0; gridIdx < gridParams.LevelPartitionParams.size(); gridIdx++)
		{
			lowest_Between_Grid_Cell_Width_And_Height =
				gridParams.LevelPartitionParams[gridIdx].BoundaryHeight <
				gridParams.LevelPartitionParams[gridIdx].BoundaryWidth ?
				gridParams.LevelPartitionParams[gridIdx].BoundaryHeight :
				gridParams.LevelPartitionParams[gridIdx].BoundaryHeight;

			for (size_t archIdx = 0; archIdx < m_Shapes.NumOfArchetypes(); archIdx++)
			{
				for (size_t compIdx = 0; compIdx < m_Shapes.NumOfActivatedComponents(archIdx); compIdx++)
				{
					m_ObjectsMaxCollisionRadiusPerGrid.At(archIdx, compIdx).Radiuses.reserve(gridParams.LevelPartitionParams.size());
					m_ObjectsMaxCollisionRadiusPerGrid.At(archIdx, compIdx).Radiuses.emplace_back();

					for (size_t vrtxIdx = 0; vrtxIdx < m_Shapes.At(archIdx, compIdx).Shape.size(); vrtxIdx++)
					{
						currentVertex = m_Shapes.At(archIdx, compIdx).Shape.at(vrtxIdx);
						currentPosition = m_Shapes.At(archIdx, compIdx).Center; //make sure this is correct
						length_Of_Radius_In_Number_Of_Grid_Cells_In_Current_Grid =
							std::ceil(SVectors::Distance(currentVertex, currentPosition) / lowest_Between_Grid_Cell_Width_And_Height);

						if (length_Of_Radius_In_Number_Of_Grid_Cells_In_Current_Grid > m_ObjectsMaxCollisionRadiusPerGrid.At(archIdx, compIdx).Radiuses[gridIdx])
						{
							m_ObjectsMaxCollisionRadiusPerGrid.At(archIdx, compIdx).Radiuses[gridIdx] = length_Of_Radius_In_Number_Of_Grid_Cells_In_Current_Grid;
						}

						
					}
				}
			}
		}

	}

#pragma endregion

#pragma region Calculate Shape Collision

	UCalculateShapeCollision::UCalculateShapeCollision(URegistry& registry) :
		m_Registry(registry),
		m_SceneNodes(registry)
	{
		m_Registry.GetComponents(m_LevelPartitionsCollisionInformation);
		m_Registry.GetComponents(m_ShapeOverlapInfos);
		m_Registry.GetComponents(m_ObjectCollisions);
		m_Registry.GetComponents(m_DetailedShapeOverlapInfos);
	}

	void UCalculateShapeCollision::ResetShapeOverlapInfo()
	{
		for (size_t archIdx = 0; archIdx < m_ShapeOverlapInfos.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ShapeOverlapInfos.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_ShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.Clear();
			}
		}
	}

	void UCalculateShapeCollision::ResetObjectCollisionInfo()
	{
		for (size_t archIdx = 0; archIdx < m_ObjectCollisions.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ObjectCollisions.NumOfComponents(archIdx); compIdx++)
			{
				m_ObjectCollisions.At(archIdx, compIdx).EntitiesCollidedWith.Clear();
			}
		}
	}

	void UCalculateShapeCollision::ResetObjectDetailedCollisionInfo()
	{
		m_DetailedShapeOverlapInfos;

		for (size_t archIdx = 0; archIdx < m_DetailedShapeOverlapInfos.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_DetailedShapeOverlapInfos.NumOfComponents(archIdx); compIdx++)
			{
				m_DetailedShapeOverlapInfos.At(archIdx, compIdx).Entities_To_ShapeOverlapInfo.Clear();
			}
		}
	}

	void UCalculateShapeCollision::Debug_Update()
	{
		//optimize this -> requires doing componentHandles (like componentContainer but with single components)

		ResetShapeOverlapInfo();
		ResetObjectDetailedCollisionInfo();

		m_Registry.Lock<FShape2Df>();
		m_Registry.Lock<FObjectCollision>();
		m_Registry.Lock<FDetailedEntityResolveOverlapInfo>();

		FShape2Df* firstShape{nullptr};
		FShape2Df* secondShape{nullptr};

		FObjectCollision* firstObjectCollision{nullptr};
		FObjectCollision* secondObjectCollision{ nullptr };

		FDetailedEntityResolveOverlapInfo* firstResolveOverlapInfo{ nullptr };
		FDetailedEntityResolveOverlapInfo* secondResolveOverlapInfo{ nullptr };

		FEdgesAndVerticesOverlapInfo firstOverlapInfo{};
		FEdgesAndVerticesOverlapInfo secondOverlapInfo{};

		// force * seconds = mass * velocity

		//for each grid -> only has one archetype
		for (size_t gridIdx = 0; gridIdx < m_LevelPartitionsCollisionInformation.NumOfActivatedComponents(0); gridIdx++)
		{
			//for each cell
			for (size_t cellIdx = 0; cellIdx < m_LevelPartitionsCollisionInformation.At(0, gridIdx).NumOfActivatedComponents(); cellIdx++)
			{
				size_t numOfEntitiesThatCollidedWithCell = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.NumOfEntities();
				numOfEntitiesThatCollidedWithCell = numOfEntitiesThatCollidedWithCell <= 0 ? 0 : numOfEntitiesThatCollidedWithCell - 1; // because of the last index

				//for each entity collided with cell
				for (size_t entityIdxInCell = 0; entityIdxInCell < numOfEntitiesThatCollidedWithCell; entityIdxInCell++)
				{
					const UEntityHandle& firstEntityHandle = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.ConstEntityAt(entityIdxInCell);

					firstShape				= m_Registry.GetComponent<FShape2Df>(firstEntityHandle.GetConstEntity());
					firstObjectCollision	= m_Registry.GetComponent<FObjectCollision>(firstEntityHandle.GetConstEntity());
					firstResolveOverlapInfo = m_Registry.GetComponent<FDetailedEntityResolveOverlapInfo>(firstEntityHandle.GetConstEntity());

					//for each entity collided with cell
					for (size_t otherEntityIdxInCell = (entityIdxInCell + 1); otherEntityIdxInCell < m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.NumOfEntities(); otherEntityIdxInCell++)
					{
						const UEntityHandle& secondEntityHandle = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.ConstEntityAt(otherEntityIdxInCell);

						//if same scene object hierarchy
						if (m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(firstEntityHandle) == 
							m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(secondEntityHandle))
						{
							continue;
						}

						if (!firstObjectCollision->EntitiesCollidedWith.Contains(secondEntityHandle))
						{
							secondShape				= m_Registry.GetComponent<FShape2Df>(secondEntityHandle.GetConstEntity());
							secondObjectCollision	= m_Registry.GetComponent<FObjectCollision>(secondEntityHandle.GetConstEntity());
							secondResolveOverlapInfo = m_Registry.GetComponent<FDetailedEntityResolveOverlapInfo>(secondEntityHandle.GetConstEntity());

							if (SCollision::IsOverlapping(*firstShape, *secondShape, firstOverlapInfo, secondOverlapInfo, true))
							{
								firstObjectCollision ->EntitiesCollidedWith.Add(secondEntityHandle);
								secondObjectCollision->EntitiesCollidedWith.Add(firstEntityHandle);
								firstResolveOverlapInfo->Entities_To_ShapeOverlapInfo.AddOrSet(secondEntityHandle, firstOverlapInfo);
								secondResolveOverlapInfo->Entities_To_ShapeOverlapInfo.AddOrSet(firstEntityHandle, secondOverlapInfo);
							}
							else
							{
								firstObjectCollision ->EntitiesCollidedWith.SwapRemove(secondEntityHandle);
								secondObjectCollision->EntitiesCollidedWith.SwapRemove(firstEntityHandle);
								firstResolveOverlapInfo->Entities_To_ShapeOverlapInfo.SwapRemove(secondEntityHandle);
								secondResolveOverlapInfo->Entities_To_ShapeOverlapInfo.SwapRemove(firstEntityHandle);
							}
						}


					}

				}
			}
		}

		m_Registry.Unlock<FShape2Df>();
		m_Registry.Unlock<FObjectCollision>();
		m_Registry.Unlock<FDetailedEntityResolveOverlapInfo>();
	}

	void UCalculateShapeCollision::Update()
	{
		//optimize this -> requires doing componentHandles (like componentContainer but with single components)

		ResetShapeOverlapInfo();
		ResetObjectCollisionInfo();

		m_Registry.Lock<FShape2Df>();
		m_Registry.Lock<FObjectCollision>();
		m_Registry.Lock<FEntityResolveOverlapInfo>();

		FShape2Df* firstShape{ nullptr };
		FShape2Df* secondShape{ nullptr };

		FObjectCollision* firstObjectCollision{ nullptr };
		FObjectCollision* secondObjectCollision{ nullptr };

		FEntityResolveOverlapInfo* firstOverlapInfo{ nullptr };
		FEntityResolveOverlapInfo* secondOverlapInfo{ nullptr };

		FResolveOverlapInfo firstResolveOverlapInfo{};
		FResolveOverlapInfo secondResolveOverlapInfo{};

		// force * seconds = mass * velocity

		//for each grid -> only has one archetype
		for (size_t gridIdx = 0; gridIdx < m_LevelPartitionsCollisionInformation.NumOfActivatedComponents(0); gridIdx++)
		{
			//for each cell
			for (size_t cellIdx = 0; cellIdx < m_LevelPartitionsCollisionInformation.At(0, gridIdx).NumOfActivatedComponents(); cellIdx++)
			{
				size_t numOfEntitiesThatCollidedWithCell = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.NumOfEntities();
				numOfEntitiesThatCollidedWithCell = numOfEntitiesThatCollidedWithCell <= 0 ? 0 : numOfEntitiesThatCollidedWithCell - 1; // because of the last index

				//for each entity collided with cell
				for (size_t entityIdxInCell = 0; entityIdxInCell < numOfEntitiesThatCollidedWithCell; entityIdxInCell++)
				{
					const UEntityHandle& firstEntityHandle = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.ConstEntityAt(entityIdxInCell);

					if (!m_Registry.IsActivated(firstEntityHandle.GetConstEntity()))
					{
						continue;
					}

					firstShape = m_Registry.GetComponent<FShape2Df>(firstEntityHandle.GetConstEntity());
					firstObjectCollision = m_Registry.GetComponent<FObjectCollision>(firstEntityHandle.GetConstEntity());
					firstOverlapInfo = m_Registry.GetComponent<FEntityResolveOverlapInfo>(firstEntityHandle.GetConstEntity());

					//for each entity collided with cell
					for (size_t otherEntityIdxInCell = (entityIdxInCell + 1); otherEntityIdxInCell < m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.NumOfEntities(); otherEntityIdxInCell++)
					{
						const UEntityHandle& secondEntityHandle = m_LevelPartitionsCollisionInformation.At(0, gridIdx).At(cellIdx).EntitiesCollidedWith.ConstEntityAt(otherEntityIdxInCell);

						if (!m_Registry.IsActivated(secondEntityHandle.GetConstEntity()))
						{
							continue;
						}

						//if same scene object hierarchy
						if (m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(firstEntityHandle) ==
							m_SceneNodes.GetConst().Entity_To_EntitySceneHierarchyIndex.ConstValueAt(secondEntityHandle))
						{
							continue;
						}

						if (!firstObjectCollision->EntitiesCollidedWith.Contains(secondEntityHandle))
						{
							secondShape = m_Registry.GetComponent<FShape2Df>(secondEntityHandle.GetConstEntity());
							secondObjectCollision = m_Registry.GetComponent<FObjectCollision>(secondEntityHandle.GetConstEntity());
							secondOverlapInfo = m_Registry.GetComponent<FEntityResolveOverlapInfo>(secondEntityHandle.GetConstEntity());

							if (SCollision::IsOverlapping(*firstShape, *secondShape, firstResolveOverlapInfo, secondResolveOverlapInfo))
							{
								firstObjectCollision->EntitiesCollidedWith.Add(secondEntityHandle);
								secondObjectCollision->EntitiesCollidedWith.Add(firstEntityHandle);
								firstOverlapInfo->Entities_To_ResolveOverlapInfo.AddOrSet(secondEntityHandle, firstResolveOverlapInfo);
								secondOverlapInfo->Entities_To_ResolveOverlapInfo.AddOrSet(firstEntityHandle, secondResolveOverlapInfo);
							}
							else
							{
								firstObjectCollision->EntitiesCollidedWith.SwapRemove(secondEntityHandle);
								secondObjectCollision->EntitiesCollidedWith.SwapRemove(firstEntityHandle);
								firstOverlapInfo->Entities_To_ResolveOverlapInfo.SwapRemove(secondEntityHandle);
								secondOverlapInfo->Entities_To_ResolveOverlapInfo.SwapRemove(firstEntityHandle);
							}
						}


					}

				}
			}
		}

		m_Registry.Unlock<FShape2Df>();
		m_Registry.Unlock<FObjectCollision>();
		m_Registry.Unlock<FEntityResolveOverlapInfo>();
	}

#pragma endregion

#pragma region Collision

	UShapeCollision::UShapeCollision(URegistry& registry) :
		m_CreateLevelPartition{ registry },
		m_CalculateObjectBounds{ registry },
		m_ClassifyObjectsIntoLevelPartition{ registry },
		m_CalculateShapeCollision{ registry },
		m_ExecuteSolidCollision(registry)
	{


	}

	void UShapeCollision::Initialize()
	{
		m_CreateLevelPartition.Create();
		m_CalculateObjectBounds.Calculate(); //has to be called after mesh creation
	}

	void UShapeCollision::Update(float deltaSeconds)
	{
		m_ClassifyObjectsIntoLevelPartition.Update();
		m_CalculateShapeCollision.Update();
		m_ExecuteSolidCollision.Update(deltaSeconds);
	}


#pragma endregion


}