
#pragma once

#include "../../../include/Structs/FVector.h"

namespace Pillar {

	class STools {
	
	public:
		static const int GetRandomBetweenRange(int min, int max);

		static const FPoint3f CalculateLerpPoint(const FPoint3f& start, const FPoint3f& end, const double percentageFromZeroToOne);
		static const FPoint2i CalculateLerpPoint(const FPoint2i& start, const FPoint2i& end, const double percentageFromZeroToOne);
		static const FPoint2 CalculateLerpPoint(const FPoint2& start, const FPoint2& end, const double percentageFromZeroToOne);
		static const FPoint2f CalculateLerpPoint(const FPoint2f& start, const FPoint2f& end, const double percentageFromZeroToOne);
		static constexpr double CalculateLerpValue(const double start, const double second, const double percentageFromZeroToOne);
	
		static constexpr bool IsEven(double value);
		static constexpr bool IsEven(float value);
		static constexpr bool IsEven(int value);
		static constexpr double Clamp(const double value, const double min, const double max);
		static constexpr double Loop(const double value, const double min, const double max);
		static constexpr double Rebound(double value, double min, double max);
		static constexpr double DoubleRemainderAfterDivision(const double totalQuantity, const double valueToFitInOther);
		static constexpr double PercentageOfTotalQuantityRemaindingAfterDivision(const double totalQuantity, const double valueToFitInOther);
		
	
	};

#pragma region Implementation

	constexpr double STools::CalculateLerpValue(const double first, const double second, const double percentageFromZeroToOne)
	{
		return first + ((second - first) * Clamp(percentageFromZeroToOne, 0, 1));
	}

	constexpr bool STools::IsEven(double value)
	{
		return !(int(value) % 2);
	}

	constexpr bool STools::IsEven(float value)
	{
		return !(int(value) % 2);
	}

	constexpr bool STools::IsEven(int value)
	{
		return !(value % 2);
	}

	constexpr double STools::Clamp(const double value, const double min, const double max) {

		return value < min ? min : value > max ? max : value;

	}

	constexpr double STools::Loop(const double value, const double min, const double max)
	{
		double delta{ max - min };
		if (delta == 0)
		{
			return min;
		}
		double loopedValue{ value };
		double percentageRemainderOfTotalQuantity{ PercentageOfTotalQuantityRemaindingAfterDivision(delta, value) };

		if (value < min)
		{
			loopedValue = max - percentageRemainderOfTotalQuantity;
		}
		else if (value > max)
		{
			loopedValue = min + percentageRemainderOfTotalQuantity;

		}

		return loopedValue;
	}

	constexpr double STools::Rebound(double value, double min, double max)
	{
		double reboundedValue{ value };

		double delta{ max - min };

		if (delta == 0)
		{
			return min;
		}

		bool numberOfLoopsIsEven = IsEven(value / delta);

		double percentageRemainderOfTotalQuantity{ PercentageOfTotalQuantityRemaindingAfterDivision(delta, value) };


		if (numberOfLoopsIsEven)
		{
			reboundedValue = min + percentageRemainderOfTotalQuantity;
		}
		else
		{
			reboundedValue = max - percentageRemainderOfTotalQuantity;
		}

		return reboundedValue;
	}

	constexpr double STools::DoubleRemainderAfterDivision(const double totalQuantity, const double valueToFitInOther)
	{
		double percentage = (valueToFitInOther / totalQuantity);
		percentage -= int(percentage);

		return percentage;
	}

	constexpr double STools::PercentageOfTotalQuantityRemaindingAfterDivision(const double totalQuantity, const double valueToFitInOther)
	{
		return  DoubleRemainderAfterDivision(totalQuantity, valueToFitInOther) * totalQuantity;
	}

#pragma endregion

}
