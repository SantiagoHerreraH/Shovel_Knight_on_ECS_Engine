#pragma once

#include <unordered_set>
#include <unordered_map>

#include "../../include/Structs/FLevelPartition.h"
#include "../../include/Structs/FVector.h"
#include "../../include/Structs/FShape.h"

namespace Pillar {

#pragma region Coordinate

	struct FCoordinateHasher {

		explicit FCoordinateHasher(const FParams_Grid& paramsForLevelPartition) :
			m_ParamsForLevelPartition(paramsForLevelPartition) {}

		//put it in scale
		int Hash(const FPoint2i& coordinate);
		int Hash(const FPoint2f& coordinate);
		int Hash(const FPoint2& coordinate);

	private:
		const FParams_Grid& m_ParamsForLevelPartition;
	};

#pragma endregion
}

