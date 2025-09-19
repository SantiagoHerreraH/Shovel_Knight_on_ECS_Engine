
#include "../../../include/Systems/CurveSystems/USplineSystem.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/STools.h"

namespace Pillar {

	/*

		SPLINE CREATOR

	*/

	USplineCreator::USplineCreator()
	{
	}

	const bool USplineCreator::AddSpline(DSpline& spline, D_Spline_Container& splineContainer)
	{
		splineContainer.emplace_back(spline);
		return calculateSplinePointsDistanceFromOriginSplinePoint(splineContainer);
	}

	const bool USplineCreator::DeleteSpline(const int splineIndex, D_Spline_Container& splineContainer)
	{
		if (splineIndex < 0)
		{
			//debug message here
			return false;
		}
		if (splineIndex >= splineContainer.size())
		{
			//debug message here
			return false;
		}

		splineContainer.erase(splineContainer.cbegin() + splineIndex);
		return calculateSplinePointsDistanceFromOriginSplinePoint(splineContainer);
	}

	const bool USplineCreator::AddSplinePoint(FSplinePoint& splinePoint, DSpline& splineRef)
	{
		splineRef.emplace_back(splinePoint);
		return calculateSplinePointsDistanceFromOriginSplinePoint(splineRef);
	}

	const bool USplineCreator::DeleteSplinePoint(const int splinePointIndex, DSpline& splineRef)
	{
		if (splinePointIndex < 0)
		{
			//debug message here
			return false;
		}
		if (splinePointIndex >= splineRef.size())
		{
			//debug message here
			return false;
		}

		splineRef.erase(splineRef.cbegin() + splinePointIndex);
		return calculateSplinePointsDistanceFromOriginSplinePoint(splineRef);
	}



	//PRIVATE

