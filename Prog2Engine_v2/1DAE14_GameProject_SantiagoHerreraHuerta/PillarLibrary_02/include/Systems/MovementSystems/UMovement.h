#pragma once

#include "../../../include/Systems/MovementSystems/UHoningSystem.h"
#include "../../../include/Systems/StateSystems/UpdateStateSystems.h"
#include "../../../include/Systems/StateSystems/UpdateStateSystems2.h"
#include "../../../include/Systems/StateSystems/UpdateStateSystems3.h"

namespace Pillar {

	class UTransformState final {

	public:
		UTransformState(URegistry& registry);

		void Update(float deltSeconds);

	private:
		UCheckStateMachineRelationships m_CheckStateMachineRelationships;

		UUpdateState_Activation				m_UpdateState_Activation;
		UUpdateState_Transform				m_UpdateState_Transform;
		UUpdateState_VectorMove				m_UpdateState_VectorMove;
		UUpdateState_VectorMoveOverTime		m_UpdateState_VectorMoveOverTime;
		UUpdateState_RotationTransformation	m_UpdateRotationTransformation;
		UUpdateState_Targets				m_UpdateState_Targets;
		UUpdateState_HomingInformation		m_UpdateState_HomingInformation;
		UUpdateState_LookAtInformation		m_UpdateState_LookAtInformation;
		UUpdateState_CollisionBodyType		m_UpdateState_CollisionBodyType;
		UUpdateState_EnvironmentForce		m_UpdateState_EnvironmentForce;
		UUpdateState_HealthImmunity			m_UpdateState_HealthImmunity;
		UUpdateState_SpawnManager			m_UpdateState_SpawnManager;
		UUpdateState_SceneChanger			m_UpdateState_SceneChanger;
		UUpdateState_SoundEffects			m_UpdateState_SoundEffects;

		UCalculateTargetInformation m_CalculateTargetInformation;
		UExecuteSimpleHomingAction m_ExecuteSimpleHomingAction;
		UExecuteSimpleLookAtRotation m_ExecuteSimpleLookAtRotation;
		
	};

}