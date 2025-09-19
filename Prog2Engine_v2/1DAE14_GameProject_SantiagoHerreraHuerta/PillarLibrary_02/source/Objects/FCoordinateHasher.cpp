
#include "../../include/Objects/FCoordinateHasher.h"
#include "../../include/Systems/MathSystems/SVectors.h"

int Pillar::FCoordinateHasher::Hash(const FPoint2i& coordinate)
{
	FPoint2i topLeftCorner{};
	topLeftCorner.X = -((m_ParamsForLevelPartition.NumberOfColumns / 2.f) * m_ParamsForLevelPartition.BoundaryWidth);
	topLeftCorner.Y = -((m_ParamsForLevelPartition.NumberOfRows / 2.f) * m_ParamsForLevelPartition.BoundaryHeight);
	FPoint2i topLeftCenter{};
	topLeftCenter.X = topLeftCorner.X + m_ParamsForLevelPartition.BoundaryWidth;
	topLeftCenter.Y = topLeftCorner.Y + m_ParamsForLevelPartition.BoundaryHeight;

	int numberOfColumn = std::lround(((coordinate.X - topLeftCorner.X) / m_ParamsForLevelPartition.BoundaryWidth));
	int numberOfRow = std::lround(((coordinate.Y - topLeftCorner.Y) / m_ParamsForLevelPartition.BoundaryHeight));

	return (numberOfRow * m_ParamsForLevelPartition.NumberOfColumns) + numberOfColumn;
}

int Pillar::FCoordinateHasher::Hash(const FPoint2f& coordinate)
{
	return Hash(SVectors::CastToIntPoint(coordinate));
}

int Pillar::FCoordinateHasher::Hash(const FPoint2& coordinate)
{
	return Hash(SVectors::CastToIntPoint(coordinate));
}
