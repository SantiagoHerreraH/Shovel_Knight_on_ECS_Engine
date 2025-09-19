
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/MathSystems/SMatrix.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"

namespace Pillar {

#pragma region IsPointInCircle

	bool SCollision::IsPointInCircle(const FPoint2f& point, const FCirclef& circle)
	{
		
		return 
			IsPointInCircle(
				SVectors::CastToDoublePoint(point),
			 {  SVectors::CastToDoublePoint(circle.Center), (double)circle.Radius });
	}

	bool SCollision::IsPointInCircle(const FPoint2i& point, const FCirclei& circle)
	{
		return
			IsPointInCircle(
				SVectors::CastToDoublePoint(point),
				{ SVectors::CastToDoublePoint(circle.Center), (double)circle.Radius });
	}

	bool SCollision::IsPointInCircle(const FPoint2& point, const FCircle& circle)
	{
		const double distance{ SVectors::Distance(point, circle.Center) };
		bool result{ distance <= circle.Radius };

		return result;
	}

#pragma endregion

#pragma region IsPointInRect

	bool SCollision::IsPointInRect(const FPoint2i& point, const FRecti& rect)
	{
		const bool horizontalCheck{ (point.X >= rect.Left) && (point.X <= rect.Left + rect.Width) };
		const bool verticalCheck{ (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height)) };

		return(horizontalCheck && verticalCheck);
	}

	bool SCollision::IsPointInRect(const FPoint2f& point, const FRectf& rect)
	{
		const bool horizontalCheck{ (point.X >= rect.Left) && (point.X <= rect.Left + rect.Width) };
		const bool verticalCheck{ (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height)) };

		return(horizontalCheck && verticalCheck);
	}

	bool SCollision::IsPointInRect(const FPoint2& point, const FRect& rect)
	{
		const bool horizontalCheck{ (point.X >= rect.Left) && (point.X <= rect.Left + rect.Width) };
		const bool verticalCheck{ (point.Y <= rect.Top) && (point.Y >= (rect.Top - rect.Height)) };

		return(horizontalCheck && verticalCheck);
	}

#pragma endregion

#pragma region IsPointInLine

	bool SCollision::IsPointInLine(const FPoint2f& point, const FLine2f& line, float permittedDistanceFromLine)
	{
		FPoint2f localProjectedVector = 
			SVectors::ProjectedVector(
				SVectors::Subtract(line.End, line.Start),
				SVectors::Subtract(point, line.Start));

		FPoint2f actualProjectedVector
			= SVectors::Add(localProjectedVector, line.Start);

		bool isAtAPassablePerpendicularDistance = SVectors::Distance(actualProjectedVector, point) <= permittedDistanceFromLine;
		bool isAtAPassableParallelDistance = SVectors::Distance(line.Start, line.End) >= SVectors::Distance(line.Start, point);

		return isAtAPassableParallelDistance && isAtAPassablePerpendicularDistance;
	}

	bool SCollision::IsPointInLine(const FPoint2i& point, const FLine2i& line, float permittedDistanceFromLine)
	{
		FPoint2i localProjectedVector =
			SVectors::ProjectedVector(
				SVectors::Subtract(line.End, line.Start),
				SVectors::Subtract(point, line.Start));

		FPoint2i actualProjectedVector
			= SVectors::Add(localProjectedVector, line.Start);

		bool isAtAPassablePerpendicularDistance = SVectors::Distance(actualProjectedVector, point) <= permittedDistanceFromLine;
		bool isAtAPassableParallelDistance = SVectors::Distance(line.Start, line.End) >= SVectors::Distance(line.Start, point);

		return isAtAPassableParallelDistance && isAtAPassablePerpendicularDistance;
	}

	bool SCollision::IsPointInLine(const FPoint2& point, const FLine2& line, float permittedDistanceFromLine)
	{
		FPoint2 localProjectedVector =
			SVectors::ProjectedVector(
				SVectors::Subtract(line.End, line.Start),
				SVectors::Subtract(point, line.Start));

		FPoint2 actualProjectedVector
			= SVectors::Add(localProjectedVector, line.Start);

		bool isAtAPassablePerpendicularDistance = SVectors::Distance(actualProjectedVector, point) <= permittedDistanceFromLine;
		bool isAtAPassableParallelDistance = SVectors::Distance(line.Start, line.End) >= SVectors::Distance(line.Start, point);

		return isAtAPassableParallelDistance && isAtAPassablePerpendicularDistance;
	}

