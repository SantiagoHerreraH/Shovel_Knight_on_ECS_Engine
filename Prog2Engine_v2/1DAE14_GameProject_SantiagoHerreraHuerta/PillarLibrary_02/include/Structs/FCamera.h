#pragma once

#include "../../include/Structs/FTransform.h"
#include "../../include/Core/UEntityHandle.h"

namespace Pillar {

	struct FCamera {
		float ScreenWidth;
		float ScreenHeight;
		float ApertureAngle{90.f};

		float MaxAutomaticDepth{ 1.f };
		float MinAutomaticDepth{ 1.f };

		float MaxAutomaticScale{ 1.f };
		float MinAutomaticScale{ 0.5f };
	};
}