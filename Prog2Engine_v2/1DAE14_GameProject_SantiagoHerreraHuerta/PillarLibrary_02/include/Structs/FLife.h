#pragma once
#include "../../include/Structs/FVector.h"
#include "../../include/Core/URegistry.h"
#include "../../include/Structs/FState.h"

namespace Pillar {

	struct FHealthImmunityState {
		bool CurrentlyImmune{false}; 
		EStateRemovalType StateRemovalType;
	};

	struct FHealth {
		explicit FHealth(int maxHealth, float woundDuration = 1.f, float deathDuration = 0.f) :
			MaxHealth(maxHealth), 
			CurrentHealth(maxHealth), 
			WoundDuration(woundDuration),
			DeathDuration(deathDuration){}

		int MaxHealth{3};
		int CurrentHealth{3};
		UEntityHandleContainer EntitiesBeingAttackedBy;//tiene las mismas entities hasta que vuelve a ser atacado por otra entity

		float WoundDuration{1};
		float CurrentWoundTime{ 0 };
		bool Wounded{ false };

		float DeathDuration{ 0 };
		float CurrentDeathTime{ 0 };

		bool CurrentlyImmune{ false };
	};

	struct FAttack {
		int Damage{1};
		bool AttackedThisTick{ false };
	};

	enum class EEntityActivationAction {
		Activate,
		Deactivate
	};

	struct FActivationState {
		float TimeUntilEntityActivationAction;
		float CurrentTime;
		EEntityActivationAction EntityActivationAction{ EEntityActivationAction::Activate};
		EStateRemovalType StateRemovalType{ EStateRemovalType::Local };
	};

	struct FWealth {
		int Amount{0};
	};

	enum class ETextureType {
		Font,
		Image
	}; 
	enum class EPlayerUIType {
		Wealth,
		Health
	};
}