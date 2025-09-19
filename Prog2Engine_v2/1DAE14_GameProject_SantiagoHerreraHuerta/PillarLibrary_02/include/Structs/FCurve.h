#pragma once

#include "FVector.h"

namespace Pillar {

	/*

		CURVE

	*/

	enum class EValueBehavior {
		Loop,
		Rebound,
		Clamp
	};

	struct FCurvePoint {

		FPoint2f Point{};

		FVector LeftTangentPoint{};

		FVector RightTangentPoint{};

	};

	using DCurve = std::vector<FCurvePoint>;

	using D_Curve_Container = std::vector<DCurve>;

	struct FControlledCurve {

		DCurve Curve;
		EValueBehavior CurveBehavior;
	};

	/*

		SPLINE

	*/

	struct FSplinePoint {

		FCurvePoint CurvePoint{};

		double DistanceFromStart{};
	};

	using DSpline = std::vector<FSplinePoint>;

	using D_Spline_Container = std::vector<DSpline>;


	using DOrderedShape2D = std::vector<FPoint2AndLength>;

	using D_OrderedShape2D_Container = std::vector<DOrderedShape2D>;

}
