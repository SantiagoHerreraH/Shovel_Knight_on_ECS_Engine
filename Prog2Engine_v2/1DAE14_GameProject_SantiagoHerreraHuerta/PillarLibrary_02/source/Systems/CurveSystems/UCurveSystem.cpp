
#include "../../../include/Systems/CurveSystems/UCurveSystem.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/STools.h"

namespace Pillar {

	/*
	
		CURVES
	
	*/

	const UCurveEvaluator SCurves::CurveEvaluator{};
	const UCurveCompressor SCurves::CurveCompressor{};
	const UCompressedCurveEvaluator SCurves::CompressedCurveEvaluator{};

	/*

		CURVE EVALUATOR

	*/

	UCurveEvaluator::UCurveEvaluator()
	{
	}

	const double UCurveEvaluator::EvaluateCurve(const D_Curve_Container& curveContainer, const size_t curveIndex, const double value) const
	{

		//first CHECK if the curve has the value you are looking for
		if (curveIndex >= curveContainer.size())
		{
			//put a debug comment here
			return -1;
		}

		return EvaluateCurve(curveContainer[curveIndex], value);
	}

	const double UCurveEvaluator::EvaluateCurve(const DCurve& curveToEvaluate, const double value) const
	{

		if (value > curveToEvaluate[curveToEvaluate.size() - 1].Point.X ||
			value < curveToEvaluate[0].Point.X)
		{
			//put a debug comment here
			return -1;
		}

		if (curveToEvaluate.size() == 1)
		{
			return curveToEvaluate.at(0).Point.Y;
		}

		//binary search
		size_t currentIndex{ curveToEvaluate.size() / 2 };
		size_t maxIndex{ curveToEvaluate.size() - 1 };
		size_t minIndex{ 0 };
		bool found{ false };

		while (!found)
		{
			currentIndex = ((maxIndex - minIndex) / 2) + minIndex;

			if (value == curveToEvaluate[currentIndex].Point.X)
			{
				minIndex = currentIndex;
				maxIndex = currentIndex >= curveToEvaluate.size() - 1 ? currentIndex : currentIndex + 1;

				found = true;

				break;
			}
			else if (maxIndex - minIndex <= 1)
			{
				found = true;

				break;
			}


			if (value < curveToEvaluate[currentIndex].Point.X)
			{
				maxIndex = currentIndex;

			}
			else {
				minIndex = currentIndex;
			}

		}


		//Calculate the Tangent points
		FPoint2f leftTangent = SVectors::ObtainRelativeVectorComponent(
			curveToEvaluate[minIndex].RightTangentPoint,
			curveToEvaluate[minIndex].Point);

		FPoint2f rightTangent = SVectors::ObtainRelativeVectorComponent(
			curveToEvaluate[maxIndex].LeftTangentPoint,
			curveToEvaluate[maxIndex].Point);

		//calculate the lerp percentage
		double percentage =
			(value -
				curveToEvaluate[minIndex].Point.X) /
			static_cast<double>(
				curveToEvaluate[maxIndex].Point.X -
				curveToEvaluate[minIndex].Point.X);

		//lerp between all of the points
		FPoint2f lerpPointA =
			STools::CalculateLerpPoint
			(curveToEvaluate[minIndex].Point,
				leftTangent,
				percentage);

		FPoint2f lerpPointB =
			STools::CalculateLerpPoint
			(rightTangent,
				curveToEvaluate[maxIndex].Point,
				percentage);

		FPoint2f lerpPointC =
			STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


		FPoint2f lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
		FPoint2f lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

		FPoint2f finalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);


