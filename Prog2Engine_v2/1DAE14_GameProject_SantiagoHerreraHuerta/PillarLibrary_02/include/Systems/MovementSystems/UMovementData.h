#pragma once

#include "../../../include/Structs/FMovement.h"

namespace Pillar {

	struct FCompareSpeedAtDistance {

		bool operator()(FSpeedAtDistance& first, FSpeedAtDistance& second) {
			return first.MaxDistanceThreshold < second.MaxDistanceThreshold;
		}
	};

	//You can abstract this into sorted vector, but for now it's not important
	struct FMovementMagnitudePerDistance final {

	public:
		void Add(const FSpeedAtDistance& speedAtDistance);
		void Remove(size_t index);
		const float GetSpeed(float distance);
		void Order();

	private:
		
		bool m_isOrdered{true};
		FCompareSpeedAtDistance m_Comparer;
		std::vector<FSpeedAtDistance> SpeedPerDistance;
	};

	struct FHomingInformation {
		FMovementMagnitudePerDistance MovementMagnitudePerDistance;
		bool MovementIsRelativeToDistance{ true };
		EMovementType MovementType;
		bool ApplyOnX{true};
		bool ApplyOnY{true};
		bool IsActive{ true };
	};

	struct FHomingInfoState {
		FHomingInformation HomingInfo;
		EStateRemovalType StateRemovalType{ EStateRemovalType::Local};
	};

	struct FLookAtRotationInfo {
		bool ApplyOnY{true};
		bool ApplyOnZ{false};
		bool IsActive{ true };
	};

	struct FLookAtRotationInfoState {
		FLookAtRotationInfo LookAtRotationInfo;
		EStateRemovalType StateRemovalType{ EStateRemovalType::Local };
	};
}