#pragma once

#include <string>
#include <unordered_map>
#include <queue>

#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FState.h"

namespace Pillar {

	/*
	
		Lets try and set a distinction between state and events
		- State is the qualitative distinction between events, meaning state is always bound to an event
		- This string (event), means this action (data)
		- Each event has a start and an end
		- Both can be triggered
		- The events can be triggered explicitly (player input) or contextually
		- Also there are data bound events and general events
		- The data bound events have independent lifetime even though they share the same string id; each state controller manages them
		- The general events dictate the lifetime of the data bound events <--- not sure about this one yet

		Possible ways to make this more powerful but slower -> so instead of triggering an event -> it would be changing its state
		- Add State to the Events themselves (in progress, terminating, etc) -> not just on or off
		- This can be an enum or a string
	*/

	struct FEventExecutionAndRemovalInfo {

		std::unordered_set<std::string> EventsToParticipateInExecution;
		std::unordered_set<std::string> EventsToParticipateInRemoval;

		std::vector<FEventTriggerCondition> ConditionsToTriggerExecution;
		std::vector<FEventTriggerCondition> ConditionsToTriggerRemoval;
	};

	class IStateController {

	public:
		virtual const UEntityHandle& GetEntityHandle() const = 0;
		virtual bool TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger) = 0;
		virtual bool IsValid() const = 0;
		virtual EEventState EventState(const std::string& eventOrActionName)const = 0;
		virtual const TActivatableFlatMap<std::string, EEventState>& GetConstData()const = 0;
	};

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	class UStateController final : public IStateController {

	public:
		UStateController(URegistry& registry, const UEntityHandle& stateControllerEntityHandle);
		UStateController(const IStateController* other);

		const UEntityHandle& GetEntityHandle() const override;
		bool IsValid() const override;

		EEventState EventState(const std::string& eventOrActionName)const override;

		bool TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger) override;

		//the eventName has to be an event with state (no nested ifs allowed for now) aka be direct with the boolean logic
		//here you are assuming that the execution triggers will be on the execution of the other events
		bool AddConditionsToExecute(const std::string& eventName, const FEventTriggerCondition& eventExecutionTriggers);//one group is like && one more function cal is like ||
		bool AddConditionsToRemove(const std::string& eventName, const FEventTriggerCondition& eventRemovalTriggers);

		bool AddState(const std::string& eventName, const Type& state);
		void RemoveState(const std::string& eventName);

		virtual const TActivatableFlatMap<std::string, EEventState>& GetConstData()const override;

	protected:

		bool ExplicitlyTriggeredEvent_CanExecute(const std::string& eventName);
		bool EventTriggeredByConditions_CanExecute(const std::string& eventName);

		bool ExplicitlyTriggeredEvent_CanBeRemoved(const std::string& eventName);
		bool EventTriggeredByConditions_CanBeRemoved(const std::string& eventName);

		void ExecuteCurrentEvent(const std::string& eventName);
		void RemoveCurrentEvent(const std::string& eventName);
		bool IsActive(const std::string& eventName);

		//constexpr bool m_AllowsMultipleStatesAtTheSameTime{ allowsMultipleStatesAtTheSameTime };
		//constexpr bool m_AllowsNoState{ allowsNoState };
		URegistry* m_Registry{nullptr};
		UEntityHandle m_StateControllerEntityHandle;

		TActivatableFlatMap<std::string, EEventState> m_EventName_To_ActiveState;// for now lets just flip the bool and not actually move them
		std::unordered_map<std::string, FEventExecutionAndRemovalInfo> m_EventName_To_EventExecutionAndRemovalInfo;
		//when the events end their active state, you also have to check the events that were linked to that one in a NOT fashion

		std::queue<FEventNameAndEventStateToTrigger> m_EventsToRemoveOrExecute{};
	};

#pragma region State Controller Implementation

