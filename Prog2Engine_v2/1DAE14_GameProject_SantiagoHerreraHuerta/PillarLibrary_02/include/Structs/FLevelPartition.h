#pragma once

#include <unordered_map>
#include <unordered_set>
#include "../../include/Structs/FShape.h"
#include "../../include/Core/UEntityHandle.h"

namespace Pillar {
#pragma region Level Partition

	struct FParams_Grid {
		int BoundaryWidth{100};
		int BoundaryHeight{100};
		unsigned int NumberOfRows{50};
		unsigned int NumberOfColumns{50};
	};

	//put this in a component subtypeafterwards -> to make this safe you have to format it differently
	struct FObjectMaxCollisionRadiusPerGrid {
		std::vector<int> Radiuses; //to check num of level partition boundaries nearby
		//this has to have the same size and order as the FParams_LevelPartitions
	};

	struct FParams_Grids {
		std::vector<FParams_Grid> LevelPartitionParams;
		UEntityHandle CollisionGridEntityHandle;

	};

	struct FGridCell {
		FRectf Boundary;
		FPoint2f Center;

		size_t Coordinate_Index; //id is the index in Coordinate
		size_t LevelPartition_Index; //level partition is the index in LevelPartition
	};

	struct FObjectMaxCollisionRadius {
		double Radius;
	};

#pragma endregion 
}