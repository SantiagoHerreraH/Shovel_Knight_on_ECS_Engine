#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Systems/InputSystems/UInputSystem.h"

namespace Pillar {
	UStateMachine::UStateMachine(URegistry& registry, const UEntityHandle& handle) :
		m_EventRegistry(registry),
		m_Registry(&registry),
		m_EntityHandle(handle){}

	const UEntityHandle& UStateMachine::GetEntityHandle() const
	{
		return m_EntityHandle;
	}

	bool UStateMachine::IsValid() const
	{
		return m_EntityHandle.IsValid();
	}

	EEventState UStateMachine::EventState(const std::string& eventOrActionName)const
	{
		if (!m_StateName_To_ReactingInformation.contains(eventOrActionName))
		{
			return EEventState::Null;
		}
		return m_StateName_To_ReactingInformation.at(eventOrActionName).CurrentGeneralStateOfEvent;
	}

	void UStateMachine::AddState(const std::string& stateName)
	{
		if (!m_StateName_To_ReactingInformation.contains(stateName))
		{
			m_StateName_To_ReactingInformation.emplace(std::pair(stateName, FEventReactingInformation{}));
		}
	}

	void UStateMachine::Implementation_TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_StateName_To_ReactingInformation.contains(eventName))
		{
			auto begin = m_StateName_To_ReactingInformation.at(eventName).StateControllerTypesWithEvent.begin();
			auto end = m_StateName_To_ReactingInformation.at(eventName).StateControllerTypesWithEvent.end();

			for (auto It = begin; It != end; It++)
			{
				m_Type_To_StateControllers.at((*It)).get()->TriggerState(eventName, eventStateToTrigger);
			}

		}
	}


	void UStateMachine::BindEventToInput(const std::string& eventName, EEventStateToTrigger eventStateToTrigger, SDL_Scancode input, EInputState inputState) {
		
		if (IsValid() && inputState != EInputState::None)
		{
			std::string inputEvent{ SInput::InputToEvent(input, inputState) };

			m_EventRegistry.Get().SubscribeEntityHandleToEvent(inputEvent, m_EntityHandle);

			AddReactingState(inputEvent, EEventStateToTrigger::ConditionalExecution, eventName, eventStateToTrigger);

		}
	}

	void UStateMachine::UnbindEventFromInput(const std::string& eventName, EEventStateToTrigger eventStateToTrigger, SDL_Scancode input, EInputState inputState) {
		
		if (IsValid() && inputState != EInputState::None)
		{
			std::string inputEvent{ SInput::InputToEvent(input, inputState) };

			m_EventRegistry.Get().UnsubscribeEntityHandleToEvent(inputEvent, m_EntityHandle);

			RemoveReactingState(inputEvent, EEventStateToTrigger::ConditionalExecution, eventName, eventStateToTrigger);
		}
		
	}

	bool UStateMachine::AddReactingState(const std::string& eventNameThatIsGoingToTrigger, EEventStateToTrigger eventStateToReactTo, const std::string& eventThatIsGoingToReact, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_StateName_To_ReactingInformation.contains(eventThatIsGoingToReact))
		{
			if (!m_StateName_To_ReactingInformation.contains(eventNameThatIsGoingToTrigger))
			{
				m_StateName_To_ReactingInformation.emplace(std::pair(eventNameThatIsGoingToTrigger, FEventReactingInformation{}));
			}

			switch (eventStateToReactTo)
			{
			case Pillar::EEventStateToTrigger::ConditionalExecution:
			case Pillar::EEventStateToTrigger::ForcedExecution:
				m_StateName_To_ReactingInformation.at(eventNameThatIsGoingToTrigger).EventsToReactOnExecution.Push(eventThatIsGoingToReact, eventStateToTrigger);
				break;
			case Pillar::EEventStateToTrigger::ConditionalRemoval:
			case Pillar::EEventStateToTrigger::ForcedRemoval:
				m_StateName_To_ReactingInformation.at(eventNameThatIsGoingToTrigger).EventsToReactOnRemoval.Push(eventThatIsGoingToReact, eventStateToTrigger);
				break;
			}
			
			return true;
		}

		return false;
	}

	void UStateMachine::RemoveReactingState(const std::string& eventNameThatIsGoingToTrigger, EEventStateToTrigger eventStateToReactTo, const std::string& eventThatIsGoingToReact, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_StateName_To_ReactingInformation.contains(eventNameThatIsGoingToTrigger) &&
			m_StateName_To_ReactingInformation.contains(eventThatIsGoingToReact))
		{
			switch (eventStateToReactTo)
			{
			case Pillar::EEventStateToTrigger::ConditionalExecution:
				m_StateName_To_ReactingInformation.at(eventNameThatIsGoingToTrigger).EventsToReactOnExecution.Erase(eventThatIsGoingToReact);
				break;
			case Pillar::EEventStateToTrigger::ConditionalRemoval:
				m_StateName_To_ReactingInformation.at(eventNameThatIsGoingToTrigger).EventsToReactOnRemoval.Erase(eventThatIsGoingToReact);
				break;
			}
			
		}
	}

	/*
	
		NEW AI FUNCTIONS
	
	*/

	bool UStateMachine::AddConditionsToExecute(const std::string& eventOrActionToExecute, const FEventTriggerCondition& condition)
	{
		if (IsValid() && m_StateName_To_ReactingInformation.contains(eventOrActionToExecute))
		{
			m_StateName_To_ReactingInformation.at(eventOrActionToExecute).ConditionsToTriggerExecution.push_back(condition);

			const std::string* currentEventName{ nullptr };

			for (size_t eventIdx = 0; eventIdx < condition.EventName_To_ActiveState.Size(); eventIdx++)
			{
				currentEventName = &condition.EventName_To_ActiveState.ConstKeyAt(eventIdx);
				if (!m_StateName_To_ReactingInformation.contains(*currentEventName))
				{
					m_StateName_To_ReactingInformation.emplace(
						std::pair(
							*currentEventName,
							FEventReactingInformation{}));
				}

				m_StateName_To_ReactingInformation.at(*currentEventName).EventsToParticipateInExecution.emplace(eventOrActionToExecute);
			}

			return true;
		}

		return false;
	}

	bool UStateMachine::AddConditionsToRemove(const std::string& eventOrActionToRemove, const FEventTriggerCondition& condition)
	{
		if (IsValid() && m_StateName_To_ReactingInformation.contains(eventOrActionToRemove))
		{
			m_StateName_To_ReactingInformation.at(eventOrActionToRemove).ConditionsToTriggerRemoval.push_back(condition);

			const std::string* currentEventName{ nullptr };

			for (size_t eventIdx = 0; eventIdx < condition.EventName_To_ActiveState.Size(); eventIdx++)
			{
				currentEventName = &condition.EventName_To_ActiveState.ConstKeyAt(eventIdx);
				if (!m_StateName_To_ReactingInformation.contains(*currentEventName))
				{
					m_StateName_To_ReactingInformation.emplace(
						std::pair(
							*currentEventName,
							FEventReactingInformation{}));
				}

				m_StateName_To_ReactingInformation.at(*currentEventName).EventsToParticipateInRemoval.emplace(eventOrActionToRemove);
			}
			return true;
		}

		return false;
	}

	void UStateMachine::AddConditionsToExecute(const std::string& eventOrActionToExecute, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType)
	{
		m_ConditionsToExecute.AddCondition(eventOrActionToExecute, condition, eventTriggerConditionType);
	}

	void UStateMachine::AddConditionsToRemove(const std::string& eventOrActionToRemove, const UStateCondition& condition, EEventTriggerConditionType eventTriggerConditionType)
	{
		m_ConditionsToRemove.AddCondition(eventOrActionToRemove, condition, eventTriggerConditionType);
	}

	void UStateMachine::CheckStateTriggersOfOtherStateMachines()
	{
		for (size_t i = 0; i < m_ConditionsToExecute.NumOfConditions(); i++)
		{
			if (m_ConditionsToExecute.TriggerConditionsAreFulfilled(i))
			{
				TriggerState(
					m_ConditionsToExecute.StateNameAt(i),
					EEventStateToTrigger::ConditionalExecution);
			}
		}

		for (size_t i = 0; i < m_ConditionsToRemove.NumOfConditions(); i++)
		{
			if (m_ConditionsToRemove.TriggerConditionsAreFulfilled(i))
			{
				TriggerState(
					m_ConditionsToRemove.StateNameAt(i),
					EEventStateToTrigger::ConditionalRemoval);
			}
		}
	}

	bool UStateMachine::GateConditions_CanExecute_Internal(const std::string& eventName)
	{
		if (m_StateName_To_ReactingInformation.contains(eventName))
		{
			if (m_StateName_To_ReactingInformation.at(eventName).ConditionsToTriggerExecution.empty())
			{
				return true;
			}

			bool noGates{ true };
			FEventTriggerCondition* eventTriggerCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_StateName_To_ReactingInformation.at(eventName).ConditionsToTriggerExecution.size(); condIdx++)
			{
				eventTriggerCondition = &m_StateName_To_ReactingInformation.at(eventName).ConditionsToTriggerExecution.at(condIdx);

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

					if ((bool)m_StateName_To_ReactingInformation.at(*currentEvent).CurrentGeneralStateOfEvent != currentActiveState)// this should be fine, make sure to set everything correctly
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

	bool UStateMachine::GateConditions_CanBeRemoved_Internal(const std::string& eventOrActionName)
	{
		if (m_StateName_To_ReactingInformation.contains(eventOrActionName))
		{
			if (m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.empty())
			{
				return true;
			}

			bool noGates{ true };
			FEventTriggerCondition* eventTriggerRemovalCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.size(); condIdx++)
			{
				eventTriggerRemovalCondition = &m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.at(condIdx);

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

					if ((bool)m_StateName_To_ReactingInformation.at(*currentEvent).CurrentGeneralStateOfEvent != currentActiveState)// this should be fine, make sure to set everything correctly
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

	bool UStateMachine::TriggerConditions_CanExecute_Internal(const std::string& eventOrActionName)
	{
		if (m_StateName_To_ReactingInformation.contains(eventOrActionName))
		{
			if (m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerExecution.empty())
			{
				return true;
			}

			FEventTriggerCondition* eventTriggerCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerExecution.size(); condIdx++)
			{
				eventTriggerCondition = &m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerExecution.at(condIdx);

				for (size_t eventIdx = 0; eventIdx < eventTriggerCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerCondition->EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
					{
						break;
					}

					currentActiveState = (bool)eventTriggerCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_StateName_To_ReactingInformation.at(*currentEvent).CurrentGeneralStateOfEvent != currentActiveState)// this should be fine, make sure to set everything correctly
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

	bool UStateMachine::TriggerConditions_CanBeRemoved_Internal(const std::string& eventOrActionName)
	{
		if (m_StateName_To_ReactingInformation.contains(eventOrActionName))
		{
			if (m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.empty())
			{
				return true;
			}

			FEventTriggerCondition* eventTriggerRemovalCondition{ nullptr };
			const std::string* currentEvent{ nullptr };
			bool currentActiveState{ false };

			for (size_t condIdx = 0; condIdx < m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.size(); condIdx++)
			{
				eventTriggerRemovalCondition = &m_StateName_To_ReactingInformation.at(eventOrActionName).ConditionsToTriggerRemoval.at(condIdx);

				for (size_t eventIdx = 0; eventIdx < eventTriggerRemovalCondition->EventName_To_ActiveState.Size(); eventIdx++)
				{
					if (eventTriggerRemovalCondition->EventTriggerConditionType == EEventTriggerConditionType::OnlyGate)
					{
						break;
					}

					currentActiveState = (bool)eventTriggerRemovalCondition->EventName_To_ActiveState.At(eventIdx);
					currentEvent = &eventTriggerRemovalCondition->EventName_To_ActiveState.ConstKeyAt(eventIdx);

					if ((bool)m_StateName_To_ReactingInformation.at(*currentEvent).CurrentGeneralStateOfEvent != currentActiveState)// this should be fine, make sure to set everything correctly
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

	void UStateMachine::PushEventsToReactOnExecution(const std::string& eventOrActionNameExecuting)
	{
		for (size_t i = 0; i < m_StateName_To_ReactingInformation.at(eventOrActionNameExecuting).EventsToReactOnExecution.Size(); i++)
		{
			m_EventsToRemoveOrExecute.push(
				FEventNameAndEventStateToTrigger{
					m_StateName_To_ReactingInformation.at(eventOrActionNameExecuting).EventsToReactOnExecution.ConstKeyAt(i),
					m_StateName_To_ReactingInformation.at(eventOrActionNameExecuting).EventsToReactOnExecution.At(i) });
		}
	}

	void UStateMachine::PushEventsToReactOnRemoval(const std::string& eventOrActionNameRemoval)
	{
		for (size_t i = 0; i < m_StateName_To_ReactingInformation.at(eventOrActionNameRemoval).EventsToReactOnRemoval.Size(); i++)
		{
			m_EventsToRemoveOrExecute.push(
				FEventNameAndEventStateToTrigger{
					m_StateName_To_ReactingInformation.at(eventOrActionNameRemoval).EventsToReactOnRemoval.ConstKeyAt(i),
					m_StateName_To_ReactingInformation.at(eventOrActionNameRemoval).EventsToReactOnRemoval.At(i) });
		}
	}

	void UStateMachine::TriggerState(const std::string& eventName, EEventStateToTrigger eventStateToTrigger)
	{
		if (m_StateName_To_ReactingInformation.contains(eventName))
		{

			bool triggered{ false };

			m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ eventName, eventStateToTrigger });

			do
			{
				switch (m_EventsToRemoveOrExecute.front().EventStateToTrigger)
				{
				case EEventStateToTrigger::ConditionalExecution:

					triggered = EventState(m_EventsToRemoveOrExecute.front().EventName) != EEventState::Active &&
						(GateConditions_CanExecute_Internal(m_EventsToRemoveOrExecute.front().EventName) &&

							(!m_ConditionsToExecute.ConditionContainsState(m_EventsToRemoveOrExecute.front().EventName) ||
							m_ConditionsToExecute.GateConditionsAreFulfilled(m_EventsToRemoveOrExecute.front().EventName)));

					if (triggered)
					{
						Implementation_TriggerState(m_EventsToRemoveOrExecute.front().EventName, EEventStateToTrigger::ConditionalExecution);
						m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).CurrentGeneralStateOfEvent = EEventState::Active;
						PushEventsToReactOnExecution(m_EventsToRemoveOrExecute.front().EventName);
					}

					break;

				case EEventStateToTrigger::ForcedExecution:

					Implementation_TriggerState(m_EventsToRemoveOrExecute.front().EventName, EEventStateToTrigger::ForcedExecution);
					m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).CurrentGeneralStateOfEvent = EEventState::Active;
					PushEventsToReactOnExecution(m_EventsToRemoveOrExecute.front().EventName);

					triggered = true;

					break;

				case EEventStateToTrigger::ConditionalRemoval:

					triggered = EventState(m_EventsToRemoveOrExecute.front().EventName) == EEventState::Active &&
						(GateConditions_CanBeRemoved_Internal(m_EventsToRemoveOrExecute.front().EventName) &&

							(!m_ConditionsToExecute.ConditionContainsState(m_EventsToRemoveOrExecute.front().EventName) || 
							m_ConditionsToRemove.GateConditionsAreFulfilled(m_EventsToRemoveOrExecute.front().EventName)));

					if (triggered)
					{
						Implementation_TriggerState(m_EventsToRemoveOrExecute.front().EventName, EEventStateToTrigger::ConditionalRemoval);
						m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).CurrentGeneralStateOfEvent = EEventState::Inactive;
						PushEventsToReactOnRemoval(m_EventsToRemoveOrExecute.front().EventName);
					}

					break;

				case EEventStateToTrigger::ForcedRemoval:

					Implementation_TriggerState(m_EventsToRemoveOrExecute.front().EventName, EEventStateToTrigger::ForcedRemoval);
					m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).CurrentGeneralStateOfEvent = EEventState::Inactive;
					PushEventsToReactOnRemoval(m_EventsToRemoveOrExecute.front().EventName);

					triggered = true;

					break;
				}

				if (triggered)
				{
					auto eventsToExecute_begin{ m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInExecution.cbegin() };
					auto eventsToExecute_end{ m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInExecution.cend() };

					for (auto It = eventsToExecute_begin; It != eventsToExecute_end; It++)
					{
						// proactive event triggers only check here
						if (EventState((*It)) != EEventState::Active &&
							(TriggerConditions_CanExecute_Internal((*It)) ||
								m_ConditionsToExecute.TriggerConditionsAreFulfilled((*It)))) // I think this can still infinite loop -> maybe do it so it can only loop once or twice (hasn't yet)
						{
							m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ (*It) , EEventStateToTrigger::ConditionalExecution });
						}
					}

					auto eventsToRemove_begin{ m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cbegin() };
					auto eventsToRemove_end{ m_StateName_To_ReactingInformation.at(m_EventsToRemoveOrExecute.front().EventName).EventsToParticipateInRemoval.cend() };

					for (auto It = eventsToRemove_begin; It != eventsToRemove_end; It++)
					{
						if (EventState((*It)) == EEventState::Active &&
							(TriggerConditions_CanBeRemoved_Internal((*It)) ||
								m_ConditionsToRemove.TriggerConditionsAreFulfilled((*It))))
						{
							m_EventsToRemoveOrExecute.push(FEventNameAndEventStateToTrigger{ (*It) , EEventStateToTrigger::ConditionalRemoval });
						}
					}
				}

				m_EventsToRemoveOrExecute.pop();

			} while (!m_EventsToRemoveOrExecute.empty());
		}


	}

}