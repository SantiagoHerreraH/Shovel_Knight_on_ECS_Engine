#pragma once

#include "../../../include/Structs/FVector.h"

namespace Pillar {

	class SAngles {
	
	public:
	
		static const double CalculateDegreeBetweenVectorComponents(const FPoint2i& from, const FPoint2i& to);
		static const double CalculateRadiansBetweenVectorComponents(const FPoint2i& from, const FPoint2i& to);
	
		static const double InDegrees(const double radians);
		static const double InRadians(const double degrees);
	
		static const double ClampToDegrees(const double value);
		static const double ClampToRadians(const double value);
	
	};
}