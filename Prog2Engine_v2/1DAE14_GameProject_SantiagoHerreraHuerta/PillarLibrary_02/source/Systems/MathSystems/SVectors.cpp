
#define _USE_MATH_DEFINES 
#include <math.h>

#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/MathSystems/SAngles.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"
#include "../../../include/Systems/MathSystems/STools.h"

namespace Pillar {

	/*

		VECTORS

	*/

	const FPoint2 SVectors::CastToDoublePoint(const FPoint2i& point)
	{
		return { static_cast<double>(point.X), static_cast<double>(point.Y) };
	}

	const FPoint2 SVectors::CastToDoublePoint(const FPoint2f& point)
	{
		return { static_cast<double>(point.X), static_cast<double>(point.Y) };
	}

	const FPoint2i SVectors::CastToIntPoint(const FPoint2& point)
	{
		return { static_cast<int>(point.X), static_cast<int>(point.Y) };
	}

	const FPoint2i SVectors::CastToIntPoint(const FPoint2f& point)
	{
		return { static_cast<int>(point.X), static_cast<int>(point.Y) };
	}

	const FPoint2f SVectors::CastToFloatPoint(const FPoint2& point)
	{
		return { static_cast<float>(point.X), static_cast<float>(point.Y) };
	}

	const FPoint2f SVectors::CastToFloatPoint(const FPoint2i& point)
	{
		return { static_cast<float>(point.X), static_cast<float>(point.Y) };
	}

	const FPoint2f SVectors::Add(const FPoint2f& first, const FPoint2f& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FPoint2 SVectors::Add(const FPoint2& first, const FPoint2& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FPoint2i SVectors::Add(const FPoint2i& first, const FPoint2i& second)
	{
		return { first.X + second.X, first.Y + second.Y };
	}

	const FPoint2 SVectors::Subtract(const FPoint2& end, const FPoint2& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FPoint2f SVectors::Subtract(const FPoint2f& end, const FPoint2f& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FPoint2i SVectors::Subtract(const FPoint2i& end, const FPoint2i& start)
	{
		return { end.X - start.X, end.Y - start.Y };
	}

	const FPoint2f SVectors::ObtainRelativeVectorComponent(const FVector& direction, const FPoint2f& centerPoint)
	{
		double adjacentOverHypotenuse = cos((SAngles::InRadians(direction.AngleInDegrees)));
		double oppositeOverHypotenuse = sin((SAngles::InRadians(direction.AngleInDegrees)));

		double adjacent = adjacentOverHypotenuse * direction.Length;
		double opposite = oppositeOverHypotenuse * direction.Length;

		return
		{
		float(centerPoint.X + adjacent),
		float(centerPoint.Y + opposite) };
	}

	const FPoint2f SVectors::OrthogonalVector(const FPoint2f& upVector)
	{
		return FPoint2f{upVector.Y, -(upVector.X)};
	}

	const FPoint3 SVectors::CastToDoublePoint(const FPoint3i& point)
	{
		return {
			static_cast<double>(point.X),
			static_cast<double>(point.Y),
			static_cast<double>(point.Z) };
	}

	const FPoint3 SVectors::CastToDoublePoint(const FPoint3f& point)
	{
		return {
			static_cast<double>(point.X),
			static_cast<double>(point.Y),
			static_cast<double>(point.Z) };
	}

	const FPoint3i SVectors::CastToIntPoint(const FPoint3& point)
	{
		return {
			static_cast<int>(point.X),
			static_cast<int>(point.Y),
			static_cast<int>(point.Z) };
	}

	const FPoint3i SVectors::CastToIntPoint(const FPoint3f& point)
	{
		return {
			static_cast<int>(point.X),
			static_cast<int>(point.Y),
			static_cast<int>(point.Z) };
	}

	const FPoint3f SVectors::CastToFloatPoint(const FPoint3& point)
	{
		return {
			static_cast<float>(point.X),
			static_cast<float>(point.Y),
			static_cast<float>(point.Z) };
	}

	const FPoint3f SVectors::CastToFloatPoint(const FPoint3i& point)
	{
		return {
			static_cast<float>(point.X),
			static_cast<float>(point.Y),
			static_cast<float>(point.Z) };
	}

	const FPoint3f SVectors::Scale(const FPoint3f& point, const float scale)
	{
		return FPoint3f{ point.X * scale, point.Y * scale, point.Z * scale };
	}

	const FPoint3 SVectors::Scale(const FPoint3& point, const double scale)
	{
		return FPoint3{ point.X * scale, point.Y * scale, point.Z * scale };
	}

	const FPoint2 SVectors::Scale(const FPoint2& point, const double scale)
	{
		return FPoint2{ point.X * scale, point.Y * scale };
	}

	const FPoint2f SVectors::Scale(const FPoint2f& point, const float scale)
	{
		return FPoint2f{ point.X * scale, point.Y * scale };
	}

	const FPoint2i SVectors::Scale(const FPoint2i& point, const float scale)
	{
		return FPoint2i{ int(point.X * scale), int(point.Y * scale) };
	}

	const FPoint3 SVectors::GetUnitRotationVector(const FPoint3& centerReference, const FPoint3& point)
	{
		double xDelta = point.X - centerReference.X;
		double yDelta = point.Y - centerReference.Y;
		double zDelta = point.Z - centerReference.Z;

		double x_z_Hypotenuse = STrigonometry::AbsoluteHypotenuse(xDelta, zDelta);
		double realHypotenuse = STrigonometry::AbsoluteHypotenuse(x_z_Hypotenuse, yDelta);

		return FPoint3{ xDelta / realHypotenuse, yDelta / realHypotenuse, zDelta / realHypotenuse };
	}

	const FPoint2 SVectors::GetUnitRotationVector(const FPoint2& centerReference, const FPoint2& point)
	{
		double xDelta = point.X - centerReference.X;
		double yDelta = point.Y - centerReference.Y;

		double hypotenuse = STrigonometry::AbsoluteHypotenuse(xDelta, yDelta);

		return FPoint2{ xDelta / hypotenuse, yDelta / hypotenuse };
	}

	const float SVectors::Distance(const FPoint2i& pointOne, const FPoint2i& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);;
	}

	const float SVectors::Distance(const FPoint2f& pointOne, const FPoint2f& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);;
	}

	const double SVectors::Distance(const FPoint2& pointOne, const FPoint2& pointTwo)
	{
		return STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Y - pointOne.Y);
	}

	const double SVectors::Distance(const FPoint3& pointOne, const FPoint3& pointTwo)
	{
		double x_z_Hypotenuse = STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Z - pointOne.Z);

		return STrigonometry::AbsoluteHypotenuse(x_z_Hypotenuse, pointTwo.Y - pointOne.Y);
	}

