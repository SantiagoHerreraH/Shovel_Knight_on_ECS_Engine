#pragma once

#include "../../Structs/FCurve.h"
#include "../../Structs/FShape.h"

namespace Pillar {

	class UCurveEvaluator {

	public:
		UCurveEvaluator();

		const double EvaluateCurve(const D_Curve_Container& curveContainer, const size_t curveIndex, const double value) const;
		const double EvaluateCurve(const DCurve& curveToEvaluate, const double value)const;
		const double EvaluateCurve(const FControlledCurve& controlledCurve, const double value)const;

		const double GetMaxCurveValue(const D_Curve_Container& curveContainer, const size_t curveIndex)const;
		const double GetMinCurveValue(const D_Curve_Container& curveContainer, const size_t curveIndex)const;

		const double GetMaxCurveValue(const DCurve& curve)const;
		const double GetMinCurveValue(const DCurve& curve)const;

	};

	class UCurveCompressor {

	public:
		UCurveCompressor();

		//number of segments is not the same as number of points
		const bool CompressCurve(const DCurve& curve, const unsigned int numberOfTotalSegments, DShape2Df& outVectorArray);
		const bool CompressCurve(const D_Curve_Container& curveContainer, const size_t curveIndex, const unsigned int numberOfTotalSegments, DShape2Df& outVectorArray);

		const bool CompressCurveContainer(const D_Curve_Container& curveContainer, const unsigned int numberOfTotalSegments, D_Shape2D_Container& outCompressedCurve);

	private:

		UCurveEvaluator m_CurveEvaluator;

	};

	class UCompressedCurveEvaluator {

	public:
		UCompressedCurveEvaluator();

		const double EvaluateCompressedCurve(const D_Shape2D_Container& compressedCurveContainer, const size_t curveIndex, const double value);
		const double EvaluateCompressedCurve(const DShape2Df& arrayOfPoints, const double value);

	};

	class SCurves {

	public:
		const static UCurveEvaluator CurveEvaluator;
		const static UCurveCompressor CurveCompressor;
		const static UCompressedCurveEvaluator CompressedCurveEvaluator;
	};

}