	const bool USplineCreator::calculateSplinePointsDistanceFromOriginSplinePoint(D_Spline_Container& splineContainer, const unsigned int numberOfSegmentsBetweenSplinePoints)
	{

		if (splineContainer.size() <= 0)
		{
			//debug message here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints <= 0)
		{
			//debug message here
			return false;
		}

		for (size_t i = 0; i < splineContainer.size(); i++)
		{
			if (!calculateSplinePointsDistanceFromOriginSplinePoint(splineContainer[i], numberOfSegmentsBetweenSplinePoints))
			{
				return false;
			}
		}
		return true;
	}

	const bool USplineCreator::calculateSplinePointsDistanceFromOriginSplinePoint(DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints)
	{
		if (spline.size() <= 0)
		{
			//debug message here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints <= 0)
		{
			//debug message here
			return false;
		}

		FPoint2f leftTangent{};
		FPoint2f rightTangent{};

		double currentDistanceDeltaBetweenPoints{};
		double currentAccumulatedDelta{};
		double percentage{};

		FPoint2f lerpPointA{};
		FPoint2f lerpPointB{};
		FPoint2f lerpPointC{};
		FPoint2f lerpPointD{};
		FPoint2f lerpPointE{};

		FPoint2f currentFinalLerpPoint{};
		FPoint2f pastFinalLerpPoint{ spline[0].CurvePoint.Point };
		FPoint2f deltaFinalLerpPoint{};

		double currentDistanceBetweenOriginAndCurrentPoint{};

		spline[0].DistanceFromStart = 0;



		//for each spline point
		for (size_t currentSplinePointIndex = 1; currentSplinePointIndex < spline.size(); currentSplinePointIndex++)
		{

			FSplinePoint& pastPoint = spline[currentSplinePointIndex - 1];
			FSplinePoint& currentPoint = spline[currentSplinePointIndex];

			//Calculate the Tangent points
			leftTangent = SVectors::ObtainRelativeVectorComponent(
				pastPoint.CurvePoint.RightTangentPoint,
				pastPoint.CurvePoint.Point);

			rightTangent = SVectors::ObtainRelativeVectorComponent(
				currentPoint.CurvePoint.LeftTangentPoint,
				currentPoint.CurvePoint.Point);

			currentDistanceDeltaBetweenPoints = SVectors::Distance(currentPoint.CurvePoint.Point, pastPoint.CurvePoint.Point);

			//Lerp while the accumulated delta is lower than the delta between current and next point
			while (currentAccumulatedDelta < currentDistanceDeltaBetweenPoints)
			{
				//calculate the lerp percentage

				currentAccumulatedDelta += currentDistanceDeltaBetweenPoints / numberOfSegmentsBetweenSplinePoints;
				percentage = currentAccumulatedDelta / currentDistanceDeltaBetweenPoints;

				//lerp between all of the points
				lerpPointA =
					STools::CalculateLerpPoint
					(pastPoint.CurvePoint.Point,
						leftTangent,
						percentage);

				lerpPointB =
					STools::CalculateLerpPoint
					(rightTangent,
						currentPoint.CurvePoint.Point,
						percentage);

				lerpPointC =
					STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


				lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
				lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

				currentFinalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);

				deltaFinalLerpPoint = SVectors::Subtract(currentFinalLerpPoint, pastFinalLerpPoint);

				pastFinalLerpPoint = currentFinalLerpPoint;

				currentDistanceBetweenOriginAndCurrentPoint += STrigonometry::AbsoluteHypotenuse(deltaFinalLerpPoint.X, deltaFinalLerpPoint.Y);
			}

			currentAccumulatedDelta = 0;
			currentPoint.DistanceFromStart = currentDistanceBetweenOriginAndCurrentPoint + pastPoint.DistanceFromStart;

		}

		return true;
	}

	/*

		SPLINE EVALUATOR

	*/

	USplineEvaluator::USplineEvaluator() {}

	const FPoint2f USplineEvaluator::EvaluateSpline(const D_Spline_Container& splineContainer, const size_t splineIndex, const double percentageFromOneToZero) const
	{
		if (splineContainer.size() <= 0)
		{
			//put a debug comment here
			return { 0,0 };
		}
		if (splineIndex >= splineContainer.size())
		{
			//put a debug comment here
			return { 0,0 };
		}

		return EvaluateSpline(splineContainer[splineIndex], percentageFromOneToZero);
	}

	const FPoint2f USplineEvaluator::EvaluateSpline(const DSpline& splineToEvaluate, const double percentageFromOneToZero) const
	{

		if (percentageFromOneToZero < 0 || percentageFromOneToZero > 1)
		{
			//put a debug comment here
			return { 0,0 };
		}
		if (splineToEvaluate.empty())
		{
			//put a debug comment here
			return { 0,0 };
		}

		double distanceOfSplineAtPercentage = percentageFromOneToZero * splineToEvaluate[splineToEvaluate.size() - 1].DistanceFromStart;

		//binary search
		size_t currentIndex{ splineToEvaluate.size() / 2 };
		size_t maxIndex{ splineToEvaluate.size() - 1 };
		size_t minIndex{ 0 };
		bool found{ false };

		while (!found)
		{
			currentIndex = ((maxIndex - minIndex) / 2) + minIndex;

			if (distanceOfSplineAtPercentage == splineToEvaluate[currentIndex].DistanceFromStart)
			{
				minIndex = currentIndex;
				maxIndex = currentIndex >= splineToEvaluate.size() - 1 ? currentIndex : currentIndex + 1;

				found = true;

				break;
			}
			else if (maxIndex - minIndex <= 1)
			{
				found = true;

				break;
			}


			if (distanceOfSplineAtPercentage < splineToEvaluate[currentIndex].DistanceFromStart)
			{
				maxIndex = currentIndex;

			}
			else {
				minIndex = currentIndex;
			}

		}


		//Calculate the Tangent points
		FPoint2f leftTangent = SVectors::ObtainRelativeVectorComponent(
			splineToEvaluate[minIndex].CurvePoint.RightTangentPoint,
			splineToEvaluate[minIndex].CurvePoint.Point);

		FPoint2f rightTangent = SVectors::ObtainRelativeVectorComponent(
			splineToEvaluate[maxIndex].CurvePoint.LeftTangentPoint,
			splineToEvaluate[maxIndex].CurvePoint.Point);

		//calculate the lerp percentage
		double percentage =
			(distanceOfSplineAtPercentage -
				splineToEvaluate[minIndex].DistanceFromStart) /
			(splineToEvaluate[maxIndex].DistanceFromStart -
				splineToEvaluate[minIndex].DistanceFromStart);

		//lerp between all of the points
		FPoint2f lerpPointA =
			STools::CalculateLerpPoint
			(splineToEvaluate[minIndex].CurvePoint.Point,
				leftTangent,
				percentage);

		FPoint2f lerpPointB =
			STools::CalculateLerpPoint
			(rightTangent,
				splineToEvaluate[maxIndex].CurvePoint.Point,
				percentage);

		FPoint2f lerpPointC =
			STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


		FPoint2f lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
		FPoint2f lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

		FPoint2f finalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);


		return finalLerpPoint;
	}

