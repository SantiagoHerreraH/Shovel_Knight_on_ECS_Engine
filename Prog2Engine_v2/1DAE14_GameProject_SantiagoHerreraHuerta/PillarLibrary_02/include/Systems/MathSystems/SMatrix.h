#pragma once

#include "../../../include/Structs/FVector.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FMatrix.h"

namespace Pillar {

	class SMatrix {

	public:
		SMatrix();


		static void Translate(FMatrix4x4f& in, const FPoint2f& translation, bool lastTransformation = true);
		static void FlatRotateX(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void FlatRotateY(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void FlatRotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation = true);//This is like Reflection
		static void RotateX(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void RotateY(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void RotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation = true);
		static void Scale(FMatrix4x4f& in, const FPoint2f& scale, bool lastTransformation = true);

		static void ApplyTransform(FRectCollider& rectCollider, const FTransform2D& transform);
		static void ApplyTransform(FMatrix4x4f& in, const FTransform2D& transform, bool lastTransformation = true);
		static void ToTransform(const FMatrix4x4f& matrix, FTransform2D& outTransform);

		//Operations
		static void Inverse(const DMatrix2x2f& in, DMatrix2x2f& out);
		static void Inverse(DMatrix2x2f& matrixToInverse);

		static void MakeIdentity(FMatrix4x4f& matrix);

		//Matrix Multiplication
		//static void Multiply(const DMatrix2x2f& first, const DMatrix2x2f& second, DMatrix2x2f& out);
		//static void Multiply(DMatrix2x2f& firstOut, const DMatrix2x2f& second);
		static void Multiply(DMatrix2x2f& matrix, const float value);
		static void Multiply(DMatrix2x2f& first, const FPoint2f& second, FPoint2f& out);

		static void Multiply(const FMatrix4x4f& first, FMatrix4x4f& outSecond);
		static void Multiply(const FMatrix4x4f& matrix, const FPoint3f& vector, FPoint3f& outVector);
		static void Multiply(const FMatrix4x4f& matrix, const FPoint2f& vector, FPoint2f& outVector);

		static void Multiply(const DMatrix4x4f& first, const  FPoint4f& second, FPoint4f& out);
		static void MultiplyColumns(const FPoint4f& first, const DMatrix4x4f& second, DMatrix4x4f& out);
		static void AddRows(const DMatrix4x4f& matrix, FPoint4f& out);

		//Matrix Modification
		static const bool SetRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value);
		static const bool SetColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value);

		static const bool AddRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value);
		static const bool AddColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value);

		static const bool MultiplyRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value);
		static const bool MultiplyColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value);

	};

}