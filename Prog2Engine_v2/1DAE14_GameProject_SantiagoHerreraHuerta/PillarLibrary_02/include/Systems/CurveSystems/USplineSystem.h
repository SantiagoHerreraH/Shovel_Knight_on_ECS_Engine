#pragma once

#include "../../Structs/FCurve.h"
#include "../../Structs/FShape.h"

namespace Pillar {

	class USplineCreator {

	public:
		USplineCreator();

		const bool AddSpline(DSpline& spline, D_Spline_Container& splineContainer);
		const bool DeleteSpline(const int splineIndex, D_Spline_Container& splineContainer);
		const bool AddSplinePoint(FSplinePoint& splinePoint, DSpline& splineRef);
		const bool DeleteSplinePoint(const int splinePointIndex, DSpline& splineRef);

	private:
		//should be able to calcuate spline length with circumference math
		const bool calculateSplinePointsDistanceFromOriginSplinePoint(D_Spline_Container& splineContainer, const unsigned int numberOfSegmentsBetweenSplinePoints = 50);
		const bool calculateSplinePointsDistanceFromOriginSplinePoint(DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints = 50);

	};

	class USplineEvaluator {

	public:
		USplineEvaluator();

		const FPoint2f EvaluateSpline(const D_Spline_Container& splineContainer, const size_t splineIndex, const double percentageFromOneToZero) const;
		const FPoint2f EvaluateSpline(const DSpline& splineToEvaluate, const double percentageFromOneToZero) const;

		const double GetSplineTotalLength(const D_Spline_Container& splineContainer, const size_t splineIndex) const;
		const double GetSplineTotalLength(const DSpline& spline) const;
		const double GetLengthOfSplinePercentage(const D_Spline_Container& splineContainer, const size_t splineIndex, const double percentageFromOneToZero) const;
		const double GetLengthOfSplinePercentage(const DSpline& spline, const double percentageFromOneToZero) const;

	};

	class USplineCompressor {

	public:
		USplineCompressor();

		const bool CompressSpline(const D_Spline_Container& splineContainer, const size_t splineIndex, const unsigned int numberOfSegmentsBetweenSplinePoints, DOrderedShape2D& outVectorArray)const;
		const bool CompressSpline(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DOrderedShape2D& outVectorArray)const;
		const bool CompressSpline(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape2Df& outVectorArray)const;
		const bool CompressSplineIntoShape(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape2Df& outVectorArray)const;
		const bool CompressSplineIntoShape(const DSpline& spline, const unsigned int numberOfSegmentsBetweenSplinePoints, DShape3Df& outShape)const;


		const bool CompressSplineContainer(const D_Spline_Container& splineContainer, const unsigned int numberOfSegmentsBetweenSplinePoints, D_OrderedShape2D_Container& outCompressedSpline)const;

	private:

		const size_t obtainSizeOfTotalSplineElements(const D_Spline_Container& splineContainer)const;
	};

	class UCompressedSplineEvaluator {

	public:
		UCompressedSplineEvaluator();

		const FPoint2f EvaluateCompressedSpline(const D_OrderedShape2D_Container& compressedSplineContainer, const size_t curveIndex, const double percentageFromOneToZero);
		const FPoint2f EvaluateCompressedSpline(const DOrderedShape2D& compressedSpline, const double percentageFromOneToZero);

	};


}