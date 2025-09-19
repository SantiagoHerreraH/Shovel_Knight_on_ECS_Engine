#include "../../../include/Systems/InputSystems/UInputSystem.h"
#include "../../../include/Systems/CollisionSystems/SCollision.h"
#include "../../../include/Systems/EventSystems/SEventNames.h"

namespace Pillar {

	UInputSystem::UInputSystem(URegistry& registry) :
		m_Registry(registry),
		m_EventRegistry(registry)
	{
	}
	void UInputSystem::Create()
	{
		FKeyStateEvents currentKeyEvents{};

		for (int currentKey = SInput::FirstPermittedInput(); currentKey <= SInput::LastPermittedInput(); currentKey++)
		{
			SInput::InputToKeyStateEvents(SDL_Scancode(currentKey), currentKeyEvents);

			AddInput(currentKeyEvents, SDL_Scancode(currentKey));
		}
	}
	void UInputSystem::Update(float deltaSeconds)
	{
		//you can also check the key down and key up here
		const Uint8* pStates = SDL_GetKeyboardState(nullptr);

		for (size_t i = 0; i < m_KeyStates.size(); i++)
		{
			if (pStates[m_KeyStates[i].Key])
			{

				if (m_KeyStates[i].Key == SDL_Scancode::SDL_SCANCODE_LEFT)
				{
					bool yep = true;
				}

				if (!m_KeyStates[i].Pressed)
				{
   					m_EventRegistry.Get().TriggerGlobalEvent(m_KeyStates[i].KeyEvents.EventName_OnKeyDown, m_Registry);
				}
				else
				{
					m_EventRegistry.Get().TriggerGlobalEvent(m_KeyStates[i].KeyEvents.EventName_OnKeyMaintain, m_Registry);
				}

				m_KeyStates[i].Pressed = true;
				m_KeyStates[i].TimePressed += deltaSeconds;
				
			}
			else
			{
				if (m_KeyStates[i].Pressed)
				{
					m_EventRegistry.Get().TriggerGlobalEvent(m_KeyStates[i].KeyEvents.EventName_OnKeyUp, m_Registry);

				}

				m_KeyStates[i].Pressed = false;
				m_KeyStates[i].TimePressed = 0.f;
			}
		}
	}
	void UInputSystem::AddInput(const FKeyStateEvents& keyEvents, SDL_Scancode key)
	{
		m_EventRegistry.Get().AddEvent(keyEvents.EventName_OnKeyDown);
		m_EventRegistry.Get().AddEvent(keyEvents.EventName_OnKeyUp);
		m_EventRegistry.Get().AddEvent(keyEvents.EventName_OnKeyMaintain);

		FKeyState keyState{};
		keyState.KeyEvents = keyEvents;
		keyState.Key = key;
		m_KeyStates.push_back(keyState);
		
	}
	
	/*
	
		STATIC INPUT CLASS
	
	*/

	SDL_Scancode SInput::FirstPermittedInput()
	{
		return SDL_SCANCODE_A;
	}

	SDL_Scancode SInput::LastPermittedInput()
	{
		return SDL_SCANCODE_NUMLOCKCLEAR;
	}

	std::string SInput::InputToEvent(SDL_Scancode input, EInputState inputState)
	{
		std::string eventName{};
		
		//Range for now
		if (input >= FirstPermittedInput() && input <= LastPermittedInput())
		{
			eventName = input;
		}
		else
		{
			eventName = "INPUT HAS NO EVENT";
		}

		AddInputStateToString(inputState, eventName);

		return eventName;
	}

	void SInput::InputToKeyStateEvents(SDL_Scancode input, FKeyStateEvents& outKeyStateEvent)
	{
		outKeyStateEvent.EventName_OnKeyDown = InputToEvent(input, EInputState::Down);
		outKeyStateEvent.EventName_OnKeyMaintain = InputToEvent(input, EInputState::Maintain);
		outKeyStateEvent.EventName_OnKeyUp = InputToEvent(input, EInputState::Up);
	}

	void SInput::AddInputStateToString(EInputState inputState, std::string& stringToModify)
	{
		switch (inputState)
		{
		case EInputState::Down:
			stringToModify += + "_Down";
			break;
		case EInputState::Up:
			stringToModify += + "_Up";
			break;
		case EInputState::Maintain:
			stringToModify += "_Maintain";
			break;
		case EInputState::None:
			stringToModify += "_NONE";
			break;
		default:
			break;
		}
	}

