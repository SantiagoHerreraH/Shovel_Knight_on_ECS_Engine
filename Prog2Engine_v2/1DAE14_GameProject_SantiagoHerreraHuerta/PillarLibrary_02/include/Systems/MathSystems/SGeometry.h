#pragma once


#include "../../../include/Structs/FShape.h"
#include "../../../include/Structs/FMesh.h"
#include "../../../../Engine/structs.h"

namespace Pillar {

	enum class ERectSimplificationMode {
		PrioritizeVerticalRect,
		PrioritizeHorizontalRects
	};

	class SGeometry {

	public:
		static const FPoint2f Center(FMesh2Df& mesh);
		static const FPoint2f Center(FShape2Df& shape);

		static void MakeRect(float width, float height, FPoint2f center, FShape2Df& shape);

		static std::vector<Rectf> Simplify(const std::vector<Rectf>& rects, ERectSimplificationMode rectSimplificationMode = ERectSimplificationMode::PrioritizeVerticalRect);
		static bool CanBeOneHorizontal(const Rectf& one, const Rectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableHeightDelta);
		static bool CanBeOneVertical(const Rectf& one, const Rectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableWidthDelta);
		static Rectf Fuse(const Rectf& one, const Rectf& two);

		static FPoint2f EvaluateLine(const FLine2f& line, float percentage);
	};
}