	const double USplineEvaluator::GetSplineTotalLength(const D_Spline_Container& splineContainer, const size_t splineIndex) const
	{

		if (splineContainer.size() <= 0)
		{
			//put a debug comment here
			return 0;
		}
		if (splineIndex >= splineContainer.size())
		{
			//put a debug comment here
			return 0;
		}
		return GetSplineTotalLength(splineContainer[splineIndex]);
	}

	const double USplineEvaluator::GetSplineTotalLength(const DSpline& spline) const
	{
		if (spline.size() <= 0)
		{
			//put a debug comment here
			return 0;
		}

		return spline[spline.size() - 1].DistanceFromStart;
	}

	const double USplineEvaluator::GetLengthOfSplinePercentage(const D_Spline_Container& splineContainer, const size_t splineIndex, const double percentageFromOneToZero) const
	{
		return GetSplineTotalLength(splineContainer, splineIndex) * percentageFromOneToZero;
	}

	const double USplineEvaluator::GetLengthOfSplinePercentage(const DSpline& spline, const double percentageFromOneToZero) const
	{
		return GetSplineTotalLength(spline) * percentageFromOneToZero;
	}


	/*

		COMPRESSED SPLINE

	*/

	USplineCompressor::USplineCompressor()
	{
	}

	const bool USplineCompressor::CompressSpline
	(const D_Spline_Container& splineContainer, const size_t splineIndex, const unsigned int numberOfSegmentsBetweenSplinePoints, DOrderedShape2D& outVectorArray) const
	{
		if (splineIndex >= splineContainer.size())
		{
			//Debug comment here
			return false;
		}
		if (splineContainer.size() <= 0)
		{
			//Debug comment here
			return false;
		}

		return CompressSpline(splineContainer[splineIndex], numberOfSegmentsBetweenSplinePoints, outVectorArray);
	}