		return finalLerpPoint.Y;
	}

	const double UCurveEvaluator::EvaluateCurve(const FControlledCurve& controlledCurve, const double value)const
	{
		if (controlledCurve.Curve.empty())
		{
			return 0.;
		}

		double min{ controlledCurve.Curve.at(0).Point.X};
		double max{ controlledCurve.Curve.at(controlledCurve.Curve.size() - 1).Point.X};
		double valueAfterOperation{};

		switch (controlledCurve.CurveBehavior)
		{
		case EValueBehavior::Rebound:
			valueAfterOperation = STools::Rebound(value, min, max);
			break;
		case EValueBehavior::Clamp:
			valueAfterOperation = STools::Clamp(value, min, max);
			break;
		case EValueBehavior::Loop: 
		default:
			valueAfterOperation = STools::Loop(value, min, max);
			break;
		}

		return EvaluateCurve(controlledCurve.Curve, valueAfterOperation);;
	}

	const double UCurveEvaluator::GetMaxCurveValue(const D_Curve_Container& curveContainer, const size_t curveIndex) const
	{
		if (curveIndex >= curveContainer.size())
		{
			//put a debug comment here
			return -1;
		}

		return GetMaxCurveValue(curveContainer[curveIndex]);
	}

	const double UCurveEvaluator::GetMinCurveValue(const D_Curve_Container& curveContainer, const size_t curveIndex) const
	{
		if (curveIndex >= curveContainer.size())
		{
			//put a debug comment here
			return -1;
		}

		return GetMinCurveValue(curveContainer[curveIndex]);
	}

	const double UCurveEvaluator::GetMaxCurveValue(const DCurve& curve) const
	{
		if (curve.size() <= 0)
		{
			//put a debug comment here
			return 0;
		}

		return curve[curve.size() - 1].Point.X;
	}

	const double UCurveEvaluator::GetMinCurveValue(const DCurve& curve) const
	{
		if (curve.size() <= 0)
		{
			//put a debug comment here
			return 0;
		}

		return curve[0].Point.X;
	}

	/*

		CURVE Compresser

	*/


	UCurveCompressor::UCurveCompressor()
	{
	}

	const bool UCurveCompressor::CompressCurve(const DCurve& curve, const unsigned int numberOfTotalSegments, DShape2Df& outVectorArray)
	{

		if (numberOfTotalSegments <= 0)
		{
			//debug comment here
			return false;
		}

		outVectorArray.clear();
		//there is always one more point than the number of segments
		outVectorArray.reserve(numberOfTotalSegments + 1);

		double maxMinDelta = m_CurveEvaluator.GetMaxCurveValue(curve) - m_CurveEvaluator.GetMinCurveValue(curve);
		double delta = maxMinDelta / numberOfTotalSegments;

		float currentXValue = m_CurveEvaluator.GetMinCurveValue(curve);
		float currentYValue{};

		for (size_t i = 0; i <= numberOfTotalSegments; i++)
		{
			currentYValue = m_CurveEvaluator.EvaluateCurve(curve, currentXValue);
			currentXValue += delta;

			outVectorArray.emplace_back(FPoint2f{ currentXValue , currentYValue });
		}

		return true;
	}

	const bool UCurveCompressor::CompressCurve(const D_Curve_Container& curveContainer, const size_t curveIndex, const unsigned int numberOfTotalSegments, DShape2Df& outVectorArray)
	{
		if (curveIndex >= curveContainer.size())
		{
			//debug comment here
			return false;
		}

		return CompressCurve(curveContainer[curveIndex], numberOfTotalSegments, outVectorArray);
	}

	const bool UCurveCompressor::CompressCurveContainer(const D_Curve_Container& curveContainer, const unsigned int numberOfTotalSegments, D_Shape2D_Container& outCompressedCurve)
	{
		if (curveContainer.size() <= 0)
		{
			return false;
		}

		outCompressedCurve.clear();
		outCompressedCurve.reserve((numberOfTotalSegments + 1) * curveContainer.size());

		for (size_t i = 0; i < curveContainer.size(); i++)
		{
			outCompressedCurve.emplace_back();
			CompressCurve(curveContainer[i], numberOfTotalSegments, outCompressedCurve[i]);
		}
		return true;
	}

	/*
		COMPRESSED CURVE EVALUATOR
	*/

	UCompressedCurveEvaluator::UCompressedCurveEvaluator()
	{
	}

	const double UCompressedCurveEvaluator::EvaluateCompressedCurve(const D_Shape2D_Container& compressedCurveContainer, const size_t curveIndex, const double value)
	{
		if (curveIndex >= compressedCurveContainer.size())
		{
			//debug comment here
			return 0;
		}
		if (compressedCurveContainer.size() <= 0)
		{
			//debug comment here
			return 0;
		}

		return EvaluateCompressedCurve(compressedCurveContainer[curveIndex], value);
	}

	const double UCompressedCurveEvaluator::EvaluateCompressedCurve(const DShape2Df& arrayOfPoints, const double value)
	{
		if (arrayOfPoints.size() <= 0)
		{
			//debug comment here
			return 0;
		}
		if (value > arrayOfPoints[arrayOfPoints.size() - 1].X)
		{
			//debug comment here
			return 0;
		}

		//binary search
		size_t currentIndex{ arrayOfPoints.size() / 2 };
		size_t maxIndex{ arrayOfPoints.size() - 1 };
		size_t minIndex{ 0 };
		bool found{ false };

		while (!found)
		{
			currentIndex = ((maxIndex - minIndex) / 2) + minIndex;

			if (value == arrayOfPoints[currentIndex].X)
			{
				minIndex = currentIndex;
				maxIndex = currentIndex >= arrayOfPoints.size() - 1 ? currentIndex : currentIndex + 1;

				found = true;

				break;
			}
			else if (maxIndex - minIndex <= 1)
			{
				found = true;

				break;
			}


			if (value < arrayOfPoints[currentIndex].X)
			{
				maxIndex = currentIndex;

			}
			else {
				minIndex = currentIndex;
			}

		}

		//calculate the lerp percentage
		double percentage =
			(value -
				arrayOfPoints[minIndex].X) /
			static_cast<double>(
				arrayOfPoints[maxIndex].X -
				arrayOfPoints[minIndex].X);

		return STools::CalculateLerpValue(arrayOfPoints[minIndex].X, arrayOfPoints[minIndex].X, percentage);

	}

}