	const double SVectors::Distance(const FPoint3f& pointOne, const FPoint3f& pointTwo)
	{
		double x_z_Hypotenuse = STrigonometry::AbsoluteHypotenuse(pointTwo.X - pointOne.X, pointTwo.Z - pointOne.Z);

		return STrigonometry::AbsoluteHypotenuse(x_z_Hypotenuse, pointTwo.Y - pointOne.Y);
	}

	const double SVectors::Length(const FPoint2& vector)
	{
		return STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
	}

	const float SVectors::Length(const FPoint2i& vector)
	{
		return STrigonometry::AbsoluteHypotenuse((float)vector.X, (float)vector.Y);
	}

	const float SVectors::Length(const FPoint2f& vector)
	{
		return STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
	}

	const double SVectors::Length(const FPoint3& point)
	{
		double x_z_Hypotenuse = STrigonometry::AbsoluteHypotenuse(point.X, point.Z);

		return STrigonometry::AbsoluteHypotenuse(x_z_Hypotenuse, point.Y);
	}

	const double SVectors::Length(const FPoint3f& vector)
	{
		double x_z_Hypotenuse = STrigonometry::AbsoluteHypotenuse(vector.X, vector.Z);

		return STrigonometry::AbsoluteHypotenuse(x_z_Hypotenuse, vector.Y);
	}

