#pragma once 

#include "../../../include/Structs/FMesh.h"
#include "../../../include/Structs/FShape.h"

namespace Pillar {

	class STrigonometry {

	public:

		static const float Area(const FTriangle2f& triangle);
		static const FPoint2f Center(const FTriangle2f& triangle);
		static void Shape(const FTriangle2f& triangle, FShape2Df& outShape);

		static const double AbsoluteHypotenuse(const double adjacentSide, const double oppositeSide);
		static const double AbsoluteAdjacentSide(const double hypotenuse, const double oppositeSide);
		static const double AbsoluteOppositeSide(const double hypotenuse, const double adjacentSide);
	};

}