	const bool USplineCompressor::CompressSpline
	(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DOrderedShape2D& outVectorArray)const
	{
		if (spline.size() <= 0)
		{
			//Debug comment here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints <= 0)
		{
			//Debug comment here
			return false;
		}

		outVectorArray.clear();
		outVectorArray.reserve((numberOfSegmentsBetweenSplinePoints + 1) * spline.size());

		FPoint2f leftTangent{};
		FPoint2f rightTangent{};

		double currentDistanceDeltaBetweenPoints{};
		double currentAccumulatedDelta{ 0. };
		double percentage{};

		FPoint2f lerpPointA{};
		FPoint2f lerpPointB{};
		FPoint2f lerpPointC{};
		FPoint2f lerpPointD{};
		FPoint2f lerpPointE{};

		FPoint2f currentFinalLerpPoint{};
		FPoint2f pastFinalLerpPoint{};
		FPoint2f deltaFinalLerpPoint{};

		double currentDistanceBetweenOriginAndCurrentPoint{ 0. };

		//for each spline point
		for (size_t currentSplinePointIndex = 0; currentSplinePointIndex < spline.size() - 1; currentSplinePointIndex++)
		{

			const FSplinePoint& currentPoint = spline[currentSplinePointIndex];
			const FSplinePoint& nextPoint = spline[currentSplinePointIndex + 1];

			pastFinalLerpPoint = spline[currentSplinePointIndex].CurvePoint.Point;

			//Calculate the Tangent points
			leftTangent = SVectors::ObtainRelativeVectorComponent(
				currentPoint.CurvePoint.RightTangentPoint,
				currentPoint.CurvePoint.Point);

			rightTangent = SVectors::ObtainRelativeVectorComponent(
				nextPoint.CurvePoint.LeftTangentPoint,
				nextPoint.CurvePoint.Point);

			currentDistanceDeltaBetweenPoints = nextPoint.DistanceFromStart - currentPoint.DistanceFromStart;

			//Lerp while the accumulated delta is lower than the delta between current and next point
			while (currentAccumulatedDelta < currentDistanceDeltaBetweenPoints)
			{
				//calculate the lerp percentage

				outVectorArray.emplace_back(FPoint2AndLength{ pastFinalLerpPoint,currentDistanceBetweenOriginAndCurrentPoint });

				currentAccumulatedDelta += currentDistanceDeltaBetweenPoints / numberOfSegmentsBetweenSplinePoints;
				percentage = currentAccumulatedDelta / currentDistanceDeltaBetweenPoints;

				//lerp between all of the points
				lerpPointA =
					STools::CalculateLerpPoint
					(currentPoint.CurvePoint.Point,
						leftTangent,
						percentage);

				lerpPointB =
					STools::CalculateLerpPoint
					(rightTangent,
						nextPoint.CurvePoint.Point,
						percentage);

				lerpPointC =
					STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


				lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
				lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

				currentFinalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);

				deltaFinalLerpPoint = SVectors::Subtract(currentFinalLerpPoint, pastFinalLerpPoint);

				pastFinalLerpPoint = currentFinalLerpPoint;

				currentDistanceBetweenOriginAndCurrentPoint += STrigonometry::AbsoluteHypotenuse(deltaFinalLerpPoint.X, deltaFinalLerpPoint.Y);
			}

		}
		return true;
	}

	const bool USplineCompressor::CompressSpline
	(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape2Df& outVectorArray)const
	{
		if (spline.size() <= 0)
		{
			//Debug comment here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints <= 0)
		{
			//Debug comment here
			return false;
		}

		outVectorArray.clear();
		outVectorArray.reserve((numberOfSegmentsBetweenSplinePoints + 1) * spline.size());

		FPoint2f leftTangent{};
		FPoint2f rightTangent{};

		double currentDistanceDeltaBetweenPoints{};
		double currentAccumulatedDelta{ 0. };
		double percentage{};

		FPoint2f lerpPointA{};
		FPoint2f lerpPointB{};
		FPoint2f lerpPointC{};
		FPoint2f lerpPointD{};
		FPoint2f lerpPointE{};

		FPoint2f currentFinalLerpPoint{};
		FPoint2f pastFinalLerpPoint{};
		FPoint2f deltaFinalLerpPoint{};

		double currentDistanceBetweenOriginAndCurrentPoint{ 0. };

		//for each spline point
		for (size_t currentSplinePointIndex = 0; currentSplinePointIndex < spline.size() - 1; currentSplinePointIndex++)
		{

			const FSplinePoint& currentPoint = spline[currentSplinePointIndex];
			const FSplinePoint& nextPoint = spline[currentSplinePointIndex + 1];

			pastFinalLerpPoint = spline[currentSplinePointIndex].CurvePoint.Point;

			//Calculate the Tangent points
			leftTangent = SVectors::ObtainRelativeVectorComponent(
				currentPoint.CurvePoint.RightTangentPoint,
				currentPoint.CurvePoint.Point);

			rightTangent = SVectors::ObtainRelativeVectorComponent(
				nextPoint.CurvePoint.LeftTangentPoint,
				nextPoint.CurvePoint.Point);

			currentDistanceDeltaBetweenPoints = nextPoint.DistanceFromStart - currentPoint.DistanceFromStart;

			//Lerp while the accumulated delta is lower than the delta between current and next point
			while (currentAccumulatedDelta < currentDistanceDeltaBetweenPoints)
			{
				//calculate the lerp percentage

				outVectorArray.emplace_back(pastFinalLerpPoint);

				currentAccumulatedDelta += currentDistanceDeltaBetweenPoints / numberOfSegmentsBetweenSplinePoints;
				percentage = currentAccumulatedDelta / currentDistanceDeltaBetweenPoints;

				//lerp between all of the points
				lerpPointA =
					STools::CalculateLerpPoint
					(currentPoint.CurvePoint.Point,
						leftTangent,
						percentage);

				lerpPointB =
					STools::CalculateLerpPoint
					(rightTangent,
						nextPoint.CurvePoint.Point,
						percentage);

				lerpPointC =
					STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


				lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
				lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

				currentFinalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);

				deltaFinalLerpPoint = SVectors::Subtract(currentFinalLerpPoint, pastFinalLerpPoint);

				pastFinalLerpPoint = currentFinalLerpPoint;

				currentDistanceBetweenOriginAndCurrentPoint += STrigonometry::AbsoluteHypotenuse(deltaFinalLerpPoint.X, deltaFinalLerpPoint.Y);
			}

			currentAccumulatedDelta = 0;

		}
		return true;
	}

