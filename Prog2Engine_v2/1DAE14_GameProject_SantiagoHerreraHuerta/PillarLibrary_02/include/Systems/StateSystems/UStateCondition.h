#pragma once

#include <string>

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UEntityHandle.h"
#include "../../../include/Structs/FState.h"

namespace Pillar {

	struct FSingleStateCondition {
		FSingleStateCondition() {};
		explicit FSingleStateCondition(const std::string& stateName, EEventState eventState, const UEntityHandle& entityHandle) :
			StateName(stateName),
			EventState(eventState),
			StateMachineEntityHandle(entityHandle){}
		std::string StateName{};
		EEventState EventState{};
		UEntityHandle StateMachineEntityHandle{};
	};

	class UStateCondition final{

	public:
		UStateCondition(URegistry& registry);

		void Set(const std::string& stateName, EEventState eventState, const UEntityHandle& stateMachineEntityHandle);
		void And(const std::string& stateName, EEventState eventState, const UEntityHandle& stateMachineEntityHandle);

		bool IsFulfilled()const;
		
	private:
		URegistry* m_Registry;
		std::vector<FSingleStateCondition> m_Conditions;
	};

	struct FStateConditionAndConditionType {
		explicit FStateConditionAndConditionType(const UStateCondition& condition, EEventTriggerConditionType conditionType) : 
			StateCondition(condition), 
			EventTriggerConditionType(conditionType){}
		UStateCondition StateCondition;
		EEventTriggerConditionType EventTriggerConditionType;
	};

	class UStateConditionsController {

	public:
		void AddCondition(const std::string& stateName, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType);
	
		size_t NumOfConditions() const;

		bool GateConditionsAreFulfilled(size_t stateIndex)const;
		bool TriggerConditionsAreFulfilled(size_t stateIndex)const;

		const std::string& StateNameAt(size_t stateIndex)const;

		bool GateConditionsAreFulfilled(const std::string& stateName)const;
		bool TriggerConditionsAreFulfilled(const std::string& stateName)const;

		bool ConditionContainsState(const std::string& stateName)const;
	private:
		TActivatableFlatMap<std::string, std::vector<FStateConditionAndConditionType>> m_StateName_To_StateConditions;//each condition is tied to an entity handle (one condition can have multiple entities)
	
	};
}