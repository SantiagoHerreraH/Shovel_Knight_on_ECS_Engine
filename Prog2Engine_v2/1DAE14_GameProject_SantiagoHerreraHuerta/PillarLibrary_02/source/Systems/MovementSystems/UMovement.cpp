#include "../../../include/Systems/MovementSystems/UMovement.h"

namespace Pillar {

	UTransformState::UTransformState(URegistry& registry) :
		m_CheckStateMachineRelationships(registry),
		m_UpdateState_Activation(registry),
		m_UpdateState_Transform(registry), 
		m_UpdateState_VectorMove(registry),
		m_UpdateState_VectorMoveOverTime(registry),
		m_UpdateRotationTransformation(registry),
		m_CalculateTargetInformation(registry),
		m_ExecuteSimpleHomingAction(registry),
		m_UpdateState_Targets(registry),
		m_UpdateState_HomingInformation(registry),
		m_ExecuteSimpleLookAtRotation(registry),
		m_UpdateState_LookAtInformation(registry),
		m_UpdateState_CollisionBodyType(registry),
		m_UpdateState_EnvironmentForce(registry),
		m_UpdateState_HealthImmunity(registry),
		m_UpdateState_SpawnManager(registry),
		m_UpdateState_SceneChanger(registry),
		m_UpdateState_SoundEffects(registry)
	{
	}
	void UTransformState::Update(float deltaSeconds)
	{
		m_CheckStateMachineRelationships.Check();
		m_UpdateState_Activation.Update(deltaSeconds);
		m_UpdateState_Transform.Update();
		m_UpdateState_VectorMove.Update();
		m_UpdateState_VectorMoveOverTime.Update(deltaSeconds);
		m_UpdateRotationTransformation.Update();
		m_UpdateState_Targets.Update();
		m_UpdateState_HomingInformation.Update();
		m_UpdateState_LookAtInformation.Update();
		m_UpdateState_CollisionBodyType.Update();
		m_UpdateState_EnvironmentForce.Update();
		m_UpdateState_HealthImmunity.Update();
		m_UpdateState_SpawnManager.Update();
		m_UpdateState_SceneChanger.Update();
		m_UpdateState_SoundEffects.Update();

		m_CalculateTargetInformation.Update();
		m_ExecuteSimpleHomingAction.Update();
		m_ExecuteSimpleLookAtRotation.Update();
		
	}

}