	bool UInputContainer::Create(const std::string& eventName, const DKeys& keys, EInputState inputStateForExecution, EInputState inputStateForRemoval)
	{
		if (!m_Events_To_Input.contains(eventName))
		{
			FKeysAndInputStates keysAndInput{};
			keysAndInput.InputStateForExecution = inputStateForExecution;
			keysAndInput.InputStateForRemoval = inputStateForRemoval;
			keysAndInput.Keys = keys;
			m_Events_To_Input.emplace(std::pair(eventName, keysAndInput));

			return true;
		}
		
		return false;
	}

	bool UInputContainer::Add(const std::string& eventName, const DKeys& keys)
	{
		
		if (!m_Events_To_Input.contains(eventName))
		{
			return false;
		}

		size_t previousSize{ m_Events_To_Input.at(eventName).Keys.size() };
		m_Events_To_Input.at(eventName).Keys.reserve(previousSize + keys.size());

		for (size_t i = 0; i < keys.size(); i++)
		{
			m_Events_To_Input.at(eventName).Keys.emplace_back(keys.at(i));
		}

		return true;
	}

	void UInputContainer::Clear(const std::string& eventName)
	{
		if (m_Events_To_Input.contains(eventName))
		{
			m_Events_To_Input.at(eventName).Keys.clear();
		}
	}

	void UInputContainer::Clear()
	{
		for (auto It = m_Events_To_Input.begin(); It != m_Events_To_Input.end(); It++)
		{
			It->second.Keys.clear();
		}
	}

	const std::unordered_map<std::string, FKeysAndInputStates>& UInputContainer::GetData() const
	{
		return m_Events_To_Input;
	}

	UMouseInputSystem::UMouseInputSystem(URegistry& registry) :
		m_SceneManagementSystem(registry),
		m_StateMachine(UStateMachine{ registry, UEntityHandle{} })
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FRectCollider{}, FMouseEvents{}, UStateMachine{ registry, UEntityHandle{} });

		registry.GetComponents(m_ButtonColliders, filter);
		registry.GetComponents(m_MouseEvents, filter);
		registry.GetComponents(m_StateMachine, filter);
	}

	void UMouseInputSystem::CheckOnHover(float mouseX, float mouseY)
	{
		bool isPointInRect{};
		for (size_t archIdx = 0; archIdx < m_ButtonColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ButtonColliders.NumOfActivatedComponents(archIdx); compIdx++)
			{
				isPointInRect = SCollision::IsPointInRect(FPoint2f{ mouseX, mouseY }, m_ButtonColliders.At(archIdx, compIdx).CurrentRect);
				
				if (isPointInRect &&
					m_StateMachine.At(archIdx, compIdx).EventState(SEventNames::OnHover) != EEventState::Active)
				{
					m_StateMachine.At(archIdx, compIdx).TriggerState(SEventNames::OnHover, EEventStateToTrigger::ForcedExecution);
				}
				else if (!isPointInRect && 
					m_StateMachine.At(archIdx, compIdx).EventState(SEventNames::OnHover) != EEventState::Inactive)
				{
					m_StateMachine.At(archIdx, compIdx).TriggerState(SEventNames::OnHover, EEventStateToTrigger::ForcedRemoval);
				}
			}
		}
	}

	void UMouseInputSystem::CheckOnMouseUp(float mouseX, float mouseY)
	{
		for (size_t archIdx = 0; archIdx < m_ButtonColliders.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ButtonColliders.NumOfActivatedComponents(archIdx); compIdx++)
			{
				
				if (SCollision::IsPointInRect(FPoint2f{mouseX, mouseY}, m_ButtonColliders.At(archIdx, compIdx).CurrentRect))
				{
					if (m_MouseEvents.At(archIdx, compIdx).NextSceneOnRelease)
					{
						m_SceneManagementSystem.NextScene();
					}
					else if (m_MouseEvents.At(archIdx, compIdx).PreviousSceneOnRelease)
					{
						m_SceneManagementSystem.PreviousScene();
					}
				}
			}
		}
	}

}

