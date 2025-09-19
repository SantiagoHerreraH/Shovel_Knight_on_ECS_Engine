#include "../../../include/Systems/StateSystems/UStateCondition.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include <cassert>

namespace Pillar {

	UStateCondition::UStateCondition(URegistry& registry) : m_Registry(&registry) { }

	void UStateCondition::Set(const std::string& stateName, EEventState eventState, const UEntityHandle& stateMachineEntityHandle)
	{
		UStateMachine* currentStateMachine{ m_Registry->GetComponent<UStateMachine>(stateMachineEntityHandle.GetConstEntity()) };

		assert(currentStateMachine);

		currentStateMachine->AddState(stateName);

		m_Conditions.clear();
		m_Conditions.push_back(FSingleStateCondition(stateName, eventState, stateMachineEntityHandle));
	}
	void UStateCondition::And(const std::string& stateName, EEventState eventState, const UEntityHandle& stateMachineEntityHandle)
	{
		UStateMachine* currentStateMachine{ m_Registry->GetComponent<UStateMachine>(stateMachineEntityHandle.GetConstEntity()) };

		assert(currentStateMachine);

		currentStateMachine->AddState(stateName);

		m_Conditions.push_back(FSingleStateCondition(stateName, eventState, stateMachineEntityHandle));
	}
	bool UStateCondition::IsFulfilled() const
	{
		UStateMachine* currentStateMachine{ nullptr };

		for (size_t condIdx = 0; condIdx < m_Conditions.size(); condIdx++)
		{
			currentStateMachine = m_Registry->GetComponent<UStateMachine>(m_Conditions[condIdx].StateMachineEntityHandle.GetConstEntity());
			
			if (currentStateMachine->EventState(m_Conditions[condIdx].StateName) != m_Conditions[condIdx].EventState)
			{
				return false;
			}
		}

		return true;
	}
	void UStateConditionsController::AddCondition(const std::string& stateName, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType)
	{
		if (!m_StateName_To_StateConditions.Contains(stateName))
		{
			m_StateName_To_StateConditions.Push(stateName, std::vector<FStateConditionAndConditionType>{});
		}

		m_StateName_To_StateConditions.ValueAt(stateName).push_back(FStateConditionAndConditionType(condition, eventTriggerConditionType));
	}
	bool UStateConditionsController::GateConditionsAreFulfilled(const std::string& stateName) const
	{
		if (m_StateName_To_StateConditions.Contains(stateName))
		{
			bool noGateConditions{ true };

			for (size_t condIdx = 0; condIdx < m_StateName_To_StateConditions.ConstValueAt(stateName).size(); condIdx++)
			{
				if (m_StateName_To_StateConditions.ConstValueAt(stateName).at(condIdx).EventTriggerConditionType == EEventTriggerConditionType::OnlyTrigger)
				{
					continue;
				}

				noGateConditions = false;

				if (m_StateName_To_StateConditions.ConstValueAt(stateName).at(condIdx).StateCondition.IsFulfilled())
				{
					return true;
				}
			}

			return noGateConditions;
		}
		
		return false;
	}
	bool UStateConditionsController::TriggerConditionsAreFulfilled(const std::string& stateName) const
	{
		if (m_StateName_To_StateConditions.Contains(stateName))
		{
			bool noTriggerConditions{ true };

			for (size_t condIdx = 0; condIdx < m_StateName_To_StateConditions.ConstValueAt(stateName).size(); condIdx++)
			{
				if (m_StateName_To_StateConditions.ConstValueAt(stateName).at(condIdx).EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
				{
					continue;
				}

				noTriggerConditions = false;

				if (m_StateName_To_StateConditions.ConstValueAt(stateName).at(condIdx).StateCondition.IsFulfilled())
				{
					return true;
				}
			}

			return noTriggerConditions;
		}

		return false;
	}
	const std::string& UStateConditionsController::StateNameAt(size_t stateIndex) const
	{
		return m_StateName_To_StateConditions.ConstKeyAt(stateIndex);
	}
	bool UStateConditionsController::GateConditionsAreFulfilled(size_t stateIndex) const
	{
		for (size_t condIdx = 0; condIdx < m_StateName_To_StateConditions.ConstAt(stateIndex).size(); condIdx++)
		{
			if (m_StateName_To_StateConditions.ConstAt(stateIndex).at(condIdx).EventTriggerConditionType == EEventTriggerConditionType::OnlyTrigger)
			{
				continue;
			}

			if (m_StateName_To_StateConditions.ConstAt(stateIndex).at(condIdx).StateCondition.IsFulfilled())
			{
				return true;
			}
		}

		return false;
	}
	bool UStateConditionsController::TriggerConditionsAreFulfilled(size_t stateIndex) const
	{
		for (size_t condIdx = 0; condIdx < m_StateName_To_StateConditions.ConstAt(stateIndex).size(); condIdx++)
		{
			if (m_StateName_To_StateConditions.ConstAt(stateIndex).at(condIdx).EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
			{
				continue;
			}

			if (m_StateName_To_StateConditions.ConstAt(stateIndex).at(condIdx).StateCondition.IsFulfilled())
			{
				return true;
			}
		}

		return false;
	}
	bool UStateConditionsController::ConditionContainsState(const std::string& stateName)const
	{
		return m_StateName_To_StateConditions.Contains(stateName);
	}
	
	size_t UStateConditionsController::NumOfConditions() const
	{
		return m_StateName_To_StateConditions.Size();
	}
}