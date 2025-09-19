#pragma once

#include <string>

#include "../../../include/Core/URegistry.h"

namespace Pillar {


	/*
	
	- Individual Events -> Happen per entity
	- Global events -> things that happen in general (input and such)

	- EventListenerRegistry with unordered_map<EventListenerName, EntityHandleContainer>
		EventListenerRegistry.Trigger(EventListenerName, EntityHandle)
		- UEventListeners System(URegistry& registry)
		RegistrySingleton<RegistryCurrentActions>
		- UEventListener(URegistry, Name)
		RegistrySingleton<EventListenerRegistry> EventListenerRegistry;
		EventListenerRegistry.GetEntities(MyName) -> and then you loop through each
	
	*/

	enum class EEventManagerType {
		EventManager,
		StateSystem
	};

	class UEventRegistry final{

	public:
		UEventRegistry() {}

		void TriggerGlobalEvent(const std::string& eventName, URegistry& registry);

		void AddEvent(const std::string& eventName);
		void AddEventListener(const std::string& eventListenerName);

		void RemoveEvent(const std::string& eventName);
		void RemoveEventListener(const std::string& eventListenerName);

		bool ContainsEvent(const std::string& eventName);
		bool ContainsEventListener(const std::string& eventListenerName);

		void SubscribeEntityHandleToEvent(const std::string& eventName, UEntityHandle& entityHandle);
		void UnsubscribeEntityHandleToEvent(const std::string& eventName, UEntityHandle& entityHandle);

		void SubscribeEntityHandleToEventListener(const std::string& eventListenerName, UEntityHandle& entityHandle); 
		void UnsubscribeEntityHandleToEventListener(const std::string& eventListenerName, UEntityHandle& entityHandle);

		const UEntityHandleContainer* const GetConstSubscribedEntityHandles(const std::string& eventListenerName)const;
		UEntityHandleContainer* const GetSubscribedEntityHandles(const std::string& eventListenerName);
		void ClearSubscribedEntityHandles(const std::string& eventListenerName);

		void ClearEntities();
		
	private:
		std::unordered_map<std::string, UEntityHandleContainer> m_EventListenerName_To_EntityHandleContainer;
		std::unordered_map<std::string, UEntityHandleContainer> m_EventNames_To_EntityHandleMapOfEventManagerTypes;
	};

}