#pragma region Constructor


	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::UStateController(URegistry& registry, const UEntityHandle& stateControllerEntityHandle) :
		m_Registry(&registry),
		m_StateControllerEntityHandle(stateControllerEntityHandle)
	{

		FState<Type> state{};
		state.StateControllerEntityHandle = m_StateControllerEntityHandle;
		m_Registry->AddComponent<FState<Type>>(m_StateControllerEntityHandle, state);
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::UStateController(const IStateController* other) :
		UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>((UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>&)*other){}

#pragma endregion

#pragma region Entity Handle

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline const UEntityHandle& UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState >::GetEntityHandle() const
	{
		return m_StateControllerEntityHandle;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::IsValid() const
	{
		return m_StateControllerEntityHandle.IsValid() && m_Registry;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline EEventState UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventState(const std::string& eventOrActionName) const
	{
		if (!m_EventName_To_ActiveState.Contains(eventOrActionName))
		{
			return EEventState::Null;
		}
		return m_EventName_To_ActiveState.ConstValueAt(eventOrActionName);
	}

#pragma endregion

#pragma region Add and Remove State

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState >::AddState(const std::string& eventName, const Type& value)
	{
		if (IsValid() && !m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			m_EventName_To_EventExecutionAndRemovalInfo.emplace(std::pair(eventName, FEventExecutionAndRemovalInfo{}));
			EEventState activationState{ EEventState::Inactive };
			m_EventName_To_ActiveState.Push(eventName, activationState, false);

			FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };

			if (!state->EventName_To_State.Contains(eventName))
			{
				state->EventName_To_State.Push(eventName, value, false);
			}

			return true;
		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::RemoveState(const std::string& eventName)
	{
		if (IsValid() && m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			//if an event doesn't have a state, it's not worth it to check for its trigger conditions -> which would be the equivalent to having nested ifs
			//if you were to allow nested ifs -> remove these loops
			std::vector<FEventTriggerCondition>& executionConditions{ m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution };
			std::vector<FEventTriggerCondition>& removalConditions{ m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval };
			const std::string* currentConditionEvent{ nullptr };

			for (size_t conditionIdx = 0; conditionIdx < executionConditions.size(); conditionIdx++)
			{
				for (size_t eventIdx = 0; eventIdx < executionConditions.at(conditionIdx).EventName_To_ActiveState.Size(); eventIdx++)
				{
					currentConditionEvent = &executionConditions.at(conditionIdx).EventName_To_ActiveState.ConstKeyAt(eventIdx);
					m_EventName_To_EventExecutionAndRemovalInfo.at(*currentConditionEvent).EventsToParticipateInExecution.erase(eventName);
				}
			}

			for (size_t conditionIdx = 0; conditionIdx < removalConditions.size(); conditionIdx++)
			{
				for (size_t eventIdx = 0; eventIdx < removalConditions.at(conditionIdx).EventName_To_ActiveState.Size(); eventIdx++)
				{
					currentConditionEvent = &removalConditions.at(conditionIdx).EventName_To_ActiveState.ConstKeyAt(eventIdx);
					m_EventName_To_EventExecutionAndRemovalInfo.at(*currentConditionEvent).EventsToParticipateInRemoval.erase(eventName);
				}
			}

			m_EventName_To_EventExecutionAndRemovalInfo.erase(eventName);

			FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };
			state->EventName_To_State.Erase(eventName);
		}
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline const TActivatableFlatMap<std::string, EEventState>& UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::GetConstData() const
	{
		return m_EventName_To_ActiveState;
	}

#pragma endregion

#pragma region Event Execution
	
	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_StateControllerEntityHandle.IsValid())
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				bool triggered{ false };

				m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ eventName, eventStateToTrigger });

				do
				{
					switch (m_EventsToRemoveOrExecute.front().EventStateToTrigger)
					{

					case EEventStateToTrigger::ConditionalExecution:

						triggered = !IsActive(m_EventsToRemoveOrExecute.front().EventName) &&
							ExplicitlyTriggeredEvent_CanExecute(m_EventsToRemoveOrExecute.front().EventName);

						if (!triggered)
						{
							break;
						}

					case EEventStateToTrigger::ForcedExecution:
						ExecuteCurrentEvent(m_EventsToRemoveOrExecute.front().EventName);
						m_EventName_To_ActiveState.ValueAt(m_EventsToRemoveOrExecute.front().EventName) = EEventState::Active;
						triggered = true;
						break;

					case EEventStateToTrigger::ConditionalRemoval:

						triggered = IsActive(m_EventsToRemoveOrExecute.front().EventName) &&
							ExplicitlyTriggeredEvent_CanBeRemoved(m_EventsToRemoveOrExecute.front().EventName);

						if (!triggered)
						{
							break;
						}

					case EEventStateToTrigger::ForcedRemoval:
						RemoveCurrentEvent(m_EventsToRemoveOrExecute.front().EventName);
						m_EventName_To_ActiveState.ValueAt(m_EventsToRemoveOrExecute.front().EventName) = EEventState::Inactive;
						triggered = true;
						break;
					}

					if (triggered)
					{
						auto eventsToExecute_begin{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInExecution.cbegin() };
						auto eventsToExecute_end{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInExecution.cend() };

						for (auto It = eventsToExecute_begin; It != eventsToExecute_end; It++)
						{
							// proactive event triggers only check here
							if (!IsActive((*It)) &&
								EventTriggeredByConditions_CanExecute((*It))) // I think this can still infinite loop -> maybe do it so it can only loop once or twice (hasn't yet)
							{
								m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ (*It) , EEventStateToTrigger::ConditionalExecution });
							}
						}

						auto eventsToRemove_begin{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cbegin() };
						auto eventsToRemove_end{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cend() };

						for (auto It = eventsToRemove_begin; It != eventsToRemove_end; It++)
						{

							if (IsActive((*It)) &&
								EventTriggeredByConditions_CanBeRemoved((*It)))
							{
								m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ (*It) , EEventStateToTrigger::ConditionalRemoval });
							}
						}
					}


					m_EventsToRemoveOrExecute.pop();

				} while (!m_EventsToRemoveOrExecute.empty());

				return true;
			}
		}

		return false;
	}

