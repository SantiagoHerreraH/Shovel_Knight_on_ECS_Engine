#pragma once

#include "../../../include/Structs/FVector.h"
#include "../../../include/Structs/FShape.h"
#include "../../../include/Structs/FMesh.h"
#include "../../../include/Structs/FCollision.h"

namespace Pillar {

	//Change all of the subtracts you did between yesterday and today

	class SCollision {

	public:
		static bool IsPointInCircle(const FPoint2f& point, const FCirclef& circle);
		static bool IsPointInCircle(const FPoint2i& point, const FCirclei& circle);
		static bool IsPointInCircle(const FPoint2& point, const FCircle& circle);

		static bool IsPointInRect(const FPoint2f& point, const FRectf& rect);
		static bool IsPointInRect(const FPoint2i& point, const FRecti& rect);
		static bool IsPointInRect(const FPoint2& point, const FRect& rect);

		static bool IsPointInLine(const FPoint2f& point, const FLine2f& line, float permittedDistanceFromLine);
		static bool IsPointInLine(const FPoint2i& point, const FLine2i& line, float permittedDistanceFromLine);
		static bool IsPointInLine(const FPoint2& point,  const FLine2& line, float permittedDistanceFromLine);

		static bool IsPointInTriangle(const FPoint2f& point, const FTriangle2f& triangle); 
		static bool IsPointInMesh(const FPoint2f& point, const DMesh2Df& mesh2d);

		static bool IsOverlapping(const FTriangle2f& triangle1, const FTriangle2f& triangle2, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape1, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape2, bool clearPreviousCollisionInfo); 
		static bool IsOverlapping(const FMesh2Df& mesh1, const FMesh2Df& mesh2, FMeshEdgesAndVerticesOverlapInfo& outOverlapInfoMesh1, FMeshEdgesAndVerticesOverlapInfo& outOverlapInfoMesh2, bool clearPreviousCollisionInfo); 
		
		static bool IsOverlapping(const DShape2Df& shape1, const DShape2Df& shape2); // Uses Separation Axes Theorem (compare shadows) -> //modify this
		static bool IsOverlapping(const FShape2Df& shape1, const FShape2Df& shape2, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape1, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape2, bool clearPreviousCollisionInfo); //Uses line intersection 
		static bool IsOverlapping(const FShape2Df& shape1, const FShape2Df& shape2, FResolveOverlapInfo& outResolveOverlapInfoShape1, FResolveOverlapInfo& outResolveOverlapInfoShape2);
		
		static bool IsOverlapping(const FLine2f& line1, const FLine2f& line2, FLineOverlapInfo& outOverlapInfo);
		
		static bool IsOverlapping(const FCirclef& circle1, const FCirclef& circle2);
		static bool IsOverlapping(const FCirclei& circle1, const FCirclei& circle2);
		static bool IsOverlapping(const FCircle& circle1, const FCircle& circle2);


		static bool IsOverlapping(const FRecti& rect1, const FRecti& rect2);
		static bool IsOverlapping(const FRect& rect1, const FRect& rect2);

		static bool IsOverlapping(const FRectf& rect1, const FRectf& rect2, FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2);
		static bool IsOverlapping(const FRectf& rect1, const FRectf& rect2, FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2, const FPoint2f& velocity1, const FPoint2f& velocity2);

	private:
		static void ChooseX(
			const FRectf& rectWithVelocityToDecide, const FRectf& otherRect,
			FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2,
			const FPoint2f& velocity1);

		static void ChooseY(
			const FRectf& rectWithVelocityToDecide, const FRectf& otherRect,
			FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2,
			const FPoint2f& velocity1);

	};
}