#pragma endregion

#pragma region IsPointInTriangle

	bool SCollision::IsPointInTriangle(const FPoint2f& point, const FTriangle2f& triangle)
	{
		float triangleArea{ std::abs(STrigonometry::Area(triangle))};
		float composedTriangleArea{};

		FTriangle2f currentTriangle{};

		for (size_t i = 0; i < 3; i++)
		{
			currentTriangle.Vertices[0] = point;
			currentTriangle.Vertices[1] = triangle.Vertices[i];
			currentTriangle.Vertices[2] = triangle.Vertices[(i + 1) % 3];

			composedTriangleArea += std::abs(STrigonometry::Area(currentTriangle));

		}
	
		return triangleArea == composedTriangleArea;
	}

#pragma endregion

#pragma region IsPointInMesh

	bool SCollision::IsPointInMesh(const FPoint2f& point, const DMesh2Df& mesh2d)
	{
		for (size_t i = 0; i < mesh2d.size(); i++)
		{
			if (IsPointInTriangle(point, mesh2d[i]))
			{
				return true;
			}
		}
		return false;
	}

	bool SCollision::IsOverlapping(const FTriangle2f& triangle1, const FTriangle2f& triangle2, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape1, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape2, bool clearPreviousCollisionInfo)
	{
		FShape2Df shape1{};
		FShape2Df shape2{};

		STrigonometry::Shape(triangle1, shape1);
		STrigonometry::Shape(triangle2, shape2);

		return IsOverlapping(shape1, shape2, outOverlapInfoShape1, outOverlapInfoShape2, clearPreviousCollisionInfo);
	}

	bool SCollision::IsOverlapping(const FMesh2Df& mesh1, const FMesh2Df& mesh2, FMeshEdgesAndVerticesOverlapInfo& outOverlapInfoMesh1, FMeshEdgesAndVerticesOverlapInfo& outOverlapInfoMesh2, bool clearPreviousCollisionInfo)
	{
		if (clearPreviousCollisionInfo)
		{
			outOverlapInfoMesh1.TrianglesWithEdgesThatOverlap.clear();
			outOverlapInfoMesh1.TrianglesWithVerticesThatOverlap.clear();
			outOverlapInfoMesh2.TrianglesWithEdgesThatOverlap.clear();
			outOverlapInfoMesh2.TrianglesWithVerticesThatOverlap.clear();
		}
	
		FEdgesAndVerticesOverlapInfo shapeOverlapInfo1{};
		FEdgesAndVerticesOverlapInfo shapeOverlapInfo2{};

		FEdgesAndVerticesOverlapInfo* currentShapeOverlapInfo{ &shapeOverlapInfo1 };
		FMeshEdgesAndVerticesOverlapInfo* currentMeshOverlapInfo{ &outOverlapInfoMesh1 };
		size_t currentTriangleIndex{};

		FMeshTriangle_OverlappingEdge_Info meshTriangle_OverlappingEdge;
		FMeshTriangle_OverlappingVertice_Info meshTriangle_OverlappingVertice;

		bool isOverlapping{false};

		for (size_t mesh1_Idx = 0; mesh1_Idx < mesh1.Triangles.size(); mesh1_Idx++)
		{
			for (size_t mesh2_Idx = 0; mesh2_Idx < mesh2.Triangles.size(); mesh2_Idx++)
			{
				if (IsOverlapping(mesh1.Triangles[mesh1_Idx], mesh2.Triangles[mesh2_Idx], shapeOverlapInfo1, shapeOverlapInfo2, true))
				{
					isOverlapping = true;

					currentShapeOverlapInfo = &shapeOverlapInfo1;
					currentMeshOverlapInfo = &outOverlapInfoMesh1;
					currentTriangleIndex = mesh1_Idx;

					for (size_t i = 0; i < 2; i++)
					{
						if (i == 1)
						{
							currentShapeOverlapInfo = &shapeOverlapInfo2;
							currentMeshOverlapInfo = &outOverlapInfoMesh2;
							currentTriangleIndex = mesh2_Idx;
						}

						for (size_t edgeInfo_Idx = 0; edgeInfo_Idx < currentShapeOverlapInfo->OverlappingEdges.size(); edgeInfo_Idx++)
						{

							meshTriangle_OverlappingEdge.OverlappedEdge = currentShapeOverlapInfo->OverlappingEdges[edgeInfo_Idx];
							meshTriangle_OverlappingEdge.TriangleIndex = currentTriangleIndex;

							currentMeshOverlapInfo->TrianglesWithEdgesThatOverlap.push_back(meshTriangle_OverlappingEdge);
						}
						for (size_t verticeInfo_Idx = 0; verticeInfo_Idx < currentShapeOverlapInfo->OverlappingVectorsFromCenterToVertice.size(); verticeInfo_Idx++)
						{
							meshTriangle_OverlappingVertice.OverlappingVectorFromCenterToVertice = currentShapeOverlapInfo->OverlappingVectorsFromCenterToVertice[verticeInfo_Idx];
							meshTriangle_OverlappingVertice.TriangleIndex = currentTriangleIndex;

							currentMeshOverlapInfo->TrianglesWithVerticesThatOverlap.push_back(meshTriangle_OverlappingVertice);
						}
					}
					
				}
			}
		}

		return isOverlapping;
	}


