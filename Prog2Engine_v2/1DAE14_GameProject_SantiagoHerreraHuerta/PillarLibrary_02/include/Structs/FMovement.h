#pragma once

#include <vector>
#include "../../include/Structs/FVector.h"
#include "../../include/Structs/FCurve.h"
#include "../../include/Structs/FState.h"
#include "../../include/Core/UEntityHandleContainer.h"

namespace Pillar {

	enum class EOperationType {
		Add,
		Override
	};

	struct FRotationTransformation {
		FRotation3f Rotation;
		EOperationType RotationType;
		bool TerminateEachTick{true};
	};

	enum class EMovementType {
		AddVelocity,
		AddForce,
		OverrideVelocity,
		OverrideForce
	};

	struct FMovement {
		FPoint2f Movement;
		EMovementType MovementType;
	};

	//when the movements that are time dependent end
	enum class EMovementTerminationType {
		TerminateAfterTimeTranspired,
		TerminateEachTick
	};

	struct FSpeedAtDistance {
		float MaxDistanceThreshold;
		float Speed;
	};

	struct FTargetInformation {
		FPoint2f TriggerDistance;
		FPoint2f DistanceFromTarget;
	};

	struct FTarget {
		FTargetInformation CurrentTargetInfo;
		UEntityHandle CurrentTarget;

		FTargetInformation CurrentPlayerTargetInfo;
		UEntityHandle PlayerTarget;
	};



	struct FTargetsOfInterest {
		UEntityHandleMap<FTargetInformation> Targets;
		size_t CurrentTargetIndex;
		EValueBehavior RangeBehaviourOfTargetIndex;
		bool CurrentIndexIncreases{ true };
		EStateRemovalType StateRemovalType{ EStateRemovalType::Local};
	};

	//To Do -> first revamp all of the spline class
	struct FSplineMovementOverTime {
		DSpline MovementDirection;
		double DistanceBetweenHeadAndTailOfVectorInTime;
		EMovementType MovementType;
		FControlledCurve ChangeInMagnitude;
		float TopMagnitude;
		float BottomMagnitude;
		float TimeOfMovement;
		float DirectionOfMovementInAngles;
	};

	enum class EMovementDirectionType {
		OnlyCustom,
		FromAttackingEntityCollisionDirection,
		FromAttackingEntityDirection
	};

	//To Do -> revamp curve class
	struct FVectorMovementOverTime {
		EMovementDirectionType MovementDirectionType{ EMovementDirectionType::OnlyCustom };
		bool GetXAxisFromMovementDirection{ true };
		bool GetYAxisFromMovementDirection{ true };
		bool AddCustomToMovement{ false };
		UEntityHandle EntityToGetMovementDirectionInfoFrom;

		FPoint2f MovementDirection;
		EMovementType MovementType;
		EMovementTerminationType MovementTerminationType;
		bool ApplyMovementOnXAxis{ true };
		bool ApplyMovementOnYAxis{ true };	

		FControlledCurve ChangeInMagnitude;
		float TopMagnitude{1.f};
		float NormalMagnitude{1.f};
		float BottomMagnitude{1.f};

		bool ResetMovementCounterOnTermination;
		float TimeOfMovement;
		float CurrentTime;

		EMovementDirectionType MovementOnStartDirectionType{ EMovementDirectionType::OnlyCustom };
		bool AddCustomToStartMovement{ false };
		float StartMovementMultiple{ 1.f };
		UEntityHandle EntityToGetStartMovementDirectionInfoFrom;

		bool OverrideMovementOnStart;
		FPoint2f MovementValueForOverrideOnStart;
		EMovementType MovementTypeForStartOverride;
		bool ApplyStartMovementOnXAxis{ true };
		bool ApplyStartMovementOnYAxis{ true };
	};

	struct FVectorMovement {
		FPoint2f Movement;
		bool TerminateEachTick{ false };
		EMovementType MovementType;
		bool ApplyMovementOnXAxis{ true };
		bool ApplyMovementOnYAxis{ true };
	};

}