
#define _USE_MATH_DEFINES 
#include <math.h>

#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {

	/*

		TRIGONOMETRY

	*/

	const float STrigonometry::Area(const FTriangle2f& triangle)
	{
		return 
			SVectors::Determinant(
				SVectors::Subtract(
					triangle.Vertices[1], triangle.Vertices[0]),
				SVectors::Subtract(
					triangle.Vertices[2], triangle.Vertices[0]))/2;
	}

	const FPoint2f STrigonometry::Center(const FTriangle2f& triangle)
	{
		FPoint2f result{};
		for (size_t i = 0; i < 3; i++)
		{
			result.X += triangle.Vertices[i].X;
			result.Y += triangle.Vertices[i].Y;
		}

		result.X /= 3.f;
		result.Y /= 3.f;

		return result;
	}

	void STrigonometry::Shape(const FTriangle2f& triangle, FShape2Df& outShape)
	{
		outShape.Shape.clear();
		outShape.Center = STrigonometry::Center(triangle);

		outShape.Shape.reserve(3);

		for (size_t i = 0; i < 3; i++)
		{
			outShape.Shape.push_back(triangle.Vertices[i]);
		}
	}

	const double STrigonometry::AbsoluteHypotenuse(const double adjacentSide, const double oppositeSide)
	{
		//a^2 + b^2 = c^2
		return sqrt(pow(adjacentSide, 2) + pow(oppositeSide, 2));
	}

	const double STrigonometry::AbsoluteAdjacentSide(const double hypotenuse, const double oppositeSide)
	{
		//b^2 = -a^2 + c^2
		return sqrt(pow(hypotenuse, 2) - pow(oppositeSide, 2));
	}

	const double STrigonometry::AbsoluteOppositeSide(const double hypotenuse, const double adjacentSide)
	{
		//a^2 = -b^2 + c^2
		return sqrt(pow(hypotenuse, 2) - pow(adjacentSide, 2));
	}

}