#pragma endregion

#pragma region IsOverlappingShape

	bool SCollision::IsOverlapping(const DShape2Df& shape1, const DShape2Df& shape2)
	{
		 const DShape2Df* ref1 = &shape1;
		 const DShape2Df* ref2 = &shape2;
		
		 FPoint2f currentPoint{};
		 FPoint2f nextPoint{};
		 FPoint2f currentEdgeVector{};
		 FPoint2f currentProjectionVector{};

		 float minRef1{ INFINITY };
		 float maxRef1{ -INFINITY };

		 float minRef2{ INFINITY };
		 float maxRef2{ -INFINITY };

		 float totalMax{};
		 float totalMin{};

		 float dotProduct{};

		for (size_t i = 0; i < 2; i++)
		{
			if (i == 1)
			{
				ref1 = &shape2;
				ref2 = &shape1;
			}

			for (size_t currentPointIndex = 0; currentPointIndex < ref1->size(); currentPointIndex++)
			{
				currentPoint = ref1->at(currentPointIndex);
				nextPoint = ref1->at((currentPointIndex + 1) % ref1->size());

				currentEdgeVector = SVectors::Subtract(nextPoint, currentPoint);
				currentProjectionVector = SVectors::NormalizeVector(SVectors::OrthogonalVector(currentEdgeVector));

				minRef1 = INFINITY;
				maxRef1 = -INFINITY;

				for (size_t i = 0; i < ref1->size(); i++)
				{
					dotProduct = SVectors::DotProduct(ref1->at(i), currentProjectionVector);

					minRef1 = dotProduct < minRef1 ? dotProduct : minRef1;
					maxRef1 = dotProduct > maxRef1 ? dotProduct : maxRef1;
				}

				minRef2 = INFINITY ;
				maxRef2 = -INFINITY;

				for (size_t i = 0; i < ref2->size(); i++)
				{
					dotProduct = SVectors::DotProduct(ref2->at(i), currentProjectionVector);

					minRef2 = dotProduct < minRef2 ? dotProduct : minRef2;
					maxRef2 = dotProduct > maxRef2 ? dotProduct : maxRef2;
				}

				totalMax  = maxRef1 > maxRef2 ? maxRef1 : maxRef2;
				totalMin  = minRef1 < minRef2 ? minRef1 : minRef2;

				//If shadows aren't overlapping
				if ((totalMax - totalMin) > ((maxRef1 - minRef1) + (maxRef2 - minRef2)))
				{
					return false;
				}
				
			}
		}

		return true;
	}

	//Fix the subtract after this
	//change the FShapeOverlapInfo out
	bool SCollision::IsOverlapping(const FShape2Df& shape1, const FShape2Df& shape2, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape1, FEdgesAndVerticesOverlapInfo& outOverlapInfoShape2, bool clearPreviousCollisionInfo)
	{

		if (clearPreviousCollisionInfo)
		{
			outOverlapInfoShape1.OverlappingEdges.clear();
			outOverlapInfoShape1.OverlappingVectorsFromCenterToVertice.clear();

			outOverlapInfoShape2.OverlappingEdges.clear();
			outOverlapInfoShape2.OverlappingVectorsFromCenterToVertice.clear();
		}

		const FShape2Df* shapeRef_Vertices{&shape1};
		const FShape2Df* shapeRef_Edges{&shape2};

		FEdgesAndVerticesOverlapInfo* overlapInfoShapeVertices{ &outOverlapInfoShape1 };
		FEdgesAndVerticesOverlapInfo* overlapInfoShapeEdges{ &outOverlapInfoShape2 };

		FLineOverlapInfo lineOverlapInfo{};

		FLine2f centerToVerticeLine{};
		FLine2f edge{};

		bool isOverlapping{ false };
		FOverlappingEdgeOfShape overlappingEdge;
		FOverlappingLineOfShape overlappingVertice;

		for (size_t currentShape = 0; currentShape < 2; currentShape++)
		{
			if (currentShape == 1)
			{
				shapeRef_Vertices = &shape2;
				shapeRef_Edges = &shape1;

				overlapInfoShapeVertices = &outOverlapInfoShape2;
				overlapInfoShapeEdges = &outOverlapInfoShape1;
			}

			for (size_t i = 0; i < shapeRef_Vertices->Shape.size(); i++)
			{
				centerToVerticeLine.Start = shapeRef_Vertices->Center;
				centerToVerticeLine.End = shapeRef_Vertices->Shape.at(i);

				for (size_t j = 0; j < shapeRef_Edges->Shape.size(); j++)
				{
					edge.Start = shapeRef_Edges->Shape.at(j);
					edge.End = shapeRef_Edges->Shape.at((j + 1) % shapeRef_Edges->Shape.size());

					if (IsOverlapping(centerToVerticeLine,edge, lineOverlapInfo))
					{
						overlappingEdge.StartPointIndex = j;
						overlappingEdge.EndPointIndex = (j + 1) % shapeRef_Edges->Shape.size();
						overlappingEdge.StartPoint = edge.Start;
						overlappingEdge.EndPoint = edge.End;
						overlappingEdge.IntersectionPoint = lineOverlapInfo.IntersectionPoint;
						overlappingEdge.Scalar = lineOverlapInfo.Line2Scalar;
						overlappingEdge.LineThatOverlapped = centerToVerticeLine;
						overlappingEdge.LineScalar = lineOverlapInfo.Line1Scalar;

						overlappingVertice.StartPoint = centerToVerticeLine.Start;
						overlappingVertice.EndPoint = centerToVerticeLine.End;
						overlappingVertice.EndPointIndex = i;
						overlappingVertice.IntersectionPoint = lineOverlapInfo.IntersectionPoint;
						overlappingVertice.Scalar = lineOverlapInfo.Line1Scalar;
						overlappingVertice.EdgeThatOverlapped = edge;
						overlappingVertice.EdgeScalar = lineOverlapInfo.Line2Scalar;

						overlapInfoShapeEdges->OverlappingEdges.push_back(overlappingEdge);
						overlapInfoShapeVertices->OverlappingVectorsFromCenterToVertice.push_back(overlappingVertice);

						isOverlapping = true;
						//return true;
					}
				}
			}

		}
		return isOverlapping;
	}

	bool SCollision::IsOverlapping(const FShape2Df& shape1, const FShape2Df& shape2, FResolveOverlapInfo& outResolveOverlapInfoShape1, FResolveOverlapInfo& outResolveOverlapInfoShape2)
	{
		FEdgesAndVerticesOverlapInfo shapeInfoOne{};
		FEdgesAndVerticesOverlapInfo shapeInfoTwo{};
		FEdgesAndVerticesOverlapInfo* chosenInfo{nullptr};
		bool isOverlapping{ IsOverlapping(shape1, shape2, shapeInfoOne, shapeInfoTwo, true)};
		
		size_t chosenIndex{};
		float smallestScalar{INFINITY};
		bool chosenVectorIsFromShapeOne{ true };

		for (size_t i = 0; i < shapeInfoOne.OverlappingVectorsFromCenterToVertice.size(); i++)
		{
			if (shapeInfoOne.OverlappingVectorsFromCenterToVertice.at(i).Scalar < smallestScalar)
			{
				smallestScalar = shapeInfoOne.OverlappingVectorsFromCenterToVertice.at(i).Scalar;
				chosenIndex = i;
			}
		}

		for (size_t i = 0; i < shapeInfoTwo.OverlappingVectorsFromCenterToVertice.size(); i++)
		{
			if (shapeInfoTwo.OverlappingVectorsFromCenterToVertice.at(i).Scalar < smallestScalar)
			{
				smallestScalar = shapeInfoTwo.OverlappingVectorsFromCenterToVertice.at(i).Scalar;
				chosenIndex = i;
				chosenVectorIsFromShapeOne = false;
			}
		}

		if (shapeInfoOne.OverlappingVectorsFromCenterToVertice.empty() && 
			shapeInfoTwo.OverlappingVectorsFromCenterToVertice.empty())
		{
			outResolveOverlapInfoShape1.isOverlapping = false;
			outResolveOverlapInfoShape2.isOverlapping = false;
			return false;
		}

		double scalarToApply{};

		if (chosenVectorIsFromShapeOne && !shapeInfoOne.OverlappingVectorsFromCenterToVertice.empty())
		{
			chosenInfo = &shapeInfoOne;
			scalarToApply = 1.0 - shapeInfoOne.OverlappingVectorsFromCenterToVertice.at(chosenIndex).Scalar;
			
		}
		else if (!chosenVectorIsFromShapeOne && !shapeInfoTwo.OverlappingVectorsFromCenterToVertice.empty())
		{
			chosenInfo = &shapeInfoTwo;
			scalarToApply = 1.0 - shapeInfoTwo.OverlappingVectorsFromCenterToVertice.at(chosenIndex).Scalar;
		}

		if (scalarToApply == 0)
		{
			outResolveOverlapInfoShape1.isOverlapping = false;
			outResolveOverlapInfoShape2.isOverlapping = false;
			return false;
		}

		FPoint2f verticeVector = SVectors::Subtract(
			chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].EndPoint,
			chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].StartPoint);

		FPoint2f edgeVector = SVectors::Subtract(
			chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].EdgeThatOverlapped.End,
			chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].EdgeThatOverlapped.Start);

		FPoint2f currentTranslationAmount = SVectors::Scale(
			verticeVector,
			scalarToApply);

		FPoint2f projectedVector = SVectors::ProjectedVector(edgeVector, currentTranslationAmount);

		currentTranslationAmount.X -= projectedVector.X;
		currentTranslationAmount.Y -= projectedVector.Y;

		outResolveOverlapInfoShape1.TranslationVector = currentTranslationAmount;
		outResolveOverlapInfoShape2.TranslationVector = currentTranslationAmount;

		if (chosenVectorIsFromShapeOne)
		{
			outResolveOverlapInfoShape1.TranslationVector.X *= -1;
			outResolveOverlapInfoShape1.TranslationVector.Y *= -1;
		}
		else
		{
			outResolveOverlapInfoShape2.TranslationVector.X *= -1;
			outResolveOverlapInfoShape2.TranslationVector.Y *= -1;
		}

		outResolveOverlapInfoShape1.IntersectionPoint = chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].IntersectionPoint;
		outResolveOverlapInfoShape2.IntersectionPoint = chosenInfo->OverlappingVectorsFromCenterToVertice[chosenIndex].IntersectionPoint;

		outResolveOverlapInfoShape1.isOverlapping  = isOverlapping;
		outResolveOverlapInfoShape2.isOverlapping  = isOverlapping;

		return isOverlapping;
	}

