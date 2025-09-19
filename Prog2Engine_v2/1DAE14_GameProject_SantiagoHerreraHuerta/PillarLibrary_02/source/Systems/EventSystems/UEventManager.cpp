#include "../../../include/Systems/EventSystems/UEventManager.h"
#include "../../../include/Systems/InputSystems/UInputSystem.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"

namespace Pillar {

	UEventManager::UEventManager(URegistry& registry) :
		m_Registry(&registry),
		m_EventRegistry(registry)
	{
	}
	
	void UEventManager::SetEntityHandle(UEntityHandle& entityHandle)
	{
		m_EntityHandle = entityHandle;
	}
	
	const UEntityHandle& UEventManager::GetEntityHandle() const
	{
		return m_EntityHandle;
	}
	
	bool UEventManager::IsValid() const
	{
		return m_EntityHandle.IsValid();
	}
	
	void UEventManager::AddInput(SDL_Scancode input) {

		FKeyStateEvents keyStateEvents{};

		SInput::InputToKeyStateEvents(input, keyStateEvents);

		AddEvent(keyStateEvents.EventName_OnKeyDown);
		AddEvent(keyStateEvents.EventName_OnKeyMaintain);
		AddEvent(keyStateEvents.EventName_OnKeyUp);
	}

	void UEventManager::RemoveInput(SDL_Scancode input) {

		FKeyStateEvents keyStateEvents{};

		SInput::InputToKeyStateEvents(input, keyStateEvents);

		RemoveEvent(keyStateEvents.EventName_OnKeyDown);
		RemoveEvent(keyStateEvents.EventName_OnKeyMaintain);
		RemoveEvent(keyStateEvents.EventName_OnKeyUp);
	}

	void UEventManager::BindEventExecutionToInput(const std::string& eventName, SDL_Scancode input, EInputState inputState) {

		AddReactingEvent(SInput::InputToEvent(input, inputState), eventName);
	}
	void UEventManager::UnbindEventExecutionFromInput(const std::string& eventName, SDL_Scancode input, EInputState inputState) {

		RemoveReactingEvent(SInput::InputToEvent(input, inputState), eventName);
	}

	void UEventManager::AddEvent(const std::string& eventName)
	{
		m_EventName_To_EventListeners.emplace(std::pair(eventName, FEventListeners{}));
		m_EventRegistry.Get().SubscribeEntityHandleToEvent(eventName, m_EntityHandle);
	}
	
	void UEventManager::RemoveEvent(const std::string& eventName)
	{
		m_EventName_To_EventListeners.erase(eventName);
	}

	void UEventManager::AddReactingEvent(const std::string& eventNameThatIsGoingToTrigger, const std::string& eventThatIsGoingToReact)
	{
		if (m_EventName_To_EventListeners.contains(eventNameThatIsGoingToTrigger) &&
			m_EventName_To_EventListeners.contains(eventThatIsGoingToReact))
		{
			m_EventName_To_EventListeners.at(eventNameThatIsGoingToTrigger).EventsToReact.Create(eventThatIsGoingToReact);
		}
	}

	void UEventManager::RemoveReactingEvent(const std::string& eventNameThatIsGoingToTrigger, const std::string& eventThatIsGoingToReact)
	{
		if (m_EventName_To_EventListeners.contains(eventNameThatIsGoingToTrigger) &&
			m_EventName_To_EventListeners.contains(eventThatIsGoingToReact))
		{
			m_EventName_To_EventListeners.at(eventNameThatIsGoingToTrigger).EventsToReact.Remove(eventThatIsGoingToReact);
		}
	}
	
	void UEventManager::AddEventListener(const std::string& eventName, const std::string& eventListenerName)
	{
		if (m_EventName_To_EventListeners.contains(eventName) && m_EntityHandle.IsValid())
		{
			m_EventName_To_EventListeners.at(eventName).EventListenersToReact.Create(eventListenerName);
		}
	}

	void UEventManager::RemoveEventListener(const std::string& eventName, const std::string& eventListenerName)
	{
		if (m_EventName_To_EventListeners.contains(eventName) && m_EntityHandle.IsValid())
		{
			m_EventName_To_EventListeners.at(eventName).EventListenersToReact.Remove(eventListenerName);
		}
	}
	
	void UEventManager::TriggerEvent(const std::string& eventName) 
	{
		if (m_EventName_To_EventListeners.contains(eventName))
		{
			std::string currentEvent{ eventName };
			m_EventsToBeCalled.push(currentEvent);
			
			do
			{
				if (!m_EventsToBeCalled.empty())
				{
					currentEvent = m_EventsToBeCalled.top(); //check if top does pop
					m_Registry->GetComponent<UStateMachine>(m_EntityHandle.GetConstEntity())->TriggerState(eventName, EEventStateToTrigger::ConditionalExecution);
					m_EventsAlreadyTriggeredInOneEventCall.emplace(currentEvent);
					m_EventsToBeCalled.pop();
				}

				for (size_t i = 0; i < m_EventName_To_EventListeners.at(currentEvent).EventListenersToReact.Size(); i++)
				{
					m_EventRegistry.Get().SubscribeEntityHandleToEventListener(m_EventName_To_EventListeners.at(currentEvent).EventListenersToReact.At(i), m_EntityHandle);
				}

				for (size_t i = 0; i < m_EventName_To_EventListeners.at(currentEvent).EventsToReact.Size(); i++)
				{
					if (!m_EventsAlreadyTriggeredInOneEventCall.contains(m_EventName_To_EventListeners.at(currentEvent).EventsToReact.At(i)))
					{
						m_EventsToBeCalled.push(m_EventName_To_EventListeners.at(currentEvent).EventsToReact.At(i));
					}
					
				}
				
			} 
			while (!m_EventsToBeCalled.empty());

			m_EventsAlreadyTriggeredInOneEventCall.clear();
		}
		
	}
	UEventListener::UEventListener(URegistry& registry, const DEventListenerName& name) :
		m_Registry(registry),
		m_EventRegistry(registry),
		m_Name(name)
	{
		m_EventRegistry.Get().AddEventListener(name);
	}
	const DEventListenerName& UEventListener::GetConstName() const
	{
		return m_Name;
	}
	UEntityHandleContainer* const UEventListener::GetSubscribedEntityHandles()
	{
		return m_EventRegistry.Get().GetSubscribedEntityHandles(m_Name);
	}
	const UEntityHandleContainer* const UEventListener::GetConstSubscribedEntityHandles() const
	{
		return m_EventRegistry.GetConst().GetConstSubscribedEntityHandles(m_Name);
	}
	void UEventListener::ClearSubscribedEntities()
	{
		m_EventRegistry.Get().ClearSubscribedEntityHandles(m_Name);
	}
	void UEventListener::UnsubscribeEntityHandle(UEntityHandle& entityHandle)
	{
		m_EventRegistry.Get().UnsubscribeEntityHandleToEventListener(m_Name, entityHandle);
	}
}

