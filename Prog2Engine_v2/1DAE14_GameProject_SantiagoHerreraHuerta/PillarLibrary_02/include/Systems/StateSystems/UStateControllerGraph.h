#pragma once

#include "../../../include/Systems/StateSystems/UStateController.h"

namespace Pillar {

	/*
	For Sprite
	- Only one activated
	- No Null state

	- On Trigger Execute -> Executes if linked
	- On Trigger Remove -> Only Removes if there is an active linked
	- There is only one active state in the gated state
	- However, the state controller graph keeps tabs internally of all of the states that are active
	
	*/

	enum class ELinkingAction {
		OneWay,
		BothWays
	};

	template<typename Type>
	struct FTypeAndGate {
		Type State;
		// for now it is the responsibility of the system that loops thorugh state 
		// to set CanBeRemoved and CanBeExecuted to false
		bool CanBeRemoved{ true }; 
		bool CalledRemove{ false }; // not needed for now

		//not needed in any system for now
		bool CanBeExecuted{ true };
		bool CalledExecution{ false };
	};

	template<typename Type>
	struct FGatedState {
		TActivatableFlatMap<std::string, FTypeAndGate<Type>> EventName_To_State;
		UEntityHandle StateControllerEntityHandle;
	};

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	class UStateControllerGraph final : public IStateController {

	public:
		UStateControllerGraph(URegistry& registry, const UEntityHandle& entityHandle);
		UStateControllerGraph(const IStateController* other);

		bool AddState(const std::string& eventName, const Type& value);
		void RemoveState(const std::string& eventName);

		// returns false if you haven't added state yet for either state
		//canExecute is from left to right , canRemove is from right to left
		bool LinkStates(const std::string& eventName, const std::string& nextEventName, ELinkingAction linking);
		void RemoveStateLink(const std::string& eventName, const std::string& nextEventName, ELinkingAction linking); // should add removeEventRemovalTriggers (but not important for now)
		bool LinkWithAll(const std::string& eventName, ELinkingAction linking);

		const UEntityHandle& GetEntityHandle() const override;
		bool IsValid() const override;

		EEventState EventState(const std::string& eventOrActionName)const override;

		bool TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger) override;

		//the eventName has to be an event with state (no nested ifs allowed for now) aka be direct with the boolean logic
		//here you are assuming that the execution triggers will be on the execution of the other events
		bool AddConditionsToExecute(const std::string& eventName, const FEventTriggerCondition& eventExecutionTriggers);//one group is like && one more function cal is like ||
		bool AddConditionsToRemove(const std::string& eventName, const FEventTriggerCondition& eventRemovalTriggers);

		virtual const TActivatableFlatMap<std::string, EEventState>& GetConstData()const override;

	private:
		
		void TriggerEvent(const std::string& eventName, EEventStateToTrigger eventStateToTrigger);
		// Basically lets try with checking if 
		bool IsStateActive(const std::string& actionName); //checks only for active states
		bool StateContains(const std::string& actionName);

		bool EventCanExecute(const std::string& eventName); //not USED
		bool EventCanBeRemoved(const std::string& eventName); //not USED

		bool ExplicitlyTriggeredEvent_CanExecute(const std::string& eventName);
		bool EventTriggeredByConditions_CanExecute(const std::string& eventName);

		bool ExplicitlyTriggeredEvent_CanBeRemoved(const std::string& eventName);
		bool EventTriggeredByConditions_CanBeRemoved(const std::string& eventName);

		void ExecuteCurrentEvent(const std::string& eventName); 
		void RemoveCurrentEvent(const std::string& eventName);

		//constexpr bool m_AllowsMultipleStatesAtTheSameTime{ allowsMultipleStatesAtTheSameTime };
		//constexpr bool m_AllowsNoState{ allowsNoState };
		URegistry* m_Registry{ nullptr };
		UEntityHandle m_EntityHandle;

		TActivatableFlatMap<std::string, EEventState> m_EventName_To_ActiveState;// for now lets just flip the bool and not actually move them
		std::unordered_map<std::string, FEventExecutionAndRemovalInfo> m_EventName_To_EventExecutionAndRemovalInfo;
		//when the events end their active state, you also have to check the events that were linked to that one in a NOT fashion

		std::unordered_map<std::string, std::unordered_set<std::string>> m_EventName_To_NextEventNames;

		//Trigger call operation -> too big to allocate and deallocate in a local function
		std::queue<FEventNameAndEventStateToTrigger> m_EventsToRemoveOrExecute{};
		std::unordered_set<std::string> m_EventsAlreadyCheckedThisTriggerCall;
	};

#pragma region State Controller Graph Implementation

