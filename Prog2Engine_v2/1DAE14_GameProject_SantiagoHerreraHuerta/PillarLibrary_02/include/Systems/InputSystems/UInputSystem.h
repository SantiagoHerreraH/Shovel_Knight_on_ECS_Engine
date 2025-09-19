#pragma once

#include <vector>

#include "../../../include/Structs/FInput.h"
#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Systems//EventSystems/UEventRegistry.h"
#include "../../../include/Structs/FShape.h"
#include "../../../include/Systems/SceneSystems/USceneGraph.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"

namespace Pillar {

	class UInputContainer final{
	public:
		bool Create(const std::string& eventName, const DKeys& keys, 
			EInputState inputStateForExecution = EInputState::Down, 
			EInputState inputStateForRemoval = EInputState::Up);

		bool Add(const std::string& eventName, const DKeys& keys);

		void Clear(const std::string& eventName);
		void Clear();
		const std::unordered_map<std::string, FKeysAndInputStates>& GetData()const;
	private:
		std::unordered_map<std::string, FKeysAndInputStates> m_Events_To_Input;
	};

	class SInput final{
	public:
		static SDL_Scancode FirstPermittedInput();
		static SDL_Scancode LastPermittedInput();
		static std::string InputToEvent(SDL_Scancode input, EInputState inputState);
		static void InputToKeyStateEvents(SDL_Scancode input, FKeyStateEvents& outKeyStateEvent);

	private:
		static void AddInputStateToString(EInputState inputState, std::string& stringToModify); // doesn't check for now
	};

	class UInputSystem {

	public:
		UInputSystem(URegistry& registry);

		
		void Create();
		void Update(float deltaSeconds);

	private:
		void AddInput(const FKeyStateEvents& keyEvents, SDL_Scancode key);

		URegistry& m_Registry;

		URegistrySingleton<UEventRegistry> m_EventRegistry;
		std::vector<FKeyState> m_KeyStates;

	};

	struct FMouseEvents {
		bool NextSceneOnRelease{ false };
		bool PreviousSceneOnRelease{ false };
	};

	class UMouseInputSystem {

	public:
		UMouseInputSystem(URegistry& registry);
		void CheckOnHover(float mouseX, float mouseY);
		void CheckOnMouseUp(float mouseX, float mouseY);

	private:
		USceneManagementSystem m_SceneManagementSystem;
		UComponentIterator<FRectCollider> m_ButtonColliders;
		UComponentIterator<FMouseEvents> m_MouseEvents;
		UComponentIterator<UStateMachine> m_StateMachine;
	};

	
}