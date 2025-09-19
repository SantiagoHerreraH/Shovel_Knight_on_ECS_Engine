#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FLife.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"

namespace Pillar {

	class UApplyDamage final {
	public:
		UApplyDamage(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FAttack> m_Attacks;
		UComponentIterator<FEntityResolveOverlapInfo> m_OverlapInfo;
	};

	class UUpdateWoundDuration {
	public:
		UUpdateWoundDuration(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		UComponentIterator<FHealth> m_Health;
	};

	class UInteractionSystem final{

	public:
		UInteractionSystem(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		UApplyDamage m_ApplyDamage;
		UUpdateWoundDuration m_UpdateWoundDuration;
	};
}