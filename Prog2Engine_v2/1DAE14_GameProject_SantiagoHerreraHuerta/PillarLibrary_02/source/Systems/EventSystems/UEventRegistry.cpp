#include "../../../include/Systems/EventSystems/UEventRegistry.h"
#include "../../../include/Systems//EventSystems/UEventManager.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"

namespace Pillar {
	
	void UEventRegistry::TriggerGlobalEvent(const std::string& eventName, URegistry& registry)
	{
		registry.Lock<UStateMachine>();

		for (size_t i = 0; i < m_EventNames_To_EntityHandleMapOfEventManagerTypes.at(eventName).NumOfEntities(); i++)
		{
			registry.GetComponent<UStateMachine>(m_EventNames_To_EntityHandleMapOfEventManagerTypes.at(eventName).EntityAt(i).GetConstEntity())->TriggerState(eventName, EEventStateToTrigger::ForcedExecution);
		}

		registry.Unlock<UStateMachine>();
	}
	void UEventRegistry::AddEvent(const std::string& eventName)
	{
		m_EventNames_To_EntityHandleMapOfEventManagerTypes.emplace(std::pair(eventName, UEntityHandleContainer{}));
	}
	void UEventRegistry::AddEventListener(const std::string& eventListenerName)
	{
		if (!m_EventListenerName_To_EntityHandleContainer.contains(eventListenerName))
		{
			m_EventListenerName_To_EntityHandleContainer.emplace(std::pair(eventListenerName, UEntityHandleContainer{}));
		}
		
	}

	void UEventRegistry::RemoveEvent(const std::string& eventName)
	{
		m_EventNames_To_EntityHandleMapOfEventManagerTypes.erase(eventName);
	}

	void UEventRegistry::RemoveEventListener(const std::string& eventListenerName)
	{
		m_EventListenerName_To_EntityHandleContainer.erase(eventListenerName);
	}

	bool UEventRegistry::ContainsEvent(const std::string& eventName)
	{
		return m_EventNames_To_EntityHandleMapOfEventManagerTypes.contains(eventName);
	}

	bool UEventRegistry::ContainsEventListener(const std::string& eventListenerName)
	{
		return m_EventListenerName_To_EntityHandleContainer.contains(eventListenerName);
	}

	void UEventRegistry::SubscribeEntityHandleToEvent(const std::string& eventName, UEntityHandle& entityHandle)
	{
		m_EventNames_To_EntityHandleMapOfEventManagerTypes.at(eventName).Add(entityHandle);
	}

	void UEventRegistry::UnsubscribeEntityHandleToEvent(const std::string& eventName, UEntityHandle& entityHandle)
	{
		m_EventNames_To_EntityHandleMapOfEventManagerTypes.at(eventName).SwapRemove(entityHandle);
	}

	void UEventRegistry::SubscribeEntityHandleToEventListener(const std::string& eventListenerName, UEntityHandle& entityHandle)
	{
		m_EventListenerName_To_EntityHandleContainer.at(eventListenerName).Add(entityHandle);
	}

	void UEventRegistry::UnsubscribeEntityHandleToEventListener(const std::string& eventListenerName, UEntityHandle& entityHandle)
	{
		m_EventListenerName_To_EntityHandleContainer.at(eventListenerName).SwapRemove(entityHandle);
	}

	void UEventRegistry::ClearEntities()
	{
		auto beginListeners = m_EventListenerName_To_EntityHandleContainer.begin();
		auto endListeners = m_EventListenerName_To_EntityHandleContainer.end();

		for (auto It = beginListeners; It != endListeners; It++)
		{
			It->second.Clear();
		}

		auto beginEvent = m_EventNames_To_EntityHandleMapOfEventManagerTypes.begin();
		auto endEvent = m_EventNames_To_EntityHandleMapOfEventManagerTypes.end();

		for (auto It = beginEvent; It != endEvent; It++)
		{
			It->second.Clear();
		}
	}

	const UEntityHandleContainer* const UEventRegistry::GetConstSubscribedEntityHandles(const std::string& eventListenerName) const
	{
		if (m_EventListenerName_To_EntityHandleContainer.contains(eventListenerName))
		{
			return &m_EventListenerName_To_EntityHandleContainer.at(eventListenerName);
		}

		return nullptr;
	}

	UEntityHandleContainer* const UEventRegistry::GetSubscribedEntityHandles(const std::string& eventListenerName)
	{
		if (m_EventListenerName_To_EntityHandleContainer.contains(eventListenerName))
		{
			return &m_EventListenerName_To_EntityHandleContainer.at(eventListenerName); //bug has to do with the way you unsi¡ubscribe entities each tick
		}

		return nullptr;
	}

	void UEventRegistry::ClearSubscribedEntityHandles(const std::string& eventListenerName)
	{
		if (m_EventListenerName_To_EntityHandleContainer.contains(eventListenerName))
		{
			m_EventListenerName_To_EntityHandleContainer.at(eventListenerName).Clear();
		}
	}



}

