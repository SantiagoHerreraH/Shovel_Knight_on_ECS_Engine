#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/URegistrySingleton.h"

#include "../../../include/Structs/FLevelPartition.h"
#include "../../../include/Structs/FObjectCollision.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FPlayer.h"
#include "../../../include/Structs/FMesh.h"
#include "../../../include/Structs/FShape.h"

#include "../../../include/Systems/TemplateSystems/TActivatableVector.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {

	class UDrawGrid {
		
	public:
		UDrawGrid(URegistry& registry);
		void Draw()const;

	private:
		UComponentIterator<URealTransform2D> m_RealTransforms;
		UComponentIterator<TActivatableVector<FGridCell>> m_Grids;
		UComponentIterator<TActivatableVector<FObjectCollision>> m_GridsCollisionInfo;
	};

	class UDrawVelocities {

	public:
		UDrawVelocities(URegistry& registry);
		void DrawAll()const;
		void Draw(const FVelocity& velocity, const FPoint2f& positionToDrawVelocityIn)const;

	private:
		UComponentIterator<URealTransform2D> m_WorldTransforms;
		UComponentIterator<FVelocity> m_Velocities;
	};

	class UDrawTransforms {
	public:
		UDrawTransforms(URegistry& registry);
		void Draw()const;

	private:

		UComponentIterator<URealTransform2D> m_RealTransforms;
	};

	class UDrawShapeCollisions {
	public:
		UDrawShapeCollisions(URegistry& registry);
		void Draw()const;

	private:
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityResolveOverlapInfo;
		UComponentIterator<FShape2Df> m_Shapes;
	};

	class UDrawRectCollisions {
	public:
		UDrawRectCollisions(URegistry& registry);
		void Draw()const;

	private:
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityResolveOverlapInfo;
		UComponentIterator<FRectCollider> m_RectColliders;
	};

	class UDebug {
	public:
		UDebug(URegistry& registry);
		void Draw()const;

	private:
		UDrawGrid m_DrawGrid;
		UDrawVelocities m_DrawVelocities;
		UDrawTransforms m_DrawTransforms;
		UDrawRectCollisions m_DrawRectCollisions;
		UDrawShapeCollisions m_DrawShapeCollisions;
	};
}