#pragma endregion

#pragma region Execution Triggers

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::AddConditionsToExecute(const std::string& eventName, const FEventTriggerCondition& eventExecutionTriggers)
	{
		if (IsValid())
		{
			FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };

			if (state->EventName_To_State.Contains(eventName))
			{
				m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.push_back(eventExecutionTriggers);

				const std::string* currentEventName{ nullptr };

				for (size_t eventIdx = 0; eventIdx < eventExecutionTriggers.EventName_To_ActiveState.Size(); eventIdx++)
				{
					currentEventName = &eventExecutionTriggers.EventName_To_ActiveState.ConstKeyAt(eventIdx);
					if (!m_EventName_To_EventExecutionAndRemovalInfo.contains(*currentEventName))
					{
						m_EventName_To_EventExecutionAndRemovalInfo.emplace(
							std::pair(
								*currentEventName,
								FEventExecutionAndRemovalInfo{}));
					}
					if (!m_EventName_To_ActiveState.Contains(*currentEventName))
					{
						m_EventName_To_ActiveState.Push(*currentEventName, EEventState::Inactive, false);
					}

					m_EventName_To_EventExecutionAndRemovalInfo.at(*currentEventName).EventsToParticipateInExecution.emplace(eventName);
				}
				return true;
			}

			
		}

		return false;
		
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::AddConditionsToRemove(const std::string& eventName, const FEventTriggerCondition& eventRemovalTriggers)
	{
		if (IsValid())
		{
			FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };

			if (state->EventName_To_State.Contains(eventName))
			{
				m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.push_back(eventRemovalTriggers);

				const std::string* currentEventName{ nullptr };

				for (size_t eventIdx = 0; eventIdx < eventRemovalTriggers.EventName_To_ActiveState.Size(); eventIdx++)
				{
					currentEventName = &eventRemovalTriggers.EventName_To_ActiveState.ConstKeyAt(eventIdx);
					if (!m_EventName_To_EventExecutionAndRemovalInfo.contains(*currentEventName))
					{
						m_EventName_To_EventExecutionAndRemovalInfo.emplace(
							std::pair(
								*currentEventName,
								FEventExecutionAndRemovalInfo{}));
					}
					if (!m_EventName_To_ActiveState.Contains(*currentEventName))
					{
						m_EventName_To_ActiveState.Push(*currentEventName, EEventState::Inactive, false);
					}

					m_EventName_To_EventExecutionAndRemovalInfo.at(*currentEventName).EventsToParticipateInRemoval.emplace(eventName);
				}
				return true;
			}

		}

		return false;
	}

#pragma endregion

