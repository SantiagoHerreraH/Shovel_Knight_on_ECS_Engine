
#include "../../../include/Systems/DebugSystems/UDebug.h"
#include "../../../include/Objects/FCoordinateHasher.h"
#include "../../../include/Structs/FCamera.h"

#include "../../../../Engine/utils.h"

Pillar::UDrawGrid::UDrawGrid(URegistry& registry)
{
	URealTransform2D realTransforms{};
	TActivatableVector<FGridCell> grids{};
	TActivatableVector<FObjectCollision> gridsCollisionInfo{};

	UComponentFilter filter{};
	filter.SetIncludingFilter(realTransforms, grids, gridsCollisionInfo);

	//registry.GetComponents(m_RealTransforms);
	registry.GetComponents(m_Grids);
	registry.GetComponents(m_GridsCollisionInfo);
}

void Pillar::UDrawGrid::Draw() const
{

	for (size_t archIdx = 0; archIdx < m_Grids.NumOfArchetypes(); archIdx++)
	{
		for (size_t gridIdx = 0; gridIdx < m_Grids.NumOfComponents(archIdx); gridIdx++)
		{
			for (size_t cellIdx = 0; cellIdx < m_Grids.ConstAt(archIdx, gridIdx).NumOfComponents(); cellIdx++)
			{
				//const FPoint2f& position = m_RealTransforms.ConstAt(archIdx, gridIdx).GetConstScreenTransform().Position;
				const FGridCell& currentCell = m_Grids.ConstAt(archIdx, gridIdx).ConstAt(cellIdx);
				const FObjectCollision& cellCollisionInfo = m_GridsCollisionInfo.ConstAt(archIdx, gridIdx).ConstAt(cellIdx);

				utils::SetColor(Color4f(0, 1, 0, 1));

				utils::DrawRect(
					currentCell.Boundary.Left,
					currentCell.Boundary.Top - currentCell.Boundary.Height,
					currentCell.Boundary.Width,
					currentCell.Boundary.Height);

				

			}
		}
	}

	
}

Pillar::UDrawTransforms::UDrawTransforms(URegistry& registry)
{
	registry.GetComponents(m_RealTransforms);
}

void Pillar::UDrawTransforms::Draw() const
{
	for (size_t archIdx = 0; archIdx < m_RealTransforms.NumOfArchetypes(); archIdx++)
	{
		for (size_t compIdx = 0; compIdx < m_RealTransforms.NumOfComponents(archIdx); compIdx++)
		{
			const FTransform2D& transform{ m_RealTransforms.ConstAt(archIdx, compIdx).GetConstScreenTransform() };
			
			utils::SetColor(Color4f(0, 0, 1, 1));
			utils::DrawEllipse(transform.Position.X, transform.Position.Y, 20, 20);
		}
	}
}

Pillar::UDebug::UDebug(URegistry& registry) :
	m_DrawGrid(registry),
	m_DrawTransforms(registry),
	m_DrawVelocities(registry),
	m_DrawShapeCollisions(registry),
	m_DrawRectCollisions(registry)
{
}

void Pillar::UDebug::Draw() const
{
	//m_DrawGrid.Draw();
	m_DrawTransforms.Draw();
	m_DrawVelocities.DrawAll();
	m_DrawRectCollisions.Draw();
	//m_DrawShapeCollisions.Draw(); 
}

Pillar::UDrawVelocities::UDrawVelocities(URegistry& registry)
{
	FVelocity velocity{};
	FLocalTransform2D transform{};
	FCamera camera{};

	UComponentFilter filter{};
	filter.SetIncludingFilter(velocity, transform, camera);

	registry.GetComponents(m_Velocities);
	registry.GetComponents(m_WorldTransforms);
}

void Pillar::UDrawVelocities::DrawAll() const
{
	for (size_t archIdx = 0; archIdx < m_Velocities.NumOfArchetypes(); archIdx++)
	{
		for (size_t compIdx = 0; compIdx < m_Velocities.NumOfComponents(archIdx); compIdx++)
		{
			FPoint2f transPos = m_WorldTransforms.ConstAt(archIdx, compIdx).GetConstScreenTransform().Position;
			Draw(m_Velocities.ConstAt(archIdx, compIdx), transPos);

		}
	}
}