#pragma endregion

#pragma region IsOverlappingLine

	// reference video (https://www.youtube.com/watch?v=5FkOO1Wwb8w)

	bool SCollision::IsOverlapping(const FLine2f& line1, const FLine2f& line2, FLineOverlapInfo& outOverlapInfo)
	{
		FPoint2f scalarVector{};

		//AC = CD * t1 + AB * t2
		FPoint2f AC{ SVectors::Subtract( line2.Start, line1.Start) };
		FPoint2f AB{ SVectors::Subtract( line1.End  , line1.Start) };
		FPoint2f CD{ SVectors::Subtract( line2.End  , line2.Start) };

		DMatrix2x2f tempMatrix{};
		tempMatrix[0][0] = AB.X;
		tempMatrix[0][1] = CD.X;
		tempMatrix[1][0] = AB.Y;
		tempMatrix[1][1] = CD.Y;

		SMatrix::Inverse(tempMatrix);

		SMatrix::Multiply(tempMatrix, AC, scalarVector);
		
		
		outOverlapInfo.Line1Scalar = scalarVector.X;
		outOverlapInfo.Line2Scalar = -scalarVector.Y;
		outOverlapInfo.IntersectionPoint = SVectors::Add(line1.Start, SVectors::Scale(AB, outOverlapInfo.Line1Scalar));

		if (outOverlapInfo.Line1Scalar < 0 || outOverlapInfo.Line2Scalar < 0 ||
			outOverlapInfo.Line1Scalar > 1 || outOverlapInfo.Line2Scalar > 1)
		{
			outOverlapInfo.isOverlapping = false;
			return false;
		}

		outOverlapInfo.isOverlapping = true;
		return outOverlapInfo.isOverlapping;
	}

