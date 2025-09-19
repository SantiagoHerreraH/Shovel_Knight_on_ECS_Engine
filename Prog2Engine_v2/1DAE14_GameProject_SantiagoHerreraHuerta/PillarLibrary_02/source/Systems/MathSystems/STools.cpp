
#include "../../../include/Systems/MathSystems/STools.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {
	const int STools::GetRandomBetweenRange(int min, int max)
	{
		//if you don't add the + 1, the range can never reach max
		return (std::rand() % (max - min + 1)) + min;
	}
	const FPoint3f STools::CalculateLerpPoint(const FPoint3f& start, const FPoint3f& end, const double percentageFromZeroToOne)
	{
		FPoint3f vector;
		vector.X = ((end.X - start.X) * Clamp(percentageFromZeroToOne, 0, 1)) + start.X;
		vector.Y = ((end.Y - start.Y) * Clamp(percentageFromZeroToOne, 0, 1)) + start.Y;
		vector.Z = ((end.Z - start.Z) * Clamp(percentageFromZeroToOne, 0, 1)) + start.Z;

		return vector;
	}

	const FPoint2i STools::CalculateLerpPoint(const FPoint2i& first, const FPoint2i& second, const double percentageFromZeroToOne)
	{

		int X = (second.X - first.X) * Clamp(percentageFromZeroToOne, 0, 1);
		int Y = (second.Y - first.Y) * Clamp(percentageFromZeroToOne, 0, 1);

		return { first.X + X, first.Y + Y };
	}

	const FPoint2 STools::CalculateLerpPoint(const FPoint2& start, const FPoint2& end, const double percentageFromZeroToOne)
	{
		double X = (end.X - start.X) * Clamp(percentageFromZeroToOne, 0, 1);
		double Y = (end.Y - start.Y) * Clamp(percentageFromZeroToOne, 0, 1);

		return { start.X + X, start.Y + Y };
	}

	const FPoint2f STools::CalculateLerpPoint(const FPoint2f& start, const FPoint2f& end, const double percentageFromZeroToOne)
	{
		return SVectors::CastToFloatPoint(CalculateLerpPoint(SVectors::CastToDoublePoint(start), SVectors::CastToDoublePoint(end), percentageFromZeroToOne));
	}
}