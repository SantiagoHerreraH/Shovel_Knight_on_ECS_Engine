
#include <algorithm>
#include "../../../include/Systems//MovementSystems/UMovementData.h"


namespace Pillar {

	void FMovementMagnitudePerDistance::Add(const FSpeedAtDistance& speedAtDistance)
	{
		SpeedPerDistance.emplace_back(speedAtDistance);
		m_isOrdered = false;
	}

	void FMovementMagnitudePerDistance::Remove(size_t index)
	{
		if (index < SpeedPerDistance.size())
		{
			SpeedPerDistance.at(index) = SpeedPerDistance.at(SpeedPerDistance.size() - 1);
			m_isOrdered = false;
		}
	}

	const float FMovementMagnitudePerDistance::GetSpeed(float distance)
	{
		if (SpeedPerDistance.empty())
		{
			return 0;
		}

		Order();

		size_t currentIndex{ 0 };
		size_t maxIndex{ SpeedPerDistance.size() - 1 };
		size_t minIndex{ 0 };
		bool found{ false };

		if (SpeedPerDistance.at(maxIndex).MaxDistanceThreshold < distance)
		{
			return SpeedPerDistance.at(maxIndex).Speed;
		}
		else if (SpeedPerDistance.at(minIndex).MaxDistanceThreshold > distance)
		{
			return SpeedPerDistance.at(minIndex).Speed;
		}

		while (!found)
		{
			currentIndex = (minIndex + maxIndex) / 2;

			if ((maxIndex - minIndex) <= 1)
			{
				if (SpeedPerDistance.at(minIndex).MaxDistanceThreshold >= distance)
				{
					return SpeedPerDistance.at(minIndex).Speed;
				}
				else
				{
					return SpeedPerDistance.at(maxIndex).Speed;
				}
			}

			if (SpeedPerDistance.at(currentIndex).MaxDistanceThreshold < distance)
			{
				minIndex = currentIndex + 1;

			}
			else if (SpeedPerDistance.at(currentIndex).MaxDistanceThreshold > distance)
			{
				maxIndex = currentIndex;

			}
			else
			{
				return SpeedPerDistance.at(currentIndex).Speed;
			}

		}
		
	}

	void FMovementMagnitudePerDistance::Order()
	{
		if (!m_isOrdered)
		{
			std::sort(SpeedPerDistance.begin(), SpeedPerDistance.end(), m_Comparer);
			m_isOrdered = true;
		}	
	}
}