#pragma region Constructor

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::UStateControllerGraph(URegistry& registry, const UEntityHandle& entityHandle) :
		m_Registry{ &registry },
		m_EntityHandle{ entityHandle } 
	{
		FGatedState<Type> state{};
		state.StateControllerEntityHandle = m_EntityHandle;
		m_Registry->AddComponent<FGatedState<Type>>(m_EntityHandle, state);
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::UStateControllerGraph(const IStateController* other) :
		UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>((UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>&)* other) {}

#pragma endregion

#pragma region State Addition and Removal

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::AddState(const std::string& eventName, const Type& value)
	{
		if (IsValid() && !m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			m_EventName_To_EventExecutionAndRemovalInfo.emplace(std::pair(eventName, FEventExecutionAndRemovalInfo{}));
			EEventState activationState{ EEventState::Inactive };
			m_EventName_To_ActiveState.Push(eventName, activationState, false);

			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

			if (!state->EventName_To_State.Contains(eventName))
			{
				state->EventName_To_State.Push(eventName, FTypeAndGate<Type>{value, true}, false);
			}

			m_EventName_To_NextEventNames.emplace(std::pair(eventName, std::unordered_set<std::string>{}));

			return true;
		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::RemoveState(const std::string& eventName)
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

			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };
			state->EventName_To_State.Erase(eventName);
		}

		if (m_EventName_To_NextEventNames.contains(eventName))
		{
			m_EventName_To_NextEventNames.erase(eventName);
		}
	}

#pragma endregion

#pragma region AddNextEvent

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::LinkStates(const std::string& eventName, const std::string& nextEventName, ELinkingAction linking)
	{
		if (IsValid())
		{

			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

			if (state->EventName_To_State.Contains(eventName) && state->EventName_To_State.Contains(nextEventName))
			{
				if (linking == ELinkingAction::OneWay)
				{
					m_EventName_To_NextEventNames.at(eventName).emplace(nextEventName);
				}
				else if (linking == ELinkingAction::BothWays)
				{
					m_EventName_To_NextEventNames.at(eventName).emplace(nextEventName);
					m_EventName_To_NextEventNames.at(nextEventName).emplace(eventName);
				}

				return true;
			}
		}

		return false;
	}
	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::RemoveStateLink(const std::string& eventName, const std::string& nextEventName, ELinkingAction linkingAction)
	{
		
		if (m_EventName_To_NextEventNames.contains(eventName))
		{
			m_EventName_To_NextEventNames.at(eventName).erase(nextEventName);
		}
		if (linkingAction == ELinkingAction::BothWays)
		{
			if (m_EventName_To_NextEventNames.contains(nextEventName))
			{
				m_EventName_To_NextEventNames.at(nextEventName).erase(eventName);
			}
		}
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::LinkWithAll(const std::string& eventName, ELinkingAction linking)
	{
		if (IsValid())
		{

			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

			if (state->EventName_To_State.Contains(eventName))
			{
				if (linking == ELinkingAction::OneWay)
				{
					for (auto It = m_EventName_To_NextEventNames.cbegin(); It != m_EventName_To_NextEventNames.cend(); It++)
					{
						m_EventName_To_NextEventNames.at(eventName).emplace(It->first);
					}
					
				}
				else if (linking == ELinkingAction::BothWays)
				{
					for (auto It = m_EventName_To_NextEventNames.cbegin(); It != m_EventName_To_NextEventNames.cend(); It++)
					{
						m_EventName_To_NextEventNames.at(eventName).emplace(It->first);
						m_EventName_To_NextEventNames.at(It->first).emplace(eventName);
					}
					
				}

				return true;
			}
		}

		return false;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline void UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExecuteCurrentEvent(const std::string& eventName)
	{
		if constexpr (allowsMultipleStatesAtTheSameTime) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };
				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.ValueAt(eventName).CalledExecution = true;

					for (size_t activatedEventIdx = 0; activatedEventIdx < state->EventName_To_State.ActivatedSize(); activatedEventIdx++)
					{
						if (state->EventName_To_State.At(activatedEventIdx).CanBeRemoved &&
							m_EventName_To_NextEventNames.at(state->EventName_To_State.ConstKeyAt(activatedEventIdx)).contains(eventName))
						{
							state->EventName_To_State.Activate(eventName);
							m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active;
							break;
						}
					}

					if (state->EventName_To_State.ActivatedSize() == 0)
					{
						state->EventName_To_State.Activate(eventName);
						m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active;
						
					}
				}
				else
				{
					m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active;
				}

				
			}
		}
		else if constexpr (!allowsMultipleStatesAtTheSameTime) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.ValueAt(eventName).CalledExecution = true;

					if (state->EventName_To_State.ActivatedSize() == 0 ||
						(state->EventName_To_State.At(0).CanBeRemoved &&
						m_EventName_To_NextEventNames.at(state->EventName_To_State.ConstKeyAt(0)).contains(eventName)))
					{
						size_t zero{ 0 };
						m_EventName_To_ActiveState.ValueAt(state->EventName_To_State.ConstKeyAt(zero)) = EEventState::Inactive;
						state->EventName_To_State.DeactivateAt(zero);
						state->EventName_To_State.Activate(eventName);
						m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active; // if state only activate in these conditions
					}
					else if (!state->EventName_To_State.At(0).CanBeRemoved &&
						m_EventName_To_NextEventNames.at(state->EventName_To_State.ConstKeyAt(0)).contains(eventName))
					{
						state->EventName_To_State.Activate(eventName);
						m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active; // if state only activate in these conditions
					}

				}
				else
				{
					m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Active; // if event activate
				}
			}
		}
	}

