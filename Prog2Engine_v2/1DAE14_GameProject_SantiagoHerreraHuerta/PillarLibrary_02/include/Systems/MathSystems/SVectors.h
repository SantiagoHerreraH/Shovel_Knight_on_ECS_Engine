
#pragma once

#include "../../../include/Structs/FVector.h"

namespace Pillar {

	class SVectors {
	public:

		/*

			2D

		*/

		//2D casts
		static const FPoint2 CastToDoublePoint(const FPoint2i& vector);
		static const FPoint2 CastToDoublePoint(const FPoint2f& vector);

		static const FPoint2i CastToIntPoint(const FPoint2& vector);
		static const FPoint2i CastToIntPoint(const FPoint2f& vector);

		static const FPoint2f CastToFloatPoint(const FPoint2& vector);
		static const FPoint2f CastToFloatPoint(const FPoint2i& vector);

		//Operations
		static const FPoint2f Add(const FPoint2f& first, const FPoint2f& second);
		static const FPoint2  Add(const FPoint2& first, const FPoint2& second);
		static const FPoint2i Add(const FPoint2i& first, const FPoint2i& second);


		static const FPoint2 Subtract(const  FPoint2&  end, const FPoint2&  start);
		static const FPoint2f Subtract(const FPoint2f& end, const FPoint2f& start);
		static const FPoint2i Subtract(const FPoint2i& end, const FPoint2i& start);

		//put this function in SVectors
		static const FPoint2f ObtainRelativeVectorComponent(const FVector& direction, const FPoint2f& centerPoint);

		static const FPoint2f OrthogonalVector(const FPoint2f& upVector);

		/*

			3D

		*/

		//3D casts
		static const FPoint3 CastToDoublePoint(const FPoint3i& vector);
		static const FPoint3 CastToDoublePoint(const FPoint3f& vector);

		static const FPoint3i CastToIntPoint(const FPoint3& vector);
		static const FPoint3i CastToIntPoint(const FPoint3f& vector);

		static const FPoint3f CastToFloatPoint(const FPoint3& vector);
		static const FPoint3f CastToFloatPoint(const FPoint3i& vector);

		static const FPoint3f Scale(const FPoint3f& vector, const float scale);
		static const FPoint3  Scale(const FPoint3& vector, const double scale);
		static const FPoint2  Scale(const FPoint2& vector, const double scale);
		static const FPoint2f Scale(const FPoint2f& vector, const float scale);
		static const FPoint2i Scale(const FPoint2i& vector, const float scale);

		/*

			CALCULATIONS

		*/

		static const FPoint3 GetUnitRotationVector(const FPoint3& centerReference, const FPoint3& point);
		static const FPoint2 GetUnitRotationVector(const FPoint2& centerReference, const FPoint2& point);

		static const float  Distance(const FPoint2i& pointOne, const FPoint2i& pointTwo);
		static const float  Distance(const FPoint2f& pointOne, const FPoint2f& pointTwo);
		static const double Distance(const FPoint2& pointOne, const FPoint2& pointTwo);
		static const double Distance(const FPoint3& pointOne, const FPoint3& pointTwo);
		static const double Distance(const FPoint3f& pointOne, const FPoint3f& pointTwo);

		static const float Length(const FPoint2i& vector);
		static const float Length(const FPoint2f& vector);
		static const double Length(const FPoint2& vector);
		static const double Length(const FPoint3& vector);
		static const double Length(const FPoint3f& vector);

		static const double ProjectedPercentage(const FPoint2& vectorReference, const FPoint2& vectorToProject);

		static const double ProjectedLength(const FPoint2& vectorReference, const FPoint2& vectorToProject);
		static const float  ProjectedLength(const FPoint2f& vectorReference, const FPoint2f& vectorToProject);
		static const float  ProjectedLength(const FPoint2i& vectorReference, const FPoint2i& vectorToProject);
		static const double ProjectedLength(const FPoint3& vectorReference, const FPoint3& vectorToProject);

		static const FPoint3 ProjectedVector(const FPoint3& vectorReference, const FPoint3& vectorToProject);
		static const FPoint2  ProjectedVector(const FPoint2& vectorReference, const FPoint2& vectorToProject);
		static const FPoint2f ProjectedVector(const FPoint2f& vectorReference, const FPoint2f& vectorToProject);
		static const FPoint2i ProjectedVector(const FPoint2i& vectorReference, const FPoint2i& vectorToProject);

		static const double DotProduct(const FPoint3& one, const FPoint3& two);
		static const double DotProduct(const FPoint3f& one, const FPoint3f& two);
		static const double DotProduct(const FPoint2& one, const FPoint2& two);
		static const float  DotProduct(const FPoint2f& one, const FPoint2f& two);
		static const float	DotProduct(const FPoint2i& one, const FPoint2i& two);

		static const FVector Direction(const FPoint2& from, const FPoint2& to);
		static const FVector Direction(const FPoint2f& vector);

		static const FPoint3 RotationToForwardVector(const FRotation3& rotation);
		static const FPoint3 RotationToUpVector(const FRotation3& rotation);
		static const FPoint3 RotationToSideVector(const FRotation3& rotation);
		static const FPoint2f RotateVector(const FPoint2f& vectorToRotate, double rotation);
		static const FPoint3 RotateVector(const FPoint3& vectorToRotate, const FRotation3& rotation);
		static const FPoint3f RotateVector(const FPoint3f& vectorToRotate, const FRotation3& rotation);

		static const FPoint3  CrossProduct(const FPoint3& sideVector, const FPoint3& upVector);
		static const FPoint3f CrossProduct(const FPoint3f& sideVector, const FPoint3f& upVector);

		static const FPoint3  NormalizeVector(const FPoint3& vectorToNormalize);
		static const FPoint3f NormalizeVector(const FPoint3f& vectorToNormalize);

		static const FPoint2f NormalizeVector(const FPoint2f& vectorToNormalize);

		static const double Determinant(const FPoint3& sideVector, const FPoint3& upVector);
		static const double Determinant(const FPoint2& sideVector, const FPoint2& upVector);
		static const float  Determinant(const FPoint2f& sideVector, const FPoint2f& upVector);
		static const double Determinant(const double sideVectorX, const double sideVectorY, const double upVectorX, const double upVectorY);

		static const FPoint3  Subtract(const FPoint3& startingPoint, const FPoint3& endPoint);
		static const FPoint3f Subtract(const FPoint3f& startingPoint, const FPoint3f& endPoint);

	};

	class SRotators {

	public:

		static const FRotation3 CombineRotations(const FRotation3& firstRotation, const FRotation3& secondRotation);
		static const FRotation3 ConstraintRotation(const FRotation3& rotation);

		//the way you calculate it is you determine the rotation 
		//transformation that needs to happen to get from the forward vector to the input vector
		static const FRotation3 LookAtRotation(const FPoint3f& vector); // 

	};

}