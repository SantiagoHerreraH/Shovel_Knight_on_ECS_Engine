#pragma once

#include "../../include/Core/UEntityHandleContainer.h"

namespace Pillar {

	enum class EActivationStateForOptimization {
		ActiveBeforeDeactivating,
		UnactiveBeforeDeactivating
	};

	struct FOptimizationGridCellState {
		bool Active;
		UEntityHandleMap<EActivationStateForOptimization> EntitiesInBounds_To_ActivationStateForOptimization;//On not colliding with camera all of the collided entities put them here. On colliding with camera, activate all entities
	};
}