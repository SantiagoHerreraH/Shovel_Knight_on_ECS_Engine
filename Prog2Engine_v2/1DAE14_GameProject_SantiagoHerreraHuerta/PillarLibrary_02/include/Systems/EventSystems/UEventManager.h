#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <stack>

#include "../../../include/Systems/TemplateSystems/TUnorderedVector.h"
#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Systems/EventSystems/UEventRegistry.h"
#include "../../../include/Structs/FInput.h"

namespace Pillar {

	/*
	
		DEPRECATED
	
	*/

	struct FEventListeners {

		TUnorderedVector<std::string> EventListenersToReact;
		TUnorderedVector<std::string> EventsToReact;
	};

	/*
		This is a Component
	*/

	class UEventManager final{

	public:
		UEventManager(URegistry& registry);

		void SetEntityHandle(UEntityHandle& entityHandle);
		const UEntityHandle& GetEntityHandle() const;
		bool IsValid()const;

		void AddInput(SDL_Scancode input);
		void RemoveInput(SDL_Scancode input);

		void BindEventExecutionToInput(const std::string& eventName, SDL_Scancode input, EInputState inputState);
		void UnbindEventExecutionFromInput(const std::string& eventName, SDL_Scancode input, EInputState inputState);

		void AddEvent(const std::string& eventName);
		void RemoveEvent(const std::string& eventName);

		void AddReactingEvent(const std::string& eventNameThatIsGoingToTrigger, const std::string& eventThatIsGoingToReact);
		void RemoveReactingEvent(const std::string& eventNameThatIsGoingToTrigger, const std::string& eventThatIsGoingToReact);

		void AddEventListener(const std::string& eventName, const std::string& eventListenerName);
		void RemoveEventListener(const std::string& eventName, const std::string& eventListenerName);

		void TriggerEvent(const std::string& eventName);

	protected:

		URegistry* m_Registry{nullptr};

		URegistrySingleton<UEventRegistry> m_EventRegistry;

		UEntityHandle m_EntityHandle;

		std::unordered_map<std::string, FEventListeners> m_EventName_To_EventListeners;

		std::unordered_set<std::string> m_EventsAlreadyTriggeredInOneEventCall;
		std::stack<std::string> m_EventsToBeCalled;
	
	};

	/*
	
		This is the base class for the event listener systems
	
	*/

	using DEventListenerName = std::string;
	using DEventName = std::string;

	class UEventListener {

	public:
		UEventListener(URegistry& registry, const DEventListenerName& name);
		virtual ~UEventListener() {}
		const DEventListenerName& GetConstName() const;

	protected:
		UEntityHandleContainer* const GetSubscribedEntityHandles();
		const UEntityHandleContainer* const GetConstSubscribedEntityHandles()const;
		void ClearSubscribedEntities();
		void UnsubscribeEntityHandle(UEntityHandle& entityHandle);

		URegistry& m_Registry;

	private:

		URegistrySingleton<UEventRegistry> m_EventRegistry;
		DEventListenerName m_Name;
	};

}