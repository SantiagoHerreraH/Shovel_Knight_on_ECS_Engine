#pragma once


#include "../../../include/Core/URegistry.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Systems/MovementSystems/UMovementData.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FLife.h"
#include "../../../include/Systems/TransformSystems/USpawnPointManager.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {

	struct FCollisionBodyTypeState {
		ECollisionBodyType CollisionBodyType;
		EStateRemovalType StateRemovalType;
	};

	struct FEnvironmentForcesState {
		FEnvironmentForces EnvironmentForces;
		EStateRemovalType StateRemovalType;
	};

	using DStateController_Homing =				UStateController<FHomingInfoState,			false, true>;
	using DStateController_LookAt =				UStateController<FLookAtRotationInfoState,	false, true>;
	using DStateController_EnvironmentForces =	UStateController<FEnvironmentForcesState,	false, true>;
	using DStateController_CollisionBodyType =	UStateController<FCollisionBodyTypeState,	false, true>;
	using DStateController_HealthImmunity =		UStateController<FHealthImmunityState,		false, true>;
	//FINISH THIS HEALTH IMMUNITY SET UP 

	class UUpdateState_HomingInformation {
	public:
		UUpdateState_HomingInformation(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FHomingInfoState>> m_HomingInfo_States;
		UComponentIterator<FHomingInformation>		   m_HomingInfo;
		UComponentIterator<UStateMachine>			   m_StateMachines;
	};

	class UUpdateState_LookAtInformation {
	public:
		UUpdateState_LookAtInformation(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FLookAtRotationInfoState>> m_LooAtInfo_States;
		UComponentIterator<FLookAtRotationInfo>				 m_LooAtInfo;
		UComponentIterator<UStateMachine>					 m_StateMachines;
	};


	class UCheckStateMachineRelationships {

	public:
		UCheckStateMachineRelationships(URegistry& registry);
		void Check();

	private:
		UComponentIterator<UStateMachine>					 m_StateMachines;

	};


	class UUpdateState_CollisionBodyType {
	public:
		UUpdateState_CollisionBodyType(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FCollisionBodyTypeState>>	m_CollisionBodyTypesStates;
		UComponentIterator<ECollisionBodyType>			m_CollisionBodyTypes;
		UComponentIterator<UStateMachine>				m_StateMachines;
	};

	class UUpdateState_EnvironmentForce {
	public:
		UUpdateState_EnvironmentForce(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FEnvironmentForcesState>>	m_EnvironmentForcesStates;
		UComponentIterator<FEnvironmentForces>			m_EnvironmentForces;
		UComponentIterator<UStateMachine>				m_StateMachines;
	};

	class UUpdateState_HealthImmunity {
	public:
		UUpdateState_HealthImmunity(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FHealthImmunityState>> m_HealthImmunityStates;
		UComponentIterator<FHealth>						 m_Health;
		UComponentIterator<UStateMachine>				 m_StateMachines;
	};
}