#pragma endregion

#pragma region IsOverlappingCircle
	bool SCollision::IsOverlapping(const FCirclef& circle1, const FCirclef& circle2)
	{
		const float distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= circle1.Radius + circle2.Radius);
	}

	bool SCollision::IsOverlapping(const FCirclei& circle1, const FCirclei& circle2)
	{
		const float distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= circle1.Radius + circle2.Radius);
	}

	bool SCollision::IsOverlapping(const FCircle& circle1, const FCircle& circle2)
	{
		const double distance{ SVectors::Distance(circle1.Center, circle2.Center) };

		return (distance <= circle1.Radius + circle2.Radius);
	}

#pragma endregion

#pragma region IsOverlappingRect

	

	bool SCollision::IsOverlapping(
		const FRectf& rect1,
		const FRectf& rect2,
		FResolveOverlapInfo& outOverlapInfo1,
		FResolveOverlapInfo& outOverlapInfo2)
	{
		outOverlapInfo1.isOverlapping = false;
		outOverlapInfo1.IntersectionPoint = FPoint2f{ 0,0 };
		outOverlapInfo1.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfo1.DirectionOfCollided = FPoint2i{ 0,0 };

		outOverlapInfo2.isOverlapping = false;
		outOverlapInfo2.IntersectionPoint = FPoint2f{ 0,0 };
		outOverlapInfo2.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfo2.DirectionOfCollided = FPoint2i{ 0,0 };

		if (rect2.Left > (rect1.Left + rect1.Width) ||
			rect1.Left > (rect2.Left + rect2.Width))
		{
			return false;
		}

		if (rect2.Top < (rect1.Top - rect1.Height) ||
			rect1.Top < (rect2.Top - rect2.Height))
		{
			return false;
		}

		float bottom1{ rect1.Top - rect1.Height };
		float bottom2{ rect2.Top - rect2.Height };

		float right1{ rect1.Left + rect1.Width };
		float right2{ rect2.Left + rect2.Width };

		float deltaX_One{ abs(rect1.Left - right2) };
		float deltaX_Two{ abs(rect2.Left - right1) };

		float deltaY_One{ abs(rect1.Top - bottom2) };
		float deltaY_Two{ abs(rect2.Top - bottom1) };

		FPoint2f absoluteDelta{};
		absoluteDelta.X = deltaX_One < deltaX_Two ? deltaX_One : deltaX_Two;
		absoluteDelta.Y = deltaY_One < deltaY_Two ? deltaY_One : deltaY_Two;
		

		if (absoluteDelta.X < absoluteDelta.Y)
		{
			if (rect1.Left < rect2.Left)
			{
				outOverlapInfo1.TranslationVector.X = -absoluteDelta.X;
				outOverlapInfo2.TranslationVector.X = absoluteDelta.X;

				outOverlapInfo1.DirectionOfCollided = FPoint2i{ 1,0 };
				outOverlapInfo2.DirectionOfCollided = FPoint2i{ -1,0 };
			}
			else
			{
				outOverlapInfo1.TranslationVector.X = absoluteDelta.X;
				outOverlapInfo2.TranslationVector.X = -absoluteDelta.X;

				outOverlapInfo1.DirectionOfCollided = FPoint2i{ -1,0 };
				outOverlapInfo2.DirectionOfCollided = FPoint2i{ 1,0 };
			}
		}
		else
		{
			if (rect1.Top < rect2.Top)
			{
				outOverlapInfo1.TranslationVector.Y = -absoluteDelta.Y;
				outOverlapInfo2.TranslationVector.Y = absoluteDelta.Y;

				outOverlapInfo1.DirectionOfCollided = FPoint2i{ 0, 1 };
				outOverlapInfo2.DirectionOfCollided = FPoint2i{ 0,-1 };
			}
			else
			{
				outOverlapInfo1.TranslationVector.Y = absoluteDelta.Y;
				outOverlapInfo2.TranslationVector.Y = -absoluteDelta.Y;

				outOverlapInfo1.DirectionOfCollided = FPoint2i{ 0,-1 };
				outOverlapInfo2.DirectionOfCollided = FPoint2i{ 0,1 };
			}
		}

		float rightMostLeft{ rect1.Left < rect2.Left ? rect2.Left : rect1.Left };
		float leftMostRight{ right1 < right2 ? right1 : right2 };
		float lowestTop{ rect1.Top < rect2.Top ? rect1.Top : rect2.Top };

		float xIntersection{ (rightMostLeft + leftMostRight) / 2.f };

		outOverlapInfo1.isOverlapping = true;
		outOverlapInfo1.IntersectionPoint = FPoint2f{ xIntersection,lowestTop };

		outOverlapInfo2.isOverlapping = true;
		outOverlapInfo2.IntersectionPoint = FPoint2f{ xIntersection,lowestTop };

		return true;
	}

	bool SCollision::IsOverlapping(
		const FRectf& rect1, const FRectf& rect2,
		FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2,
		const FPoint2f& velocity1, const FPoint2f& velocity2)
	{

		outOverlapInfoRect1.isOverlapping = false;
		outOverlapInfoRect1.IntersectionPoint = FPoint2f{ 0,0 };//not relevant for now
		outOverlapInfoRect1.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfoRect1.DirectionOfCollided = FPoint2i{ 0,0 };

		outOverlapInfoRect2.isOverlapping = false;
		outOverlapInfoRect2.IntersectionPoint = FPoint2f{ 0,0 };//not relevant for now
		outOverlapInfoRect2.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfoRect2.DirectionOfCollided = FPoint2i{ 0,0 };

		float right2{(rect2.Left + rect2.Width)};
		float right1{ (rect1.Left + rect1.Width) };

		float bottom2{ (rect2.Top - rect2.Height) };
		float bottom1{ (rect1.Top - rect1.Height) };

		if (rect2.Left > right1 ||
			rect1.Left > right2)
		{
			return false;
		}

		if (rect2.Top < bottom1 ||
			rect1.Top < bottom2)
		{
			return false;
		}

		outOverlapInfoRect1.isOverlapping = true;
		outOverlapInfoRect1.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfoRect1.DirectionOfCollided = FPoint2i{ 0,0 };

		outOverlapInfoRect2.isOverlapping = true;
		outOverlapInfoRect2.TranslationVector = FPoint2f{ 0,0 };
		outOverlapInfoRect2.DirectionOfCollided = FPoint2i{ 0,0 };

		/*
		Procedure
		- choose axis of collision based on intersection points
		- choose direction of collision on axis based on velocity
			- choose whichever velocity magnitude is lower in that axis
		- choose length of collision based on distance

		*/

		// --- VARIABLES FOR CHOOSING AXIS---
		bool top2_Intersects{ rect2.Top <= rect1.Top && rect2.Top >= bottom1 };
		bool bottom2_Intersects{ bottom2 <= rect1.Top && bottom2 >= bottom1 };

		bool left2_Intersects{ rect2.Left >= rect1.Left && rect2.Left <= right1};
		bool right2_Intersects{ right2 >= rect1.Left && right2 <= right1 };

		if (top2_Intersects && bottom2_Intersects && left2_Intersects && right2_Intersects ||
			(!top2_Intersects && !bottom2_Intersects && !left2_Intersects && !right2_Intersects)) //rect 2 is completely inside or outside
		{
			//choose velocity
			
			if (SVectors::Length(velocity1) > SVectors::Length(velocity2))
			{
				if (abs(velocity1.X) > FLT_EPSILON && abs(velocity1.X) < abs(velocity1.Y))
				{
					ChooseX(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
				else
				{
					ChooseY(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
			}
			else
			{
				if (abs(velocity2.X) > FLT_EPSILON && abs(velocity2.X) < abs(velocity2.Y))
				{
					ChooseX(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
				else
				{
					ChooseY(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
			}
		}
		else if (
			((top2_Intersects && bottom2_Intersects) && (left2_Intersects || right2_Intersects)) ||
			((top2_Intersects || bottom2_Intersects) && (left2_Intersects && right2_Intersects)) ||
			((top2_Intersects && bottom2_Intersects) || (left2_Intersects && right2_Intersects))) // two intersections
		{
			//choose the one who intersected of the couple who both didn't intersect
		
			if (!(top2_Intersects && bottom2_Intersects))
			{
				//choose perpendicular to this == Y axis
		
				if (abs(velocity1.Y) > abs(velocity2.Y))
				{

					ChooseY(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
				else
				{

					ChooseY(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
			}
			else if(!(left2_Intersects && right2_Intersects))
			{
				//choose perpendicular to this == X axis
		
				if (abs(velocity1.X) > abs(velocity2.X))
				{

					ChooseX(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
				else
				{

					ChooseX(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
			}
		}
		//else if ((top2_Intersects || bottom2_Intersects) && (left2_Intersects || right2_Intersects))
		//{
		//	//choose the closest
		//	if (true)
		//	{
		//
		//	}
		//}
		else if ((top2_Intersects || bottom2_Intersects) || (left2_Intersects || right2_Intersects))
		{
			//choose the one that intersected
			if (top2_Intersects || bottom2_Intersects)
			{
				//Choose Y axis
				if (abs(velocity1.Y) > abs(velocity2.Y))
				{

					ChooseY(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
				else
				{

					ChooseY(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
			}
			else if (left2_Intersects || right2_Intersects)
			{
				//choose X axis
				if (abs(velocity1.X) > abs(velocity2.X))
				{

					ChooseX(rect1, rect2, outOverlapInfoRect1, outOverlapInfoRect2, velocity1);
				}
				else
				{

					ChooseX(rect2, rect1, outOverlapInfoRect2, outOverlapInfoRect1, velocity2);
				}
			}
		
		}
		
		


		return true;
	}

	void SCollision::ChooseX(
		const FRectf& rectWithVelocityToDecide, const FRectf& otherRect,
		FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2,
		const FPoint2f& velocity1)
	{
		float right1{ rectWithVelocityToDecide.Left + rectWithVelocityToDecide.Width };
		float right2{ otherRect.Left + otherRect.Width};

		if (velocity1.X < 0)
		{
			outOverlapInfoRect1.TranslationVector.X = right2 - rectWithVelocityToDecide.Left;
		}
		else
		{
			outOverlapInfoRect1.TranslationVector.X = otherRect.Left - right1;
		}

		outOverlapInfoRect1.DirectionOfCollided.X = velocity1.X < 0 ? -1 : 1;//direction of velocity

		outOverlapInfoRect2.TranslationVector.X = -outOverlapInfoRect1.TranslationVector.X;
		outOverlapInfoRect2.DirectionOfCollided.X = -outOverlapInfoRect1.DirectionOfCollided.X;
	}

	void SCollision::ChooseY(
		const FRectf& rectWithVelocityToDecide, const FRectf& otherRect,
		FResolveOverlapInfo& outOverlapInfoRect1, FResolveOverlapInfo& outOverlapInfoRect2, 
		const FPoint2f& velocity1)
	{

		float bottom1{ rectWithVelocityToDecide.Top - rectWithVelocityToDecide.Height };
		float bottom2{ otherRect.Top - otherRect.Height };

		if (velocity1.Y < 0)
		{
			outOverlapInfoRect1.TranslationVector.Y = otherRect.Top - bottom1;
		}
		else
		{
			outOverlapInfoRect1.TranslationVector.Y = bottom2 - rectWithVelocityToDecide.Top;
		}

		outOverlapInfoRect1.DirectionOfCollided.Y = velocity1.Y < 0 ? -1 : 1;//direction of velocity

		outOverlapInfoRect2.TranslationVector.Y = -outOverlapInfoRect2.TranslationVector.Y;
		outOverlapInfoRect2.DirectionOfCollided.Y = -outOverlapInfoRect2.DirectionOfCollided.Y;
	}

	bool SCollision::IsOverlapping(const FRecti& rect1, const FRecti& rect2)
	{
		if (rect2.Left > (rect1.Left + rect1.Width)) return false;
		if (rect1.Left > (rect2.Left + rect2.Width)) return false;

		if (rect2.Top > (rect1.Top + rect1.Height)) return false;
		if (rect1.Top > (rect2.Top + rect2.Height)) return false;

		return true;
	}

	bool SCollision::IsOverlapping(const FRect& rect1, const FRect& rect2)
	{
		if (rect2.Left > (rect1.Left + rect1.Width)) return false;
		if (rect1.Left > (rect2.Left + rect2.Width)) return false;

		if (rect2.Top > (rect1.Top + rect1.Height)) return false;
		if (rect1.Top > (rect2.Top + rect2.Height)) return false;

		return true;
	}

#pragma endregion
}