void Pillar::UDrawVelocities::Draw(const FVelocity& velocity, const FPoint2f& positionToDrawVelocityIn) const
{
	utils::SetColor(Color4f(0, 1, 0, 1));
	utils::DrawLine(
		positionToDrawVelocityIn.X,
		positionToDrawVelocityIn.Y,
		positionToDrawVelocityIn.X + velocity.DistancePerSecond.X,
		positionToDrawVelocityIn.Y + velocity.DistancePerSecond.Y);
}

Pillar::UDrawShapeCollisions::UDrawShapeCollisions(URegistry& registry)
{
	FEntityResolveOverlapInfo entityResolveOverlapInfo{};
	FShape2Df shape{};

	UComponentFilter filter{};
	filter.SetIncludingFilter(entityResolveOverlapInfo, shape);

	registry.GetComponents(m_EntityResolveOverlapInfo, filter);
	registry.GetComponents(m_Shapes, filter);
}

void Pillar::UDrawShapeCollisions::Draw() const
{
	FPoint2f start{};
	FPoint2f end{};

	for (size_t archIdx = 0; archIdx < m_EntityResolveOverlapInfo.NumOfArchetypes(); archIdx++)
	{
		
		for (size_t compIdx = 0; compIdx < m_EntityResolveOverlapInfo.NumOfActivatedComponents(archIdx); compIdx++)
		{
			utils::SetColor(Color4f(1, 1, 0, 1));

			for (size_t overlapInfoIdx = 0; overlapInfoIdx < m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); overlapInfoIdx++)
			{
				start = m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint;
				end.X = 
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint.X + 
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).TranslationVector.X;
				end.Y =
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint.Y +
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).TranslationVector.Y;
			
				
				utils::DrawLine(start.X, start.Y, end.X, end.Y);
			}

			if (m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities() > 0)
			{
				utils::SetColor(Color4f(0, 1, 0, 1));
			}
			else
			{
				utils::SetColor(Color4f(1, 0, 0, 1));
			}

			
			for (size_t vrtxIdx = 0; vrtxIdx < m_Shapes.ConstAt(archIdx, compIdx).Shape.size(); vrtxIdx++)
			{
				start = m_Shapes.ConstAt(archIdx, compIdx).Shape.at(vrtxIdx);
				end = m_Shapes.ConstAt(archIdx, compIdx).Shape.at((vrtxIdx + 1) % m_Shapes.ConstAt(archIdx, compIdx).Shape.size());

				utils::DrawLine(start.X, start.Y, end.X, end.Y);
			}
		}
	}
}

Pillar::UDrawRectCollisions::UDrawRectCollisions(URegistry& registry)
{
	FEntityResolveOverlapInfo entityResolveOverlapInfo{};
	FRectCollider rectCollider{};

	UComponentFilter filter{};
	filter.SetIncludingFilter(entityResolveOverlapInfo, rectCollider);

	registry.GetComponents(m_EntityResolveOverlapInfo, filter);
	registry.GetComponents(m_RectColliders, filter);
}

void Pillar::UDrawRectCollisions::Draw() const
{
	FPoint2f start{};
	FPoint2f end{};
	Rectf rect{};

	for (size_t archIdx = 0; archIdx < m_EntityResolveOverlapInfo.NumOfArchetypes(); archIdx++)
	{

		for (size_t compIdx = 0; compIdx < m_EntityResolveOverlapInfo.NumOfActivatedComponents(archIdx); compIdx++)
		{
			utils::SetColor(Color4f(1, 1, 0, 1));

			for (size_t overlapInfoIdx = 0; overlapInfoIdx < m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); overlapInfoIdx++)
			{
				start = m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint;
				end.X =
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint.X +
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).TranslationVector.X;
				end.Y =
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).IntersectionPoint.Y +
					m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(overlapInfoIdx).TranslationVector.Y;


				utils::DrawLine(start.X, start.Y, end.X, end.Y);
			}

			if (m_EntityResolveOverlapInfo.ConstAt(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities() > 0)
			{
				utils::SetColor(Color4f(0, 1, 0, 1));
			}
			else
			{
				utils::SetColor(Color4f(1, 0, 0, 1));
			}

			rect.bottom = m_RectColliders.ConstAt(archIdx, compIdx).CurrentRect.Top - m_RectColliders.ConstAt(archIdx, compIdx).CurrentRect.Height;
			rect.height = m_RectColliders.ConstAt(archIdx, compIdx).CurrentRect.Height;
			rect.left = m_RectColliders.ConstAt(archIdx, compIdx).CurrentRect.Left;
			rect.width = m_RectColliders.ConstAt(archIdx, compIdx).CurrentRect.Width;

			utils::DrawRect(rect);

		}
	}
}
