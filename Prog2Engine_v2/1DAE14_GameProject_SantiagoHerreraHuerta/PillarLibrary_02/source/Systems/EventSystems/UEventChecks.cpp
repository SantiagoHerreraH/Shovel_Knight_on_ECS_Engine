#include "../../../include/Systems/EventSystems/UEventChecks.h"
#include "../../../include/Systems/EventSystems/SEventNames.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {

	UCheckVelocity::UCheckVelocity(URegistry& registry) :
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UStateMachine stateMachine{ registry, UEntityHandle{} };
		FVelocity velocity{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, velocity);

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_Velocities, filter);
	}
	void UCheckVelocity::Check()
	{

		UStateMachine* currentStateMachine{ nullptr };

		for (size_t archIdx = 0; archIdx < m_StateMachines.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_StateMachines.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

				if (m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y < m_Y_NEG_VELOCITY_CHECK_THRESHOLD)
				{
					currentStateMachine->TriggerState(SEventNames::OnFall, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnFall, EEventStateToTrigger::ForcedRemoval);
				}

				if (m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y > m_Y_POS_VELOCITY_CHECK_THRESHOLD)
				{
					currentStateMachine->TriggerState(SEventNames::OnJump, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnJump, EEventStateToTrigger::ForcedRemoval);
				}

				if (SVectors::Length(m_Velocities.At(archIdx, compIdx).DistancePerSecond) < m_STILL_VELOCITY_CHECK_THRESHOLD)
				{
					currentStateMachine->TriggerState(SEventNames::OnStill, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnStill, EEventStateToTrigger::ForcedRemoval);
				}

				if (m_Velocities.At(archIdx, compIdx).DistancePerSecond.X < m_X_NEG_VELOCITY_CHECK_THRESHOLD)
				{
					currentStateMachine->TriggerState(SEventNames::OnMoveLeft, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnMoveLeft, EEventStateToTrigger::ForcedRemoval);
				}

				if (m_Velocities.At(archIdx, compIdx).DistancePerSecond.X > m_X_POS_VELOCITY_CHECK_THRESHOLD)
				{
					currentStateMachine->TriggerState(SEventNames::OnMoveRight, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnMoveRight, EEventStateToTrigger::ForcedRemoval);
				}
			}
		}
	}

	UCheckOverlap::UCheckOverlap(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		UStateMachine stateMachine(registry, UEntityHandle{});
		FEntityResolveOverlapInfo overlapInfo{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, overlapInfo);

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_OverlapInfo, filter);
	}

	void UCheckOverlap::Initialize()
	{
		for (size_t archIdx = 0; archIdx < m_StateMachines.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_StateMachines.NumOfComponents(archIdx); compIdx++)
			{
				m_Entities_To_RunTimeOverlapInfo.Add(m_StateMachines.At(archIdx, compIdx).GetEntityHandle(), FRunTimeOverlapInfo{});
			}
		}
	}

	void UCheckOverlap::Check()
	{
		m_Registry.Lock<FTag>();

		FPoint2 currentTranslationVector{};
		bool collidedDown{false};
		UStateMachine* currentStateMachine{ nullptr };
		FTag* currentTag{ nullptr };

		for (size_t archIdx = 0; archIdx < m_StateMachines.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_StateMachines.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

				if (m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities() > 0)
				{
					currentStateMachine->TriggerState(SEventNames::OnCollision, EEventStateToTrigger::ForcedExecution);
				}
				else
				{
					currentStateMachine->TriggerState(SEventNames::OnCollision, EEventStateToTrigger::ForcedRemoval);
				}

				m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.DeactivateAll();

				for (size_t i = 0; i < m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); i++)
				{
					currentTranslationVector.X = m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(i).TranslationVector.X;
					currentTranslationVector.Y = m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(i).TranslationVector.Y;

					currentTag = m_Registry.GetComponent<FTag>(m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(i).GetConstEntity());

					if (currentTag)
					{
						m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.Push(currentTag->Name, EActivation::Active);
						m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.Activate(currentTag->Name);
						m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.ValueAt(currentTag->Name) = EActivation::Active;
						
						currentStateMachine->TriggerState(SEventNames::OnCollisionWith(currentTag->Name), EEventStateToTrigger::ForcedExecution);
					}

					if (m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(i).DirectionOfCollided.Y == -1)
					{
						collidedDown = true;
						m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).NumberOfTicksNotCollided = 0;
					}
				}
				
				for (size_t i = m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.ActivatedSize();
					i < m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.Size(); i++)
				{
					if (m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.ConstAt(i) == EActivation::Active)
					{
						m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.At(i) = EActivation::Inactive;

						currentStateMachine->TriggerState(SEventNames::OnCollisionWith(
							m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).TagsCollidedWith.ConstKeyAt(i)), 
							EEventStateToTrigger::ForcedRemoval);
					}
				}
				
				if (collidedDown && 
					currentStateMachine->EventState(SEventNames::OnGroundCollision) == EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnGroundCollision, EEventStateToTrigger::ForcedExecution);
				}

				if (!collidedDown && 
					currentStateMachine->EventState(SEventNames::OnGroundCollision) == EEventState::Active)
				{
					++m_Entities_To_RunTimeOverlapInfo.ValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).NumberOfTicksNotCollided;
				}
				
				if (m_Entities_To_RunTimeOverlapInfo.ConstValueAt(m_StateMachines.At(archIdx, compIdx).GetEntityHandle()).NumberOfTicksNotCollided >=
					m_TICK_THRESHOLD_TO_COUNT_GROUNDCOLLISION && 
					currentStateMachine->EventState(SEventNames::OnGroundCollision) != EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnGroundCollision, EEventStateToTrigger::ForcedRemoval);
				}
				
				collidedDown = false;
				
			}
		}

		m_Registry.Unlock<FTag>();
	}

	UCheckHealth::UCheckHealth(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{})),
		m_Health(FHealth(0, 1))
	{
		UStateMachine stateMachine(registry, UEntityHandle{});
		FHealth health{0, 1};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, health);

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_Health, filter);
	}

	void UCheckHealth::Check()
	{
		UStateMachine* currentStateMachine{ nullptr };

		for (size_t archIdx = 0; archIdx < m_Health.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Health.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

				if (m_Health.At(archIdx, compIdx).Wounded &&
					currentStateMachine->EventState(SEventNames::OnReceiveDamage) != EEventState::Active)
				{
					currentStateMachine->TriggerState(SEventNames::OnReceiveDamage, EEventStateToTrigger::ForcedExecution);
				}
				else if(!m_Health.At(archIdx, compIdx).Wounded &&
					currentStateMachine->EventState(SEventNames::OnReceiveDamage) != EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnReceiveDamage, EEventStateToTrigger::ForcedRemoval);
				}
				if (m_Health.At(archIdx, compIdx).CurrentHealth <= 0 &&
					currentStateMachine->EventState(SEventNames::OnZeroHealth) != EEventState::Active)
				{
					currentStateMachine->TriggerState(SEventNames::OnZeroHealth, EEventStateToTrigger::ForcedExecution);
				}
				else if(m_Health.At(archIdx, compIdx).CurrentHealth > 0 &&
					currentStateMachine->EventState(SEventNames::OnZeroHealth) != EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnZeroHealth, EEventStateToTrigger::ForcedRemoval);
				}
				
			}
		}
	}


	UCheckAttackTargetInformation::UCheckAttackTargetInformation(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		UStateMachine stateMachine(registry, UEntityHandle{});
		FTarget targetInfo{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, targetInfo);

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_Targets, filter);
	}

	void UCheckAttackTargetInformation::Check()
	{
		FPoint2f currentDistance{};
		FPoint2f triggerDistance{};
		bool inRange{};

		UStateMachine* currentStateMachine{ nullptr };


		for (size_t archIdx = 0; archIdx < m_StateMachines.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_StateMachines.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

				currentDistance = m_Targets.At(archIdx, compIdx).CurrentTargetInfo.DistanceFromTarget;
				triggerDistance = m_Targets.At(archIdx, compIdx).CurrentTargetInfo.TriggerDistance;

				inRange = abs(currentDistance.X) <= triggerDistance.X && abs(currentDistance.Y) <= triggerDistance.Y;

				if (inRange && currentStateMachine->EventState(SEventNames::OnTargetInRange) == EEventState::Inactive)
				{
 					currentStateMachine->TriggerState(SEventNames::OnTargetInRange, EEventStateToTrigger::ForcedExecution);
				}
				else if(!inRange && currentStateMachine->EventState(SEventNames::OnTargetInRange) == EEventState::Active)
				{
					currentStateMachine->TriggerState(SEventNames::OnTargetInRange, EEventStateToTrigger::ForcedRemoval);
				}

				currentDistance = m_Targets.At(archIdx, compIdx).CurrentPlayerTargetInfo.DistanceFromTarget;
				triggerDistance = m_Targets.At(archIdx, compIdx).CurrentPlayerTargetInfo.TriggerDistance;

				inRange = abs(currentDistance.X) <= triggerDistance.X && abs(currentDistance.Y) <= triggerDistance.Y;

				if (inRange && currentStateMachine->EventState(SEventNames::OnPlayerInRange) == EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnPlayerInRange, EEventStateToTrigger::ForcedExecution);
				}
				else if(!inRange && currentStateMachine->EventState(SEventNames::OnPlayerInRange) == EEventState::Active)
				{
					currentStateMachine->TriggerState(SEventNames::OnPlayerInRange, EEventStateToTrigger::ForcedRemoval);
				}
			}
		}
	}

	UCheckAttack::UCheckAttack(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		UStateMachine stateMachine(registry, UEntityHandle{});
		FAttack attack{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, attack);

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_Attack, filter);
	}

	void UCheckAttack::Check()
	{
		UStateMachine* currentStateMachine{ nullptr };

		for (size_t archIdx = 0; archIdx < m_Attack.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Attack.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

				if (m_Attack.At(archIdx, compIdx).AttackedThisTick && currentStateMachine->EventState(SEventNames::OnAttackHit) == EEventState::Inactive)
				{
					currentStateMachine->TriggerState(SEventNames::OnAttackHit, EEventStateToTrigger::ForcedExecution);
				}
				else if (!m_Attack.At(archIdx, compIdx).AttackedThisTick && currentStateMachine->EventState(SEventNames::OnAttackHit) == EEventState::Active)
				{
					currentStateMachine->TriggerState(SEventNames::OnAttackHit, EEventStateToTrigger::ForcedRemoval);
				}
			}
		}
	}

	UEventChecker::UEventChecker(URegistry& registry) :
		m_CheckVelocity(registry),
		m_CheckOverlap(registry),
		m_CheckHealth(registry),
		m_CheckAttack(registry),
		m_CheckTargetInformation(registry)
	{}

	void UEventChecker::Initialize()
	{
		m_CheckOverlap.Initialize();
	}

	void UEventChecker::Check()
	{
		m_CheckAttack.Check();
		m_CheckVelocity.Check();
		m_CheckOverlap.Check();
		m_CheckHealth.Check();
		m_CheckTargetInformation.Check();
	}

	

}