	const double SVectors::ProjectedPercentage(const FPoint2& vectorReference, const FPoint2& vectorToProject)
	{
		double multipliedLengths = SVectors::Length(vectorReference) * SVectors::Length(vectorToProject);

		if (multipliedLengths == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / multipliedLengths;
	}

	//You obtain the vectortoBeProjected Length
	const double SVectors::ProjectedLength(const FPoint2& vectorReference, const FPoint2& vectorToProject)
	{
		double length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const float SVectors::ProjectedLength(const FPoint2f& vectorReference, const FPoint2f& vectorToProject)
	{
		float length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const float SVectors::ProjectedLength(const FPoint2i& vectorReference, const FPoint2i& vectorToProject)
	{
		double length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const double SVectors::ProjectedLength(const FPoint3& vectorReference, const FPoint3& vectorToProject)
	{
		double length = SVectors::Length(vectorReference);

		if (length == 0.0)
		{
			//debug comment here
			return 0.0;
		}

		return SVectors::DotProduct(vectorReference, vectorToProject) / length;
	}

	const FPoint3 SVectors::ProjectedVector(const FPoint3& vectorReference, const FPoint3& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject)/ length);
	}

	const FPoint2 SVectors::ProjectedVector(const FPoint2& vectorReference, const FPoint2& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const FPoint2f SVectors::ProjectedVector(const FPoint2f& vectorReference, const FPoint2f& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const FPoint2i SVectors::ProjectedVector(const FPoint2i& vectorReference, const FPoint2i& vectorToProject)
	{
		double length{ Length(vectorReference) };
		length = length == 0 ? 0.00001 : length;
		return Scale(vectorReference, ProjectedLength(vectorReference, vectorToProject) / length);
	}

	const double SVectors::DotProduct(const FPoint3& one, const FPoint3& two)
	{
		return one.X * two.X + one.Y * two.Y + one.Z * two.Z;
	}

	const double SVectors::DotProduct(const FPoint3f& one, const FPoint3f& two)
	{
		return one.X * two.X + one.Y * two.Y + one.Z * two.Z;
	}

	const double SVectors::DotProduct(const FPoint2& one, const FPoint2& two)
	{
		return one.X * two.X + one.Y * two.Y;
	}

	const float SVectors::DotProduct(const FPoint2f& one, const FPoint2f& two)
	{
		return DotProduct(CastToDoublePoint(one), CastToDoublePoint(two));
	}

	const float SVectors::DotProduct(const FPoint2i& one, const FPoint2i& two)
	{
		return DotProduct(CastToDoublePoint(one), CastToDoublePoint(two));
	}

	const FVector SVectors::Direction(const FPoint2& from, const FPoint2& to)
	{

		FVector direction{};

		double xDelta = to.X - from.X;
		double yDelta = to.Y - from.Y;

		double radians = atan2(yDelta, xDelta);
		double degrees = SAngles::InDegrees(radians);

		direction.Length = STrigonometry::AbsoluteHypotenuse(xDelta, yDelta);
		direction.AngleInDegrees = degrees;

		return direction;
	}

	const FVector SVectors::Direction(const FPoint2f& vector)
	{
		FVector direction{};
		double radians = atan2(vector.Y, vector.X);
		double degrees = SAngles::InDegrees(radians);

		direction.Length = STrigonometry::AbsoluteHypotenuse(vector.X, vector.Y);
		direction.AngleInDegrees = degrees;
		return direction;
	}

	const FPoint3 SVectors::RotationToForwardVector(const FRotation3& rotation)
	{
		FPoint3 forwardVector{ 0,0,1 };
		return SVectors::RotateVector(forwardVector, rotation);
	}

	const FPoint3 SVectors::RotationToUpVector(const FRotation3& rotation)
	{
		FPoint3 upVector{ 0,1,0 };
		return SVectors::RotateVector(upVector, rotation);
	}

	const FPoint3 SVectors::RotationToSideVector(const FRotation3& rotation)
	{
		FPoint3 sideVector{ 1,0,0 };
		return SVectors::RotateVector(sideVector, rotation);
	}

	const FPoint2f SVectors::RotateVector(const FPoint2f& vectorToRotate, double rotation)
	{
		FMatrix4x4f matrix{};
		SMatrix::RotateZ(matrix, rotation);
		FPoint2f result{};
		SMatrix::Multiply(matrix, vectorToRotate, result);
		return result;
	}

	const FPoint3 SVectors::RotateVector(const FPoint3& vectorToRotate, const FRotation3& rotation)
	{
		//Change to FMatrix3x3
		/*
		Remember
		a	e	i	m
		b	f	j	n
		c	g	k	o
		d	h	l	p

		*/

		//Check if these rotation matrices are correct

		DMatrix4x4f rotationXMatrix{
				1,0,0,0,
				0,cos(SAngles::InRadians(rotation.X)), sin(SAngles::InRadians(rotation.X)),0,
				0,-(sin(SAngles::InRadians(rotation.X))),  cos(SAngles::InRadians(rotation.X)),0 };


		DMatrix4x4f rotationYMatrix{
			cos(SAngles::InRadians(rotation.Y)), 0, -(sin(SAngles::InRadians(rotation.Y))),0,
			0,1,0,0,
			sin(SAngles::InRadians(rotation.Y)), 0, cos(SAngles::InRadians(rotation.Y)),0 };


		DMatrix4x4f rotationZMatrix{
			  cos(SAngles::InRadians(rotation.Z)),	 sin(SAngles::InRadians(rotation.Z)), 0, 0,
			-(sin(SAngles::InRadians(rotation.Z))),  cos(SAngles::InRadians(rotation.Z)), 0, 0,
			0,0,1,0, };


		FPoint4f vectorToRotate4D{};
		vectorToRotate4D.X = vectorToRotate.X;
		vectorToRotate4D.Y = vectorToRotate.Y;
		vectorToRotate4D.Z = vectorToRotate.Z;

		FPoint4f vectorToRotate4D_AfterX{};
		FPoint4f vectorToRotate4D_AfterY{};
		FPoint4f vectorToRotate4D_AfterZ{};

		SMatrix::Multiply(rotationXMatrix, vectorToRotate4D, vectorToRotate4D_AfterX);
		SMatrix::Multiply(rotationYMatrix, vectorToRotate4D_AfterX, vectorToRotate4D_AfterY);
		SMatrix::Multiply(rotationZMatrix, vectorToRotate4D_AfterY,	 vectorToRotate4D_AfterZ);

		FPoint3 rotatedVector{};
		rotatedVector.X = vectorToRotate4D_AfterZ.X;
		rotatedVector.Y = vectorToRotate4D_AfterZ.Y;
		rotatedVector.Z = vectorToRotate4D_AfterZ.Z;

		return rotatedVector;
	}

	const FPoint3f SVectors::RotateVector(const FPoint3f& vectorToRotate, const FRotation3& rotation)
	{
		//Change to FMatrix3x3
		/*
		Remember
		a	e	i	m
		b	f	j	n
		c	g	k	o
		d	h	l	p

		*/

		DMatrix4x4f rotationXMatrix{
				1,0,0,0,
				0,cos(SAngles::InRadians(rotation.X)), sin(SAngles::InRadians(rotation.X)),0,
				0,-(sin(SAngles::InRadians(rotation.X))),  cos(SAngles::InRadians(rotation.X)),0 };


		DMatrix4x4f rotationYMatrix{
			cos(SAngles::InRadians(rotation.Y)), 0, -(sin(SAngles::InRadians(rotation.Y))),0,
			0,1,0,0,
			sin(SAngles::InRadians(rotation.Y)), 0, cos(SAngles::InRadians(rotation.Y)),0 };


		DMatrix4x4f rotationZMatrix{
			  cos(SAngles::InRadians(rotation.Z)),	sin(SAngles::InRadians(rotation.Z)), 0, 0,
			-(sin(SAngles::InRadians(rotation.Z))),  cos(SAngles::InRadians(rotation.Z)), 0, 0,
			0,0,1,0, };


		FPoint4f vectorToRotate4D{};
		vectorToRotate4D.X = vectorToRotate.X;
		vectorToRotate4D.Y = vectorToRotate.Y;
		vectorToRotate4D.Z = vectorToRotate.Z;

		FPoint4f vectorToRotate4D_AfterX{};
		FPoint4f vectorToRotate4D_AfterY{};
		FPoint4f vectorToRotate4D_AfterZ{};

		SMatrix::Multiply(rotationXMatrix, vectorToRotate4D, vectorToRotate4D_AfterX);
		SMatrix::Multiply(rotationYMatrix, vectorToRotate4D_AfterX, vectorToRotate4D_AfterY);
		SMatrix::Multiply(rotationZMatrix, vectorToRotate4D_AfterY, vectorToRotate4D_AfterZ);

		FPoint3f rotatedVector{};
		rotatedVector.X = vectorToRotate4D_AfterZ.X;
		rotatedVector.Y = vectorToRotate4D_AfterZ.Y;
		rotatedVector.Z = vectorToRotate4D_AfterZ.Z;

		return rotatedVector;
	}

	const FPoint3 SVectors::CrossProduct(const FPoint3& sideVector, const FPoint3& upVector)
	{
		FPoint3 crossProduct{};
		crossProduct.X = SVectors::Determinant(sideVector.Y, sideVector.Z, upVector.Y, upVector.Z);
		crossProduct.Y = SVectors::Determinant(sideVector.Z, sideVector.X, upVector.Z, upVector.X);
		crossProduct.Z = SVectors::Determinant(sideVector.X, sideVector.Y, upVector.X, upVector.Y);
		return crossProduct;
	}

	const FPoint3f SVectors::CrossProduct(const FPoint3f& sideVector, const FPoint3f& upVector)
	{
		FPoint3f crossProduct{};
		crossProduct.X = SVectors::Determinant(sideVector.Y, sideVector.Z, upVector.Y, upVector.Z);
		crossProduct.Y = SVectors::Determinant(sideVector.Z, sideVector.X, upVector.Z, upVector.X);
		crossProduct.Z = SVectors::Determinant(sideVector.X, sideVector.Y, upVector.X, upVector.Y);
		return crossProduct;
	}

	const FPoint3 SVectors::NormalizeVector(const FPoint3& vectorToNormalize)
	{
		double vectorLength = Length(vectorToNormalize);
		FPoint3 normalizedVector{ vectorToNormalize };
		normalizedVector.X /= vectorLength;
		normalizedVector.Y /= vectorLength;
		normalizedVector.Z /= vectorLength;

		return normalizedVector;
	}

	const FPoint3f SVectors::NormalizeVector(const FPoint3f& vectorToNormalize)
	{
		double vectorLength = Length(vectorToNormalize);
		FPoint3f normalizedVector{ vectorToNormalize };
		normalizedVector.X /= vectorLength;
		normalizedVector.Y /= vectorLength;
		normalizedVector.Z /= vectorLength;

		return normalizedVector;
	}

	const FPoint2f SVectors::NormalizeVector(const FPoint2f& vectorToNormalize)
	{
		float vectorLength = Length(vectorToNormalize);
		FPoint2f normalizedVector{ vectorToNormalize };
		if (vectorLength != 0.f)
		{
			normalizedVector.X /= vectorLength;
			normalizedVector.Y /= vectorLength;
			return normalizedVector;
		}
		
		return FPoint2f{ 0,0 };
	}

	const double SVectors::Determinant(const FPoint3& sideVector, const FPoint3& upVector)
	{
		/*
		Order Matters
		determinants of the following axis
		X
		Y
		Z
		*/

		return
			SVectors::Determinant(sideVector.Y, sideVector.Z, upVector.Y, upVector.Z) +
			SVectors::Determinant(sideVector.Z, sideVector.X, upVector.Z, upVector.X) +
			SVectors::Determinant(sideVector.X, sideVector.Y, upVector.X, upVector.Y);
	}

	const double SVectors::Determinant(const FPoint2& sideVector, const FPoint2& upVector)
	{
		/*
		Order Matters
		*/
		return (sideVector.X * upVector.Y) - (sideVector.Y * upVector.X);
	}

	const float SVectors::Determinant(const FPoint2f& sideVector, const FPoint2f& upVector)
	{
		/*
		Order Matters
		*/
		return (sideVector.X * upVector.Y) - (sideVector.Y * upVector.X);
	}

	const double SVectors::Determinant(const double sideVectorX, const double sideVectorY, const double upVectorX, const double upVectorY)
	{
		/*
		Order Matters
		*/
		return (sideVectorX * upVectorY) - (sideVectorY * upVectorX);
	}

	const FPoint3 SVectors::Subtract(const FPoint3& startingPoint, const FPoint3& endPoint)
	{
		FPoint3 delta{};
		delta.X = endPoint.X - startingPoint.X;
		delta.Y = endPoint.Y - startingPoint.Y;
		return delta;
	}

	const FPoint3f SVectors::Subtract(const FPoint3f& startingPoint, const FPoint3f& endPoint)
	{
		FPoint3f delta{};
		delta.X = endPoint.X - startingPoint.X;
		delta.Y = endPoint.Y - startingPoint.Y;
		return delta;
	}

	/*

		ROTATORS

	*/

	const FRotation3 SRotators::CombineRotations(const FRotation3& firstRotation, const FRotation3& secondRotation)
	{

		FRotation3 desiredRotation;

		desiredRotation.X = { STools::Loop(firstRotation.X + secondRotation.X, 0.0, 360.0) };
		desiredRotation.Y = { STools::Loop(firstRotation.Y + secondRotation.Y, 0.0, 360.0) };
		desiredRotation.Z = { STools::Loop(firstRotation.Z + secondRotation.Z, 0.0, 360.0) };

		return desiredRotation;
	}

	const FRotation3 SRotators::ConstraintRotation(const FRotation3& rotation)
	{
		FRotation3 desiredRotation;

		desiredRotation.X = { STools::Loop(rotation.X, 0.0, 360.0) };
		desiredRotation.Y = { STools::Loop(rotation.Y, 0.0, 360.0) };
		desiredRotation.Z = { STools::Loop(rotation.Z, 0.0, 360.0) };

		return desiredRotation;
	}


}