#pragma once

#include <vector>
#include <string>
#include "../../../../pch.h"

namespace Pillar {

	using DKeys = std::vector<SDL_Scancode>;

	struct FKeyStateEvents {
		std::string EventName_OnKeyUp;
		std::string EventName_OnKeyDown;
		std::string EventName_OnKeyMaintain;
	};

	enum class EInputState {
		Down,
		Up,
		Maintain,
		None
	};

	struct FKeysAndInputStates {
		EInputState InputStateForExecution;
		EInputState InputStateForRemoval;
		DKeys Keys;
	};

	struct FKeyState {

		SDL_Scancode Key;
		bool Pressed{ false };
		float TimePressed{ 0.f };
		FKeyStateEvents KeyEvents;
	};
}