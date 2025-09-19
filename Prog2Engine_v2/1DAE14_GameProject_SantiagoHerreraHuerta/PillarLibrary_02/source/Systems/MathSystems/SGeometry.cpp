
#include "../../../include/Systems/MathSystems/SGeometry.h"
#include "../../../include/Systems/MathSystems/STrigonometry.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {

	const FPoint2f SGeometry::Center(FMesh2Df& mesh)
	{
		FPoint2f finalCenter{};
		for (size_t i = 0; i < mesh.Triangles.size(); i++)
		{
			FPoint2f currentCenter = STrigonometry::Center(mesh.Triangles[i]);
			finalCenter.X += currentCenter.X;
			finalCenter.Y += currentCenter.Y;
		}

		if (!mesh.Triangles.empty())
		{
			finalCenter.X /= mesh.Triangles.size();
			finalCenter.Y /= mesh.Triangles.size();
		}

		return finalCenter;
	}

	const FPoint2f SGeometry::Center(FShape2Df& shape)
	{

		FPoint2f finalCenter{};
		for (size_t i = 0; i < shape.Shape.size(); i++)
		{
			finalCenter.X += shape.Shape[i].X;
			finalCenter.Y += shape.Shape[i].Y;
		}

		finalCenter.X /= shape.Shape.size();
		finalCenter.Y /= shape.Shape.size();

		return finalCenter;
	}

	void SGeometry::MakeRect(float width, float height, FPoint2f center, FShape2Df& shape)
	{
		shape.Center = center;

		shape.Shape.resize(4);
		shape.Shape.at(0) = FPoint2f(center.X - width / 2.f, center.Y - height / 2.f);
		shape.Shape.at(1) = FPoint2f(center.X + width / 2.f, center.Y - height / 2.f);
		shape.Shape.at(2) = FPoint2f(center.X + width / 2.f, center.Y + height / 2.f);
		shape.Shape.at(3) = FPoint2f(center.X - width / 2.f, center.Y + height / 2.f);
	}

	std::vector<Rectf> SGeometry::Simplify(const std::vector<Rectf>& rects, ERectSimplificationMode rectSimplificationMode)
	{
		if (rects.empty())
		{
			return rects;
		}
		bool gotFused{ false };



		std::vector<Rectf> firstFusedRects;
		firstFusedRects.reserve(rects.size());
		firstFusedRects.push_back(rects.at(0));

		for (size_t rectsIdx = 1; rectsIdx < rects.size(); rectsIdx++)
		{
			for (size_t vertRectsIndex = 0; vertRectsIndex < firstFusedRects.size(); vertRectsIndex++)
			{
				switch (rectSimplificationMode)
				{
				case Pillar::ERectSimplificationMode::PrioritizeVerticalRect:

					if (CanBeOneVertical(rects[rectsIdx], firstFusedRects[vertRectsIndex], 1, 2, 1))
					{
						firstFusedRects[vertRectsIndex] = Fuse(rects[rectsIdx], firstFusedRects[vertRectsIndex]);
						gotFused = true;
						
					}

					break;
				case Pillar::ERectSimplificationMode::PrioritizeHorizontalRects:

					if (CanBeOneHorizontal(rects[rectsIdx], firstFusedRects[vertRectsIndex], 1, 2, 1))
					{
						firstFusedRects[vertRectsIndex] = Fuse(rects[rectsIdx], firstFusedRects[vertRectsIndex]);
						gotFused = true;
						
					}

					break;
				}

				if (gotFused)
				{
					break;
				}
				
			}

			if (!gotFused)
			{
				firstFusedRects.push_back(rects[rectsIdx]);
			}

			gotFused = false;
		}

		std::vector<Rectf> secondFusedRects;
		secondFusedRects.reserve(firstFusedRects.size());
		secondFusedRects.push_back(firstFusedRects.at(0));

		

		for (size_t vertRectIdx = 1; vertRectIdx < firstFusedRects.size(); vertRectIdx++)
		{
			for (size_t horizontalRectsIdx = 0; horizontalRectsIdx < secondFusedRects.size(); horizontalRectsIdx++)
			{
				switch (rectSimplificationMode)
				{
				case Pillar::ERectSimplificationMode::PrioritizeVerticalRect:

					if (CanBeOneHorizontal(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx], 1, 2, 1))
					{
						secondFusedRects[horizontalRectsIdx] = Fuse(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx]);
						gotFused = true;
						break;
					}

					break;
				case Pillar::ERectSimplificationMode::PrioritizeHorizontalRects:

					if (CanBeOneVertical(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx], 1, 2, 1))
					{
						secondFusedRects[horizontalRectsIdx] = Fuse(firstFusedRects[vertRectIdx], secondFusedRects[horizontalRectsIdx]);
						gotFused = true;
						break;
					}

					break;
				}
				
			}

			if (!gotFused)
			{

				secondFusedRects.push_back(firstFusedRects[vertRectIdx]);
			}

			gotFused = false;
		}

		

		return secondFusedRects;
	}

	bool SGeometry::CanBeOneHorizontal(const Rectf& rect1, const Rectf& rect2, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableHeightDelta)
	{
		float deltaBottom{ abs(rect1.bottom - rect2.bottom) };

		if (rect2.left > (rect1.left + rect1.width + maxAcceptableXDistance)) return false;
		if (rect1.left > (rect2.left + rect2.width + maxAcceptableXDistance)) return false;
		if (maxAcceptableHeightDelta > 0 && abs(rect1.height - rect2.height) > maxAcceptableHeightDelta) return false;

		return (deltaBottom <= maxAcceptableYDistance);
	}

	bool SGeometry::CanBeOneVertical(const Rectf& one, const Rectf& two, float maxAcceptableYDistance, float maxAcceptableXDistance, float maxAcceptableWidthDelta)
	{
		float deltaLeft{ abs(one.left - two.left) };

		if (one.bottom > (two.bottom + two.height + maxAcceptableYDistance)) return false; 
		if (two.bottom > (one.bottom + one.height + maxAcceptableYDistance)) return false;

		if (maxAcceptableWidthDelta > 0 && abs(one.width - two.width) > maxAcceptableWidthDelta) return false;

		return deltaLeft <= maxAcceptableXDistance;
	}

	Rectf SGeometry::Fuse(const Rectf& one, const Rectf& two)
	{
		float topOne{ one.bottom + one.height};
		float topTwo{ two.bottom + two.height };
		float rightOne{ one.left + one.width };
		float rightTwo{ two.left + two.width };

		float lowestBottom = one.bottom < two.bottom ? one.bottom : two.bottom;
		float lowestLeft = one.left < two.left ? one.left : two.left;
		float highestTop = topOne > topTwo ? topOne : topTwo;
		float highestRight = rightOne > rightTwo ? rightOne : rightTwo;

		return Rectf(lowestLeft, lowestBottom, highestRight - lowestLeft, highestTop - lowestBottom);
	}

	FPoint2f SGeometry::EvaluateLine(const FLine2f& line, float percentage)
	{
		return SVectors::Add(line.Start, SVectors::Scale(SVectors::Subtract(line.End, line.Start), percentage));
	}
}