#pragma once

#include <string>
#include "../../include/Core/UEntityHandle.h"
#include "../../include/Systems/TemplateSystems/TActivatableFlatMap.h"

namespace Pillar {


	enum class EStateRemovalType {
		Local,
		AllStateMachines,
		None

	};

	enum class EEventStateToTrigger {
		ConditionalExecution,
		ForcedExecution,
		ConditionalRemoval,
		ForcedRemoval
	};

	struct FEventNameAndEventStateToTrigger {
		std::string EventName;
		EEventStateToTrigger EventStateToTrigger;
	};

	template<typename Type>
	struct FState {
		TActivatableFlatMap<std::string, Type> EventName_To_State;
		UEntityHandle StateControllerEntityHandle;
	};

	enum class EEventState {
		Inactive = 0,
		Active = 1,
		Null = 2
	};

	enum class EEventTriggerConditionType {
		GateAndTrigger,
		OnlyTrigger, //if meet conditions -> trigger
		OnlyGate, // if doesn't meet conditions -> don't trigger
	};

	struct FEventTriggerCondition {
		TActivatableFlatMap<std::string, EEventState> EventName_To_ActiveState;
		EEventTriggerConditionType EventTriggerConditionType{ EEventTriggerConditionType::GateAndTrigger };
	};
}