#pragma region Protected Implementation


	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExplicitlyTriggeredEvent_CanExecute(const std::string& eventName)
	{
		if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.empty())
			{
				return true;
			}

			bool noGates{true};
			FEventTriggerCondition* eventTriggerCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.size(); condIdx++)
			{
				eventTriggerCondition = &m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.at(condIdx);
				
				for (size_t eventIdx = 0; eventIdx < eventTriggerCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerCondition->EventTriggerConditionType != EEventTriggerConditionType::OnlyTrigger)
					{
						noGates = false;
					}
					else
					{
						break;
					}

					currentActiveState = (bool)eventTriggerCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_EventName_To_ActiveState.ValueAt(*currentEvent) != currentActiveState)// this should be fine, make sure to set everything correctly
					{
						break;
					} 

					if (eventIdx == (eventTriggerCondition->EventName_To_ActiveState.Size() - 1))
					{
						return true;
					}
				}
			}
			
			if (noGates)
			{
				return true;
			}
		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventTriggeredByConditions_CanExecute(const std::string& eventName)
	{
		if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.empty())
			{
				return true;
			}

			FEventTriggerCondition* eventTriggerCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.size(); condIdx++)
			{
				eventTriggerCondition = &m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.at(condIdx);

				for (size_t eventIdx = 0; eventIdx < eventTriggerCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerCondition->EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
					{
						break;
					}

					currentActiveState = (bool)eventTriggerCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_EventName_To_ActiveState.ValueAt(*currentEvent) != currentActiveState)// this should be fine, make sure to set everything correctly
					{
						break;
					}

					if (eventIdx == (eventTriggerCondition->EventName_To_ActiveState.Size() - 1))
					{
						return true;
					}
				}

			}

		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExplicitlyTriggeredEvent_CanBeRemoved(const std::string& eventName)
	{
		if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.empty())
			{
				return true;
			}

			bool noGates{ true };
			FEventTriggerCondition* eventTriggerRemovalCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.size(); condIdx++)
			{
				eventTriggerRemovalCondition = &m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.at(condIdx);

				for (size_t eventIdx = 0; eventIdx < eventTriggerRemovalCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerRemovalCondition->EventTriggerConditionType != EEventTriggerConditionType::OnlyTrigger)
					{
						noGates = false;
					}
					else
					{
						break;
					}

					currentActiveState = (bool)eventTriggerRemovalCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerRemovalCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_EventName_To_ActiveState.ValueAt(*currentEvent) != currentActiveState)// this should be fine, make sure to set everything correctly
					{
						break;
					}

					if (eventIdx == (eventTriggerRemovalCondition->EventName_To_ActiveState.Size() - 1))
					{
						return true;
					}
				}
			}

			if (noGates)
			{
				return true;
			}

		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventTriggeredByConditions_CanBeRemoved(const std::string& eventName)
	{
		if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.empty())
			{
				return true;
			}

			FEventTriggerCondition* eventTriggerRemovalCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.size(); condIdx++)
			{
				eventTriggerRemovalCondition = &m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerRemoval.at(condIdx);

				for (size_t eventIdx = 0; eventIdx < eventTriggerRemovalCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerRemovalCondition->EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
					{
						break;
					}

					currentActiveState = (bool)eventTriggerRemovalCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerRemovalCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_EventName_To_ActiveState.ValueAt(*currentEvent) != currentActiveState)// this should be fine, make sure to set everything correctly
					{
						break;
					}

					if (eventIdx == (eventTriggerRemovalCondition->EventName_To_ActiveState.Size() - 1))
					{
						return true;
					}
				}

			}

		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExecuteCurrentEvent(const std::string& eventName)
	{
		//if can happen that an event is triggered and checked because it 
		// affects other events, but it's not actually linked to another state
		if constexpr (allowsMultipleStatesAtTheSameTime) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };
				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.Activate(eventName);
				}
			}
		}
		else if constexpr (!allowsMultipleStatesAtTheSameTime) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };

				if (state->EventName_To_State.Contains(eventName))
				{
					size_t zero{ 0 };
					m_EventName_To_ActiveState.ValueAt(state->EventName_To_State.ConstKeyAt(zero)) = EEventState::Inactive;
					state->EventName_To_State.DeactivateAt(zero);
					state->EventName_To_State.Activate(eventName);
				}
			}
		}
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::RemoveCurrentEvent(const std::string& eventName)
	{
		if constexpr (allowsNoState) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };
				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.Deactivate(eventName);
				}
			}
		}
		else if constexpr (!allowsNoState) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };

				if (state->EventName_To_State.Contains(eventName) && state->EventName_To_State.ActivatedSize() > 1)
				{
					state->EventName_To_State.Deactivate(eventName);
				}
			}
		}
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateController<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::IsActive(const std::string& eventName)
	{
		FState<Type>* state{ m_Registry->GetComponent<FState<Type>>(m_StateControllerEntityHandle.GetConstEntity()) };
		return state->EventName_To_State.IsActivated(eventName);
	}

#pragma endregion

#pragma endregion

}