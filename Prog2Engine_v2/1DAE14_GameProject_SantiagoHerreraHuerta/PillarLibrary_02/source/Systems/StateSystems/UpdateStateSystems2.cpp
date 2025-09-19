
#include "../../../include/Systems/StateSystems/UpdateStateSystems2.h"
#include "../../../include/Structs/FTag.h"

namespace Pillar {

	UUpdateState_HomingInformation::UUpdateState_HomingInformation(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		FState<FHomingInfoState> homingInfo_State{};
		FHomingInformation homingInfo{};
		UStateMachine stateMachine{ registry, UEntityHandle{} };

		UComponentFilter filter{};
		filter.SetIncludingFilter(homingInfo_State, homingInfo, stateMachine);

		registry.GetComponents(m_HomingInfo_States, filter);
		registry.GetComponents(m_HomingInfo, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_HomingInformation::Update()
	{
		for (size_t archIdx = 0; archIdx < m_HomingInfo_States.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_HomingInfo_States.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_HomingInfo_States.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					m_HomingInfo.At(archIdx, compIdx) = m_HomingInfo_States.At(archIdx, compIdx).EventName_To_State.ConstAt(stateIdx).HomingInfo;

					switch (m_HomingInfo_States.At(archIdx, compIdx).EventName_To_State.ConstAt(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval<DStateController_Homing>(m_HomingInfo_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_HomingInfo_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

	UUpdateState_LookAtInformation::UUpdateState_LookAtInformation(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		FState<FLookAtRotationInfoState> lookAt_State{};
		FLookAtRotationInfo lookAtInfo{};
		UStateMachine stateMachine{ registry, UEntityHandle{} };

		UComponentFilter filter{};
		filter.SetIncludingFilter(lookAt_State, lookAtInfo, stateMachine);

		registry.GetComponents(m_LooAtInfo_States, filter);
		registry.GetComponents(m_LooAtInfo, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_LookAtInformation::Update()
	{
		for (size_t archIdx = 0; archIdx < m_LooAtInfo_States.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_LooAtInfo_States.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_LooAtInfo_States.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					m_LooAtInfo.At(archIdx, compIdx) = m_LooAtInfo_States.At(archIdx, compIdx).EventName_To_State.ConstAt(stateIdx).LookAtRotationInfo;

					switch (m_LooAtInfo_States.At(archIdx, compIdx).EventName_To_State.ConstAt(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval<DStateController_Homing>(m_LooAtInfo_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_LooAtInfo_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

	UCheckStateMachineRelationships::UCheckStateMachineRelationships(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		registry.GetComponents(m_StateMachines);
	}
	void UCheckStateMachineRelationships::Check()
	{
		//For now this has to be num of components, or else deactivated entities can't spawn from triggered logic 
		for (size_t archIdx = 0; archIdx < m_StateMachines.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_StateMachines.NumOfComponents(archIdx); compIdx++)
			{
				m_StateMachines.At(archIdx, compIdx).CheckStateTriggersOfOtherStateMachines();
			}
		}
	}

	UUpdateState_CollisionBodyType::UUpdateState_CollisionBodyType(URegistry& registry) :
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FState<FCollisionBodyTypeState>{}, ECollisionBodyType{}, UStateMachine{ registry, UEntityHandle{} });

		registry.GetComponents(m_CollisionBodyTypesStates, filter);
		registry.GetComponents(m_CollisionBodyTypes, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_CollisionBodyType::Update()
	{
		for (size_t archIdx = 0; archIdx < m_CollisionBodyTypesStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_CollisionBodyTypesStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_CollisionBodyTypesStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					m_CollisionBodyTypes.At(archIdx, compIdx) = m_CollisionBodyTypesStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CollisionBodyType;

					switch (m_CollisionBodyTypesStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_CollisionBodyType>(m_CollisionBodyTypesStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_CollisionBodyTypesStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

	UUpdateState_EnvironmentForce::UUpdateState_EnvironmentForce(URegistry& registry) :
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FState<FEnvironmentForcesState>{}, FEnvironmentForces{}, UStateMachine{ registry, UEntityHandle{} });

		registry.GetComponents(m_EnvironmentForcesStates, filter);
		registry.GetComponents(m_EnvironmentForces, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_EnvironmentForce::Update()
	{
		for (size_t archIdx = 0; archIdx < m_EnvironmentForcesStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EnvironmentForcesStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_EnvironmentForcesStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					m_EnvironmentForces.At(archIdx, compIdx) = m_EnvironmentForcesStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).EnvironmentForces;

					switch (m_EnvironmentForcesStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_CollisionBodyType>(m_EnvironmentForcesStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_EnvironmentForcesStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

	UUpdateState_HealthImmunity::UUpdateState_HealthImmunity(URegistry& registry) :
		m_Health(FHealth{ 1 }),
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FState<FHealthImmunityState>{}, FHealth{1}, UStateMachine{ registry, UEntityHandle{} });

		registry.GetComponents(m_HealthImmunityStates, filter);
		registry.GetComponents(m_Health, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_HealthImmunity::Update()
	{
		for (size_t archIdx = 0; archIdx < m_HealthImmunityStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_HealthImmunityStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_HealthImmunityStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					m_Health.At(archIdx, compIdx).CurrentlyImmune = m_HealthImmunityStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentlyImmune;

					switch (m_HealthImmunityStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_HealthImmunity>(m_HealthImmunityStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_HealthImmunityStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

}