	const bool USplineCompressor::CompressSplineIntoShape(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape2Df& outVectorArray) const
	{
		if (spline.size() <= 0)
		{
			//Debug comment here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints == 0)
		{
			//Debug comment here
			return false;
		}

		outVectorArray.clear();
		outVectorArray.reserve((numberOfSegmentsBetweenSplinePoints + 1) * spline.size());

		FPoint2f leftTangent{};
		FPoint2f rightTangent{};

		double currentDistanceDeltaBetweenPoints{};
		double currentAccumulatedDelta{ 0. };
		double percentage{};

		FPoint2f lerpPointA{};
		FPoint2f lerpPointB{};
		FPoint2f lerpPointC{};
		FPoint2f lerpPointD{};
		FPoint2f lerpPointE{};

		FPoint2f currentFinalLerpPoint{};
		FPoint2f pastFinalLerpPoint{};
		FPoint2f deltaFinalLerpPoint{};

		double currentDistanceBetweenOriginAndCurrentPoint{ 0. };
		size_t nextPointIndex{ 0 };

		//for each spline point
		for (size_t currentSplinePointIndex = 0; currentSplinePointIndex < spline.size(); currentSplinePointIndex++)
		{
			nextPointIndex =
				currentSplinePointIndex < spline.size() - 1 ?
				currentSplinePointIndex + 1 :
				0;

			const FSplinePoint& currentPoint = spline[currentSplinePointIndex];
			const FSplinePoint& nextPoint = spline[nextPointIndex];

			pastFinalLerpPoint = spline[currentSplinePointIndex].CurvePoint.Point;

			//Calculate the Tangent points
			leftTangent = SVectors::ObtainRelativeVectorComponent(
				currentPoint.CurvePoint.RightTangentPoint,
				currentPoint.CurvePoint.Point);

			rightTangent = SVectors::ObtainRelativeVectorComponent(
				nextPoint.CurvePoint.LeftTangentPoint,
				nextPoint.CurvePoint.Point);

			currentDistanceDeltaBetweenPoints = SVectors::Distance(nextPoint.CurvePoint.Point, currentPoint.CurvePoint.Point);

			//Lerp while the accumulated delta is lower than the delta between current and next point
			while (currentAccumulatedDelta < currentDistanceDeltaBetweenPoints)
			{
				//calculate the lerp percentage

				outVectorArray.emplace_back(pastFinalLerpPoint);

				currentAccumulatedDelta += currentDistanceDeltaBetweenPoints / numberOfSegmentsBetweenSplinePoints;
				percentage = currentAccumulatedDelta / currentDistanceDeltaBetweenPoints;

				//lerp between all of the points
				lerpPointA =
					STools::CalculateLerpPoint
					(currentPoint.CurvePoint.Point,
						leftTangent,
						percentage);

				lerpPointB =
					STools::CalculateLerpPoint
					(rightTangent,
						nextPoint.CurvePoint.Point,
						percentage);

				lerpPointC =
					STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


				lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
				lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

				currentFinalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);

				deltaFinalLerpPoint = SVectors::Subtract(currentFinalLerpPoint, pastFinalLerpPoint);

				pastFinalLerpPoint = currentFinalLerpPoint;

				currentDistanceBetweenOriginAndCurrentPoint += STrigonometry::AbsoluteHypotenuse(deltaFinalLerpPoint.X, deltaFinalLerpPoint.Y);
			}

			currentAccumulatedDelta = 0;

		}
		return true;
	}

