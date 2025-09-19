#pragma once

#include <unordered_set>

#include "../../include/Structs/FShape.h"
#include "../../include/Core/UEntityHandleContainer.h"
#include "../../../../Engine/structs.h"

namespace Pillar {

	//FParams are parameters you set yourself

	enum class ECollisionBodyType {
		Solid,
		OverlapAll,
		OverlapOnlyMovable,
		IgnoreOnlyMovable,
		IgnoreAll
	};

	struct FLineOverlapInfo {

		bool isOverlapping{ false };
		float Line1Scalar;
		float Line2Scalar;
		FPoint2f IntersectionPoint;

	};

	struct FResolveOverlapInfo {
		FPoint2f TranslationVector;
		FPoint2f IntersectionPoint;
		FPoint2i DirectionOfCollided;
		bool isOverlapping{ false };
		bool OtherIsOptimized{ true };
	};

#pragma region Shape Overlap Info

	struct FOverlappingEdgeOfShape {
		size_t StartPointIndex;
		size_t EndPointIndex;

		FPoint2f IntersectionPoint;
		float Scalar;

		FLine2f LineThatOverlapped;
		float LineScalar;

		FPoint2f StartPoint;
		FPoint2f EndPoint;
	};

	struct FOverlappingLineOfShape {
		FPoint2f StartPoint;
		FPoint2f EndPoint;

		size_t EndPointIndex;

		FPoint2f IntersectionPoint;
		float Scalar;

		FLine2f EdgeThatOverlapped;
		float EdgeScalar;
	};

	//change the FShapeOverlapInfo out
	struct FEdgesAndVerticesOverlapInfo {
		std::vector<FOverlappingEdgeOfShape> OverlappingEdges;
		std::vector<FOverlappingLineOfShape> OverlappingVectorsFromCenterToVertice;
	};

#pragma endregion

#pragma region Mesh Overlap Info

	struct FMeshTriangle_OverlappingEdge_Info {

		FOverlappingEdgeOfShape OverlappedEdge;
		size_t TriangleIndex;

	};

	struct FMeshTriangle_OverlappingVertice_Info {

		FOverlappingLineOfShape OverlappingVectorFromCenterToVertice;
		size_t TriangleIndex;

	};

	struct FMeshEdgesAndVerticesOverlapInfo {
		std::vector<FMeshTriangle_OverlappingEdge_Info> TrianglesWithEdgesThatOverlap;
		std::vector<FMeshTriangle_OverlappingVertice_Info> TrianglesWithVerticesThatOverlap;
	};

	struct FOverlappedEntitiesAndMeshCollisionInfo {
		UEntityHandleMap<FMeshEdgesAndVerticesOverlapInfo> OverlappedEntities_To_MeshCollisionInfo;
	};

#pragma endregion

#pragma region Entity Overlap Info

	struct FEntityResolveOverlapInfo {
		UEntityHandleMap<FResolveOverlapInfo> Entities_To_ResolveOverlapInfo;

	};
	struct FDetailedEntityResolveOverlapInfo {
		UEntityHandleMap<FEdgesAndVerticesOverlapInfo> Entities_To_ShapeOverlapInfo;
	};

#pragma endregion

	/*
		NOTES
	*/

	/*
	
	Other way of abstracting this
	For Shapes Physics affects
	- translation
	- Rotation
	- Size

	Other way of abstracting this
	Physics affects
	- Points in shape
	- Shape as a whole

	For each triangle

	POINT BEHAVIOUR ON COLLISION
	Point collision is based on line collision
	Point physics just determine how a point moves when its line is hit

	TRIANGLE COLLISION
	Depending on the property of the triangle's material
	- There may be no individual point movement 
	- The other points will also move in accordance to the collision force
	- 

	MESH COLLISION
	Lets Keep the mesh structure

	- Pass 1 -> GetExternal forces 
	- Pass 2 -> Apply External Force -> thats is going to be a n^2 operation -> for each triangle, apply the force to every other triangle


	How to solve normal collisions and Torque collisions

	- In little prince notebook
	 
	
	*/

	//friction comes in when hit
	//remember there are two types of collision -> elastic and inelastic

	//OpenGL has a vector of matrices -> every time you draw -> all of the matrices are applied starting from the last pushed
	//OpenGl starts drawing on the bottom left corner -> keep that in mind for transformations
}

