#pragma once

#include "../../../include/Structs/FMovement.h"
#include "../../../include/Core/UEntityHandle.h"

namespace Pillar {

	class SMovementCreation {
	public:
		static FVectorMovementOverTime GetHitMovement(const UEntityHandle& handle);
		static FVectorMovementOverTime GetJumpMovement();
	};
}