	const bool USplineCompressor::CompressSplineIntoShape(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape3Df& outShape) const
	{
		if (spline.size() <= 0)
		{
			//Debug comment here
			return false;
		}
		if (numberOfSegmentsBetweenSplinePoints == 0)
		{
			return false;
		}
		if (spline.size() == 1)
		{
			FPoint3f point{};
			point.X = spline[0].CurvePoint.Point.X;
			point.Y = spline[0].CurvePoint.Point.Y;
			outShape.emplace_back(point);
		}

		outShape.clear();
		outShape.reserve((numberOfSegmentsBetweenSplinePoints + 1) * spline.size());

		FPoint2f leftTangent{};
		FPoint2f rightTangent{};

		double currentDistanceDeltaBetweenPoints{};
		double currentAccumulatedDelta{ 0. };
		double percentage{};

		FPoint2f lerpPointA{};
		FPoint2f lerpPointB{};
		FPoint2f lerpPointC{};
		FPoint2f lerpPointD{};
		FPoint2f lerpPointE{};

		FPoint2f currentFinalLerpPoint{};
		FPoint2f pastFinalLerpPoint{};
		FPoint2f deltaFinalLerpPoint{};

		FPoint3f pastFinalLerpPoint3D{};

		double currentDistanceBetweenOriginAndCurrentPoint{ 0. };
		size_t nextPointIndex{ 0 };

		//for each spline point
		for (size_t currentSplinePointIndex = 0; currentSplinePointIndex < spline.size(); currentSplinePointIndex++)
		{
			nextPointIndex =
				currentSplinePointIndex < spline.size() - 1 ?
				currentSplinePointIndex + 1 :
				0;

			const FSplinePoint& currentPoint = spline[currentSplinePointIndex];
			const FSplinePoint& nextPoint = spline[nextPointIndex];

			pastFinalLerpPoint = spline[currentSplinePointIndex].CurvePoint.Point;

			//Calculate the Tangent points
			leftTangent = SVectors::ObtainRelativeVectorComponent(
				currentPoint.CurvePoint.RightTangentPoint,
				currentPoint.CurvePoint.Point);

			rightTangent = SVectors::ObtainRelativeVectorComponent(
				nextPoint.CurvePoint.LeftTangentPoint,
				nextPoint.CurvePoint.Point);

			currentDistanceDeltaBetweenPoints = SVectors::Distance(nextPoint.CurvePoint.Point, currentPoint.CurvePoint.Point);

			//Lerp while the accumulated delta is lower than the delta between current and next point
			while (currentAccumulatedDelta < currentDistanceDeltaBetweenPoints)
			{
				//calculate the lerp percentage

				pastFinalLerpPoint3D.X = pastFinalLerpPoint.X;
				pastFinalLerpPoint3D.Y = pastFinalLerpPoint.Y;
				outShape.emplace_back(pastFinalLerpPoint3D);

				currentAccumulatedDelta += currentDistanceDeltaBetweenPoints / numberOfSegmentsBetweenSplinePoints;
				percentage = currentAccumulatedDelta / currentDistanceDeltaBetweenPoints;

				//lerp between all of the points
				lerpPointA =
					STools::CalculateLerpPoint
					(currentPoint.CurvePoint.Point,
						leftTangent,
						percentage);

				lerpPointB =
					STools::CalculateLerpPoint
					(rightTangent,
						nextPoint.CurvePoint.Point,
						percentage);

				lerpPointC =
					STools::CalculateLerpPoint(leftTangent, rightTangent, percentage);


				lerpPointD = STools::CalculateLerpPoint(lerpPointA, lerpPointC, percentage);
				lerpPointE = STools::CalculateLerpPoint(lerpPointC, lerpPointB, percentage);

				currentFinalLerpPoint = STools::CalculateLerpPoint(lerpPointD, lerpPointE, percentage);

				deltaFinalLerpPoint = SVectors::Subtract(currentFinalLerpPoint, pastFinalLerpPoint);

				pastFinalLerpPoint = currentFinalLerpPoint;

				currentDistanceBetweenOriginAndCurrentPoint += STrigonometry::AbsoluteHypotenuse(deltaFinalLerpPoint.X, deltaFinalLerpPoint.Y);
			}

			currentAccumulatedDelta = 0;

		}
		return true;
	}

