#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FLife.h"
#include "../../../include/Structs/FTag.h"

namespace Pillar {

	//have to do this because std::vector<bool> doesn't allow std::swap
	enum class EActivation {
		Inactive,
		Active
	};

	struct FRunTimeOverlapInfo {
		TActivatableFlatMap<std::string, EActivation> TagsCollidedWith;
		int NumberOfTicksNotCollided;
	};

	class UCheckOverlap final {

	public:
		UCheckOverlap(URegistry& registry);
		void Initialize();
		void Check();

	private:
		//values have to be between -1 and 1
		const double m_UP_COLLISION_DOT_PRODUCT_THRESHOLD{ 0.7 };
		const int m_TICK_THRESHOLD_TO_COUNT_GROUNDCOLLISION{ 2 };

		URegistry& m_Registry;

		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FEntityResolveOverlapInfo> m_OverlapInfo;

		UEntityHandleMap<FRunTimeOverlapInfo> m_Entities_To_RunTimeOverlapInfo;
	};

	class UCheckVelocity final{

	public:
		UCheckVelocity(URegistry& registry);
		void Check();

	private:
		const float m_STILL_VELOCITY_CHECK_THRESHOLD{ 3.f};

		const float m_Y_POS_VELOCITY_CHECK_THRESHOLD{ 3.f };
		const float m_Y_NEG_VELOCITY_CHECK_THRESHOLD{ -3.f };

		const float m_X_POS_VELOCITY_CHECK_THRESHOLD{ 3.f };
		const float m_X_NEG_VELOCITY_CHECK_THRESHOLD{ -3.f };

		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FVelocity> m_Velocities;
	};

	class UCheckAttack final {
	public:
		UCheckAttack(URegistry& registry);
		void Check();

	private:

		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FAttack> m_Attack;
	};

	class UCheckHealth final {

	public:
		UCheckHealth(URegistry& registry);
		void Check();

	private:

		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FHealth> m_Health;
	};
	
	class UCheckAttackTargetInformation {
	public:
		UCheckAttackTargetInformation(URegistry& registry);
		void Check();

	private:

		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FTarget> m_Targets;
	};

	class UEventChecker final{

	public:
		UEventChecker(URegistry& registry);
		void Initialize();
		void Check();

	private:
		UCheckVelocity m_CheckVelocity;
		UCheckOverlap m_CheckOverlap;
		UCheckHealth m_CheckHealth;
		UCheckAttack m_CheckAttack;
		UCheckAttackTargetInformation m_CheckTargetInformation;
	};
}