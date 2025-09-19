

#include "../../../include/Systems/MathSystems/SMatrix.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/SAngles.h"

namespace Pillar {

	SMatrix::SMatrix()
	{
	}

	void SMatrix::Translate(FMatrix4x4f& in, const FPoint2f& translation, bool lastTransformation)
	{
		FMatrix4x4f translationMatrix{ };
		translationMatrix.Values[0][3] = translation.X;
		translationMatrix.Values[1][3] = translation.Y;

		if (lastTransformation)
		{
			Multiply(translationMatrix, in);
		}
		else
		{
			Multiply(in, translationMatrix);
			in = translationMatrix;
		}
		
	}

	void SMatrix::FlatRotateX(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::FlatRotateY(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[1][1] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::FlatRotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[2][2] = cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateX(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[1][1] =  cos(radians);
		rotationMatrix.Values[1][2] = -sin(radians);
		rotationMatrix.Values[2][1] =  sin(radians);
		rotationMatrix.Values[2][2] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateY(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] =  cos(radians);
		rotationMatrix.Values[2][0] = -sin(radians);
		rotationMatrix.Values[0][2] =  sin(radians);
		rotationMatrix.Values[2][2] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
	}

	void SMatrix::RotateZ(FMatrix4x4f& in, double rotation, bool lastTransformation)
	{
		FMatrix4x4f rotationMatrix{};
		double radians(SAngles::InRadians(rotation));
		rotationMatrix.Values[0][0] =  cos(radians);
		rotationMatrix.Values[0][1] = -sin(radians);
		rotationMatrix.Values[1][0] =  sin(radians);
		rotationMatrix.Values[1][1] =  cos(radians);

		if (lastTransformation)
		{
			Multiply(rotationMatrix, in);
		}
		else
		{
			Multiply(in, rotationMatrix);
			in = rotationMatrix;
		}
		
	}

	void SMatrix::Scale(FMatrix4x4f& in, const FPoint2f& scale, bool lastTransformation)
	{
		FMatrix4x4f scaleMatrix{};
		scaleMatrix.Values[0][0] = scale.X;
		scaleMatrix.Values[1][1] = scale.Y;

		if (lastTransformation)
		{
			Multiply(scaleMatrix, in);
		}
		else
		{
			Multiply(in, scaleMatrix);
			in = scaleMatrix;
		}
		
	}

	void SMatrix::ApplyTransform(FRectCollider& rectCollider, const FTransform2D& transform)
	{

		float rotatedHeight{ rectCollider.OriginalRect.Height * cosf(SAngles::InRadians(transform.RotationX)) };
		float rotatedWidth{ rectCollider.OriginalRect.Width * cosf(SAngles::InRadians(transform.RotationY)) };

		rectCollider.CurrentRect.Top =  std::round((rectCollider.OriginalRect.Top * transform.Scale.Y) + transform.Position.Y);
		rectCollider.CurrentRect.Left = std::round((rectCollider.OriginalRect.Left * transform.Scale.X) + transform.Position.X);

		rectCollider.CurrentRect.Height = std::round(abs(rotatedHeight) * transform.Scale.Y);
		rectCollider.CurrentRect.Width =  std::round(abs(rotatedWidth) * transform.Scale.X );
	}

	void SMatrix::ApplyTransform(FMatrix4x4f& in, const FTransform2D& transform, bool lastTransformation)
	{
		if (lastTransformation)
		{
			Scale(in, transform.Scale);
			RotateX(in, transform.RotationX);
			RotateY(in, transform.RotationY);
			RotateZ(in, transform.RotationZ);
			Translate(in, transform.Position);
		}
		else
		{
			Translate(in, transform.Position, lastTransformation);
			RotateZ(in, transform.RotationZ, lastTransformation);
			RotateY(in, transform.RotationY, lastTransformation);
			RotateX(in, transform.RotationX, lastTransformation);
			Scale(in, transform.Scale, lastTransformation);
		}
		
	}

	void SMatrix::ToTransform(const FMatrix4x4f& matrix, FTransform2D& outTransform)
	{
		outTransform.Position = { matrix.Values[0][3], matrix.Values[1][3] };

		outTransform.Scale.X = SVectors::Length(FPoint3f{ matrix.Values[0][0], matrix.Values[1][0], matrix.Values[2][0] });
		outTransform.Scale.Y = SVectors::Length(FPoint3f{ matrix.Values[0][1], matrix.Values[1][1], matrix.Values[2][1] });

		const double radiansZ = atan2(-matrix.Values[0][1], matrix.Values[0][0]);
		outTransform.RotationZ = SAngles::InDegrees(radiansZ);

		const double radiansY = asin(matrix.Values[0][2] / outTransform.Scale.X);
		outTransform.RotationY = SAngles::InDegrees(radiansY);

		const double radiansX = atan2(matrix.Values[2][1], matrix.Values[2][2]);
		outTransform.RotationX = SAngles::InDegrees(radiansX);
		
	}

	void SMatrix::Inverse(const DMatrix2x2f& in, DMatrix2x2f& out)
	{
		FPoint2f rightVector{ in[0][0],in[1][0] };
		FPoint2f upVector{ in[0][1],in[1][1] };
		float determinant{ SVectors::Determinant(rightVector, upVector) };
		float scalar = determinant == 0 ? 0 : 1.0f / determinant;
		
		out[0][0] =  in[1][1];
		out[0][1] = -in[0][1];
		out[1][0] = -in[1][0];
		out[1][1] =  in[0][0];

		Multiply(out, scalar);
	}

	void SMatrix::Inverse(DMatrix2x2f& matrixToInverse)
	{
		FPoint2f rightVector{ matrixToInverse[0][0],matrixToInverse[1][0] };
		FPoint2f upVector	{ matrixToInverse[0][1],matrixToInverse[1][1]};
		float determinant{ SVectors::Determinant(rightVector, upVector) };
		float scalar = determinant == 0 ? 0 : 1.0f / determinant;

		float temp{ matrixToInverse[0][0] };
		

		matrixToInverse[0][0] = matrixToInverse[1][1];
		matrixToInverse[0][1] *= -1;
		matrixToInverse[1][0] *= -1;
		matrixToInverse[1][1] = temp;

		Multiply(matrixToInverse, scalar);
		
	}

	void SMatrix::MakeIdentity(FMatrix4x4f& matrix)
	{
		for (size_t rowIdx = 0; rowIdx < 4; rowIdx++)
		{
			for (size_t columnIdx = 0; columnIdx < 4; columnIdx++)
			{
				if (rowIdx == columnIdx)
				{
					matrix.Values[rowIdx][columnIdx] = 1;
				}
				else
				{
					matrix.Values[rowIdx][columnIdx] = 0;
				}
			}
		}
	}

	void SMatrix::Multiply(DMatrix2x2f& matrix, const float value)
	{
		//for each row
		for (size_t currentRowIndex = 0; currentRowIndex < matrix.size(); currentRowIndex++)
		{
			//for each column
			for (size_t currentColumnIndex = 0; currentColumnIndex < matrix[currentRowIndex].size(); currentColumnIndex++)
			{
				matrix[currentRowIndex][currentColumnIndex] *= value;
			}
		}
	}

	void SMatrix::Multiply(DMatrix2x2f& first, const FPoint2f& second, FPoint2f& out)
	{
		out.X = second.X * first[0][0] + second.Y * first[0][1];
		out.Y = second.X * first[1][0] + second.Y * first[1][1];
	}

	void SMatrix::Multiply(const FMatrix4x4f& first, FMatrix4x4f& outSecond)
	{
		FMatrix4x4f result{};
		for (size_t resultColumnIdx = 0; resultColumnIdx < 4; resultColumnIdx++)
		{
			for (size_t resultRowIdx = 0; resultRowIdx < 4; resultRowIdx++)
			{

				result.Values[resultRowIdx][resultColumnIdx] = 0;

				for (size_t currentMultiplication = 0; currentMultiplication < 4; currentMultiplication++)
				{
					result.Values[resultRowIdx][resultColumnIdx] +=
						first.Values[resultRowIdx][currentMultiplication] *
						outSecond.Values[currentMultiplication][resultColumnIdx];
				}
				
			}
		}

		outSecond.Values.swap(result.Values);
	}

	void SMatrix::Multiply(const FMatrix4x4f& matrix, const FPoint3f& vector, FPoint3f& outVector)
	{
		outVector.X = vector.X * matrix.Values[0][0] + vector.Y * matrix.Values[0][1] + vector.Z * matrix.Values[0][2] + matrix.Values[0][3];
		outVector.Y = vector.X * matrix.Values[1][0] + vector.Y * matrix.Values[1][1] + vector.Z * matrix.Values[1][2] + matrix.Values[1][3];
		outVector.Z = vector.X * matrix.Values[2][0] + vector.Y * matrix.Values[2][1] + vector.Z * matrix.Values[2][2] + matrix.Values[2][3];
	}

	void SMatrix::Multiply(const FMatrix4x4f& matrix, const FPoint2f& vector, FPoint2f& outVector)
	{
		FPoint3f vector3{vector.X, vector.Y, 1.f};
		FPoint3f outVector3{};
		Multiply(matrix, vector3, outVector3);
		outVector.X = outVector3.X;
		outVector.Y = outVector3.Y;
	}

	void SMatrix::Multiply(const DMatrix4x4f& first, const  FPoint4f& second, FPoint4f& out)
	{

		out.X = second.X * first[0][0] + second.Y * first[0][1] + second.Z * first[0][2] + second.W * first[0][3];
		out.Y = second.X * first[1][0] + second.Y * first[1][1] + second.Z * first[1][2] + second.W * first[1][3];
		out.Z = second.X * first[2][0] + second.Y * first[2][1] + second.Z * first[2][2] + second.W * first[2][3];
		out.W = second.X * first[3][0] + second.Y * first[3][1] + second.Z * first[3][2] + second.W * first[3][3];
	}

	/*
		MULTIPLY COLUMNS
	*/

	void SMatrix::MultiplyColumns(const FPoint4f& first, const DMatrix4x4f& second, DMatrix4x4f& out)
	{
		const int numberOfDimensions = 4;
		size_t num = 0;
		float vector[numberOfDimensions] = {first.X, first.Y, first.Z, first.W};

		for (size_t i = 0; i < numberOfDimensions; i++)
		{
			for (size_t j = 0; j < numberOfDimensions; j++)
			{
				out[j][num] = vector[i] * second[j][num];

			}

			num++;
		}

	}

	/*
		ADD ROWS
	*/

	void SMatrix::AddRows(const DMatrix4x4f& matrix, FPoint4f& out)
	{
		out.X = matrix[0][0] + matrix[1][0] + matrix[2][0] + matrix[3][0];
		out.Y = matrix[0][1] + matrix[1][1] + matrix[2][1] + matrix[3][1];
		out.Z = matrix[0][2] + matrix[1][2] + matrix[2][2] + matrix[3][2];
		out.W = matrix[0][3] + matrix[1][3] + matrix[2][3] + matrix[3][3];
	}

	const bool SMatrix::SetRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value)
	{
		if (rowIndex < 0 || rowIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[rowIndex][0] = value.X;
		matrix[rowIndex][1] = value.Y;
		matrix[rowIndex][2] = value.Z;
		matrix[rowIndex][3] = value.W;


		return true;
	}

	const bool SMatrix::SetColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value)
	{
		if (columnIndex < 0 || columnIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[0][columnIndex] = value.X;
		matrix[1][columnIndex] = value.Y;
		matrix[2][columnIndex] = value.Z;
		matrix[3][columnIndex] = value.W;

		return true;
	}

	const bool SMatrix::AddRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value)
	{
		if (rowIndex < 0 || rowIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[rowIndex][0] += value.X;
		matrix[rowIndex][1] += value.Y;
		matrix[rowIndex][2] += value.Z;
		matrix[rowIndex][3] += value.W;

		return true;
	}

	const bool SMatrix::AddColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value)
	{
		if (columnIndex < 0 || columnIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[0][columnIndex] += value.X;
		matrix[1][columnIndex] += value.Y;
		matrix[2][columnIndex] += value.Z;
		matrix[3][columnIndex] += value.W;

		return true;
	}

	const bool SMatrix::MultiplyRowValues(DMatrix4x4f& matrix, const int rowIndex, const FPoint4& value)
	{
		if (rowIndex < 0 || rowIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[rowIndex][0] *= value.X;
		matrix[rowIndex][1] *= value.Y;
		matrix[rowIndex][2] *= value.Z;
		matrix[rowIndex][3] *= value.W;

		return true;
	}

	const bool SMatrix::MultiplyColumnValues(DMatrix4x4f& matrix, const int columnIndex, const FPoint4& value)
	{
		if (columnIndex < 0 || columnIndex > 3)
		{
			//debug comment
			return false;
		}

		matrix[0][columnIndex] *= value.X;
		matrix[1][columnIndex] *= value.Y;
		matrix[2][columnIndex] *= value.Z;
		matrix[3][columnIndex] *= value.W;

		return true;
	}

}