	const bool USplineCompressor::CompressSplineContainer(
		const D_Spline_Container& splineContainer, const unsigned int numberOfSegmentsBetweenSplinePoints, D_OrderedShape2D_Container& outCompressedSpline)const
	{

		unsigned int size = obtainSizeOfTotalSplineElements(splineContainer);

		if (size <= 0)
		{
			//debug comment here
			return false;
		}

		outCompressedSpline.clear();
		//CONTINUE FROM HERE
		outCompressedSpline.reserve(size);

		//for each spline
		for (size_t currentSplineIndex = 0; currentSplineIndex < splineContainer.size(); currentSplineIndex++)
		{
			outCompressedSpline.emplace_back();
			CompressSpline
			(splineContainer[currentSplineIndex],
				numberOfSegmentsBetweenSplinePoints,
				outCompressedSpline[currentSplineIndex]);
		}

		return true;
	}

	const size_t USplineCompressor::obtainSizeOfTotalSplineElements(const D_Spline_Container& splineContainer)const
	{
		if (splineContainer.size() <= 0)
		{
			//debug comment here
			return 0;
		}

		unsigned int size{};

		for (size_t i = 0; i < splineContainer.size(); i++)
		{
			size += splineContainer[i].size();
		}
		return size;
	}

	/*

		COMPRESSED SPLINE EVALUATOR

	*/

	UCompressedSplineEvaluator::UCompressedSplineEvaluator()
	{
	}


	const FPoint2f UCompressedSplineEvaluator::EvaluateCompressedSpline(const D_OrderedShape2D_Container& compressedSplineContainer, const size_t curveIndex, const double percentageFromOneToZero)
	{
		if (compressedSplineContainer.size() <= 0)
		{
			//debug comment here
			return { 0,0 };
		}
		if (curveIndex >= compressedSplineContainer.size())
		{
			//debug comment here
			return { 0,0 };
		}

		return EvaluateCompressedSpline(compressedSplineContainer[curveIndex], percentageFromOneToZero);
	}

	const FPoint2f UCompressedSplineEvaluator::EvaluateCompressedSpline(const DOrderedShape2D& compressedSpline, const double percentageFromOneToZero)
	{

		if (percentageFromOneToZero < 0 || percentageFromOneToZero > 1)
		{
			//put a debug comment here
			return { 0,0 };
		}
		if (compressedSpline.size() <= 0)
		{
			//put a debug comment here
			return { 0,0 };
		}

		double distanceOfSplineAtPercentage = percentageFromOneToZero * compressedSpline[compressedSpline.size() - 1].Length;

		//binary search
		size_t currentIndex{ compressedSpline.size() / 2 };
		size_t maxIndex{ compressedSpline.size() - 1 };
		size_t minIndex{ 0 };
		bool found{ false };

		while (!found)
		{
			currentIndex = ((maxIndex - minIndex) / 2) + minIndex;

			if (distanceOfSplineAtPercentage == compressedSpline[currentIndex].Length)
			{
				minIndex = currentIndex;
				maxIndex = currentIndex >= compressedSpline.size() - 1 ? currentIndex : currentIndex + 1;

				found = true;

				break;
			}
			else if (maxIndex - minIndex <= 1)
			{
				found = true;

				break;
			}


			if (distanceOfSplineAtPercentage < compressedSpline[currentIndex].Length)
			{
				maxIndex = currentIndex;

			}
			else {
				minIndex = currentIndex;
			}

		}

		//calculate the lerp percentage
		double percentage =
			(distanceOfSplineAtPercentage -
				compressedSpline[minIndex].Length) /
			static_cast<double>(
				compressedSpline[maxIndex].Length -
				compressedSpline[minIndex].Length);

		return STools::CalculateLerpPoint(compressedSpline[minIndex].Point, compressedSpline[minIndex].Point, percentage);
	}


}