#pragma endregion

#pragma endregion


#pragma region Entity Handle

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline const UEntityHandle& UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState >::GetEntityHandle() const
	{
		return m_EntityHandle;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::IsValid() const
	{
		return m_EntityHandle.IsValid() && m_Registry;
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline EEventState UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventState(const std::string& eventOrActionName) const
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
	inline const TActivatableFlatMap<std::string, EEventState>& UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::GetConstData() const
	{
		return m_EventName_To_ActiveState;
	}

#pragma endregion

#pragma region Event Execution

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_EntityHandle.IsValid())
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

						triggered = !IsStateActive(m_EventsToRemoveOrExecute.front().EventName) &&
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

						triggered = IsStateActive(m_EventsToRemoveOrExecute.front().EventName) &&
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
							if (!IsStateActive((*It)) &&
								EventTriggeredByConditions_CanExecute((*It))) // I think this can still infinite loop -> maybe do it so it can only loop once or twice (hasn't yet)
							{
								m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ (*It) , EEventStateToTrigger::ConditionalExecution });
							}
						}

						auto eventsToRemove_begin{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cbegin() };
						auto eventsToRemove_end{ m_EventName_To_EventExecutionAndRemovalInfo.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cend() };

						for (auto It = eventsToRemove_begin; It != eventsToRemove_end; It++)
						{

							if (IsStateActive((*It)) &&
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::AddConditionsToExecute(const std::string& eventName, const FEventTriggerCondition& eventExecutionTriggers)
	{
		if (IsValid())
		{
			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::AddConditionsToRemove(const std::string& eventName, const FEventTriggerCondition& eventRemovalTriggers)
	{
		if (IsValid())
		{
			FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

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
	inline void UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::TriggerEvent(const std::string& eventName, EEventStateToTrigger eventStateToTrigger)
	{
		
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::IsStateActive(const std::string& eventName)
	{
		FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };
		return state->EventName_To_State.IsActivated(eventName);
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::StateContains(const std::string& actionName)
	{
		FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };
		return state->EventName_To_State.Contains(actionName);
	}

	template<typename Type, bool allowsMultipleStatesAtTheSameTime, bool allowsNoState>
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventCanExecute(const std::string& eventName)
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventCanBeRemoved(const std::string& eventName)
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExplicitlyTriggeredEvent_CanExecute(const std::string& eventName)
	{
		if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
		{
			if (m_EventName_To_EventExecutionAndRemovalInfo.at(eventName).ConditionsToTriggerExecution.empty())
			{
				return true;
			}

			bool noGates{ true };
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventTriggeredByConditions_CanExecute(const std::string& eventName)
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::ExplicitlyTriggeredEvent_CanBeRemoved(const std::string& eventName)
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
	inline bool UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::EventTriggeredByConditions_CanBeRemoved(const std::string& eventName)
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
	inline void UStateControllerGraph<Type, allowsMultipleStatesAtTheSameTime, allowsNoState>::RemoveCurrentEvent(const std::string& eventName)
	{
		if constexpr (allowsNoState) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };
				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.ValueAt(eventName).CalledRemove = true;

					state->EventName_To_State.Deactivate(eventName);
				}

				m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Inactive;
			}
		}
		else if constexpr (!allowsNoState) {
			if (m_EventName_To_EventExecutionAndRemovalInfo.contains(eventName))
			{
				FGatedState<Type>* state{ m_Registry->GetComponent<FGatedState<Type>>(m_EntityHandle.GetConstEntity()) };

				if (state->EventName_To_State.Contains(eventName))
				{
					state->EventName_To_State.ValueAt(eventName).CalledRemove = true;

					if (state->EventName_To_State.ActivatedSize() > 1)
					{
						state->EventName_To_State.Deactivate(eventName);
						
					}
				}
				
				m_EventName_To_ActiveState.ValueAt(eventName) = EEventState::Inactive;
			}
		}
	}

#pragma endregion

#pragma endregion

}