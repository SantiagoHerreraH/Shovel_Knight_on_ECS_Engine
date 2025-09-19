#pragma once

#include <array>
#include <vector>
#include "../../include/Core/UEntityHandleContainer.h"
#include "../../include/Structs/FVector.h"
#include "../../include/Structs/FState.h"

namespace Pillar {

	struct FTransform {
		FPoint3f Position;
		FRotation3 Rotation;
		FPoint3f Scale;
	};
	//I should change Rotation3 to quaternions

	struct FLocalTransform {
		FPoint3f Position;
		FRotation3 Rotation;
		FPoint3f Scale;
	};

#pragma region 2D Transforms

	struct FTransform2D {
		FPoint2f Position{ 0.f,0.f };
		double RotationX{ 0 };
		double RotationY{ 0 };
		double RotationZ{ 0 };
		FPoint2f Scale{ 1.f,1.f };
	};

	struct FLocalTransform2D {
		FTransform2D Transform;
	};

	struct FWorldTransform2D {
		FTransform2D Transform;
	};

#pragma endregion

	enum class EValueModificationType {
		DontChange,
		OverrideCustom,
		OverrideFromOtherEntity
	};

	struct FTransformState {

		FTransform2D Transform{};
		EValueModificationType ValueModificationTypeFor_Position{ EValueModificationType::DontChange };
		EValueModificationType ValueModificationTypeFor_Rotation{ EValueModificationType::DontChange };
		EValueModificationType ValueModificationTypeFor_Scale{ EValueModificationType::DontChange };

		UEntityHandleContainer ReferenceEntitiesFor_Transform;
		size_t Current_Transform_ReferenceEntities_Index;
		bool IncreaseIndexAfterUsingReference{ false };

		EStateRemovalType StateRemovalType{ EStateRemovalType::Local };
	};
}
