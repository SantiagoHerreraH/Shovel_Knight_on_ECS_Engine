#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../../include/Core/UEntityHandle.h"
#include "../../include/Core/UEntityHandleContainer.h"
#include "../../include/Structs/FOptimization.h"


namespace Pillar {

	struct FSceneNode {
		UEntityHandle Parent; 
		UEntityHandleContainer Children;
		bool Traversed{ false };
	};

	struct FSceneObject {

		int SceneIndex{ 0 };
	};
	
	struct FSceneRegistry {

		//For Scene Nodes
		UEntityHandleContainer RootEntities;
		UEntityHandleMap<size_t> Entity_To_EntitySceneHierarchyIndex;
		std::vector<UEntityHandleContainer> SceneHierarchies;

		//For Scene Objects
		std::vector<UEntityHandleMap<EActivationStateForOptimization>> SceneEntities;
		int CurrentSceneIndex{0};
	};
	
}