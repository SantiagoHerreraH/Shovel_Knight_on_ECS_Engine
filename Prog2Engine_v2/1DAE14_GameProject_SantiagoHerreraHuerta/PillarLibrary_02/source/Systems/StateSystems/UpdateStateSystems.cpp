#include "../../../include/Systems/StateSystems/UpdateStateSystems.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Systems/CurveSystems/UCurveSystem.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"


namespace Pillar {

#pragma region SMovement

	void SMovementCalculation::ApplyMovementType(EMovementType movementType, const FPoint2f& movement, FVelocity& velocityToModify, FNetForce2D& netForceToModify, bool applyMovementOnXAxis, bool applyMovementOnYAxis)
	{
		switch (movementType)
		{
		case Pillar::EMovementType::AddVelocity:
			if (applyMovementOnXAxis)
			{
				velocityToModify.DistancePerSecond.X += movement.X;
			}
			if (applyMovementOnYAxis)
			{
				velocityToModify.DistancePerSecond.Y += movement.Y;
			}
			break;
		case Pillar::EMovementType::AddForce:

			if (applyMovementOnXAxis)
			{
				netForceToModify.NetForcePerTick.X += movement.X;
			}
			if (applyMovementOnYAxis)
			{
				netForceToModify.NetForcePerTick.Y += movement.Y;
			}
			break;
		case Pillar::EMovementType::OverrideVelocity:
			if (applyMovementOnXAxis)
			{
				velocityToModify.DistancePerSecond.X = movement.X;
			}
			if (applyMovementOnYAxis)
			{
				velocityToModify.DistancePerSecond.Y = movement.Y;
			}
			break;
		case Pillar::EMovementType::OverrideForce:
			if (applyMovementOnXAxis)
			{
				netForceToModify.NetForcePerTick.X = movement.X;
			}
			if (applyMovementOnYAxis)
			{
				netForceToModify.NetForcePerTick.Y = movement.Y;
			}
			break;
		default:
			break;
		}
	}

	FPoint2f SMovementCalculation::GetMovementDirectionBasedOnMovementType(URegistry* const registry, EMovementDirectionType movementDirectionType, const FEntity& entityToGetDirectionFrom, const FPoint2f& customMovement)
	{
		FPoint2f currentMovement{};
		FHealth* currentHealth{ nullptr };
		FEntityResolveOverlapInfo* currentResolveOverlapInfo{ nullptr };

		switch (movementDirectionType)
		{
		case EMovementDirectionType::OnlyCustom:

			currentMovement = customMovement;

			break;

		case EMovementDirectionType::FromAttackingEntityCollisionDirection:
			//no checking here -> I prefer for it to crash
			currentResolveOverlapInfo = registry->GetComponent<FEntityResolveOverlapInfo>(entityToGetDirectionFrom);
			currentHealth = registry->GetComponent<FHealth>(entityToGetDirectionFrom);

			for (size_t i = 0; i < currentHealth->EntitiesBeingAttackedBy.NumOfEntities(); i++)
			{
				if (currentResolveOverlapInfo->Entities_To_ResolveOverlapInfo.Contains(currentHealth->EntitiesBeingAttackedBy.EntityAt(i)))
				{
					currentMovement.X +=
						currentResolveOverlapInfo->Entities_To_ResolveOverlapInfo.
						ConstValueAt(currentHealth->EntitiesBeingAttackedBy.EntityAt(i)).TranslationVector.X;

					currentMovement.Y +=
						currentResolveOverlapInfo->Entities_To_ResolveOverlapInfo.
						ConstValueAt(currentHealth->EntitiesBeingAttackedBy.EntityAt(i)).TranslationVector.Y;
				}
			}

			currentMovement =
				SVectors::NormalizeVector(currentMovement);

			break;

		case EMovementDirectionType::FromAttackingEntityDirection:

			URealTransform2D* currentOtherRealTransform{nullptr};
			URealTransform2D* currentRealTransform = registry->GetComponent<URealTransform2D>(entityToGetDirectionFrom);
			currentHealth = registry->GetComponent<FHealth>(entityToGetDirectionFrom);
			FPoint2f currentDelta{};

			for (size_t i = 0; i < currentHealth->EntitiesBeingAttackedBy.NumOfEntities(); i++)
			{
				currentOtherRealTransform = registry->GetComponent<URealTransform2D>(currentHealth->EntitiesBeingAttackedBy.EntityAt(i).GetConstEntity());
				currentDelta = SVectors::Subtract(
					currentRealTransform->GetConstRealWorldTransform().Position,
					currentOtherRealTransform->GetConstRealWorldTransform().Position);

				currentMovement.X += currentDelta.X;

				currentMovement.Y += currentDelta.Y;
			}

			currentMovement =
				SVectors::NormalizeVector(currentMovement);


			break;
		}
		return currentMovement;
	}

	FPoint2f SMovementCalculation::CalculateVectorMovementOverTime(const FVectorMovementOverTime& movement, const FPoint2f& movementDirection)
	{
		float timeOfMovement = movement.TimeOfMovement == 0 ? 0.000001 : movement.TimeOfMovement;
		double currentCurveValue = SCurves::CurveEvaluator.EvaluateCurve(movement.ChangeInMagnitude, movement.CurrentTime / timeOfMovement);
		float magnitude = currentCurveValue * movement.NormalMagnitude;
		magnitude =
			magnitude < movement.BottomMagnitude ? movement.BottomMagnitude :
			magnitude > movement.TopMagnitude ? movement.TopMagnitude : magnitude;
		return SVectors::Scale(movementDirection, magnitude);
	}

#pragma endregion

#pragma region Rotation Transformation

	UUpdateState_RotationTransformation::UUpdateState_RotationTransformation(URegistry& registry) :
		m_Registry(registry)
	{
		FState<FRotationTransformation> rotationTransformationState{};
		FWorldTransform2D worldTransform{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(rotationTransformationState, worldTransform);

		m_Registry.GetComponents(m_RotationTransformationStates, filter);
		m_Registry.GetComponents(m_WorldTransforms, filter);
	}

	void UUpdateState_RotationTransformation::Update()
	{
		FRotationTransformation* currentRotationTransformation{ nullptr };

		for (size_t archIdx = 0; archIdx < m_RotationTransformationStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_RotationTransformationStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_RotationTransformationStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); )
				{
					currentRotationTransformation = &m_RotationTransformationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx);
					switch (currentRotationTransformation->RotationType)
					{
					case EOperationType::Add:

						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationX += currentRotationTransformation->Rotation.X;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY += currentRotationTransformation->Rotation.Y;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationZ += currentRotationTransformation->Rotation.Z;
						break;
					case EOperationType::Override:
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationX = currentRotationTransformation->Rotation.X;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY = currentRotationTransformation->Rotation.Y;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationZ = currentRotationTransformation->Rotation.Z;
						break;
						break;
					}

					if (currentRotationTransformation->TerminateEachTick)
					{
						m_Registry.GetComponent<UStateMachine>(m_RotationTransformationStates.At(archIdx, compIdx).StateControllerEntityHandle.GetConstEntity())
							->TriggerTypeSpecificEventRemoval<DStateController_RotationTransformation>
							(m_RotationTransformationStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
					}
					else
					{
						++stateIdx;
					}
				}
			}
		}
	}

#pragma endregion

#pragma region Vector Movement

	UUpdateState_VectorMove::UUpdateState_VectorMove(URegistry& registry) :
		m_Registry(registry)
	{
		FState<FVectorMovement> vectorMovement{};
		FVelocity velocity{};
		FNetForce2D netForce{};

		UComponentFilter filter{ };
		filter.SetIncludingFilter(vectorMovement, velocity, netForce);

		registry.GetComponents(m_VectorMovements, filter);
		registry.GetComponents(m_Velocities, filter);
		registry.GetComponents(m_NetForces, filter);
	}

	void UUpdateState_VectorMove::Update()
	{
		FVectorMovement* currentVectorMovement{ nullptr };
		FVelocity* currentVelocity{ nullptr };
		FNetForce2D* currentNetForce{ nullptr };
		FPoint2f currentMovement{};
		int activatedSizeBefore{};

		for (size_t archIdx = 0; archIdx < m_VectorMovements.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_VectorMovements.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_VectorMovements.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); )
				{
					currentVectorMovement = &m_VectorMovements.At(archIdx, compIdx).EventName_To_State.At(stateIdx);
					currentVelocity = &m_Velocities.At(archIdx, compIdx);
					currentNetForce = &m_NetForces.At(archIdx, compIdx);

					SMovementCalculation::ApplyMovementType(
						currentVectorMovement->MovementType,
						currentVectorMovement->Movement,
						*currentVelocity,
						*currentNetForce,
						currentVectorMovement->ApplyMovementOnXAxis,
						currentVectorMovement->ApplyMovementOnYAxis);


					if (currentVectorMovement->TerminateEachTick)
					{
						activatedSizeBefore = m_VectorMovements.At(archIdx, compIdx).EventName_To_State.ActivatedSize();

						m_Registry.GetComponent<UStateMachine>(m_VectorMovements.At(archIdx, compIdx).StateControllerEntityHandle.GetConstEntity())
							->TriggerTypeSpecificEventRemoval<DStateController_VectorMovement>
							(m_VectorMovements.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));

						if (m_VectorMovements.At(archIdx, compIdx).EventName_To_State.ActivatedSize() >= activatedSizeBefore)
						{
							++stateIdx;
						}
					}
					else
					{
						++stateIdx;
					}
				}

			}
		}
	}

#pragma endregion

#pragma region Vector Movement Over Time

	UUpdateState_VectorMoveOverTime::UUpdateState_VectorMoveOverTime(URegistry& registry) :
		m_Registry(registry)
	{
		FState<FVectorMovementOverTime> vectorMovementOverTime{};
		FVelocity velocity{};
		FNetForce2D netForce{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(vectorMovementOverTime, velocity, netForce);

		registry.GetComponents(m_VectorMovementsOverTime, filter);
		registry.GetComponents(m_Velocities, filter);
		registry.GetComponents(m_NetForces, filter);
	}

	void UUpdateState_VectorMoveOverTime::Update(float deltaSeconds)
	{
		FVectorMovementOverTime* currentVectorMovementOverTime{ nullptr };
		FVelocity* currentVelocity{ nullptr };
		FNetForce2D* currentNetForce{ nullptr };

		URealTransform2D* currentRealTransform{ nullptr };
		URealTransform2D* currentOtherRealTransform{ nullptr };
		FEntityResolveOverlapInfo* currentResolveOverlapInfo{ nullptr };
		FHealth* currentHealth{ nullptr };

		size_t sizeBeforeDeactivation{};

		FPoint2f currentMovement{};
		bool unsubscribe{ false };

		for (size_t archIdx = 0; archIdx < m_VectorMovementsOverTime.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_VectorMovementsOverTime.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.ActivatedSize();)
				{
					currentVectorMovementOverTime = &m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.At(stateIdx);
					currentVelocity = &m_Velocities.At(archIdx, compIdx);
					currentNetForce = &m_NetForces.At(archIdx, compIdx);

					currentMovement = SMovementCalculation::GetMovementDirectionBasedOnMovementType(
						&m_Registry,
						currentVectorMovementOverTime->MovementDirectionType,
						currentVectorMovementOverTime->EntityToGetMovementDirectionInfoFrom.GetConstEntity(),
						currentVectorMovementOverTime->MovementDirection);

					if (!currentVectorMovementOverTime->GetXAxisFromMovementDirection)
					{
						currentMovement.X = 0;
					}
					if (!currentVectorMovementOverTime->GetYAxisFromMovementDirection)
					{
						currentMovement.Y = 0;
					}

					if (currentVectorMovementOverTime->OverrideMovementOnStart &&
						currentVectorMovementOverTime->CurrentTime == 0)
					{
						FPoint2f startingMovement{};

						startingMovement = SMovementCalculation::GetMovementDirectionBasedOnMovementType(
							&m_Registry,
							currentVectorMovementOverTime->MovementOnStartDirectionType,
							currentVectorMovementOverTime->EntityToGetStartMovementDirectionInfoFrom.GetConstEntity(),
							currentVectorMovementOverTime->MovementValueForOverrideOnStart);

						startingMovement = SVectors::Scale(startingMovement, currentVectorMovementOverTime->StartMovementMultiple);

						if (currentVectorMovementOverTime->AddCustomToStartMovement && 
							currentVectorMovementOverTime->MovementOnStartDirectionType != EMovementDirectionType::OnlyCustom)
						{
							startingMovement = SVectors::Add(startingMovement, currentVectorMovementOverTime->MovementValueForOverrideOnStart);
						}

						SMovementCalculation::ApplyMovementType(
							currentVectorMovementOverTime->MovementTypeForStartOverride, 
							startingMovement,
							*currentVelocity,
							*currentNetForce,
							currentVectorMovementOverTime->ApplyStartMovementOnXAxis,
							currentVectorMovementOverTime->ApplyStartMovementOnYAxis);
					}

					currentMovement = SMovementCalculation::CalculateVectorMovementOverTime(*currentVectorMovementOverTime, currentMovement);

					if (currentVectorMovementOverTime->AddCustomToMovement &&
						currentVectorMovementOverTime->MovementDirectionType != EMovementDirectionType::OnlyCustom)
					{
						currentMovement = SVectors::Add(currentMovement, currentVectorMovementOverTime->MovementDirection);
					}

					SMovementCalculation::ApplyMovementType(
						currentVectorMovementOverTime->MovementType,
						currentMovement,
						*currentVelocity,
						*currentNetForce,
						currentVectorMovementOverTime->ApplyMovementOnXAxis,
						currentVectorMovementOverTime->ApplyMovementOnYAxis);

					currentVectorMovementOverTime->CurrentTime += deltaSeconds;

					switch (currentVectorMovementOverTime->MovementTerminationType)
					{
					case EMovementTerminationType::TerminateAfterTimeTranspired:
						if (currentVectorMovementOverTime->CurrentTime >= currentVectorMovementOverTime->TimeOfMovement)
						{
							if (currentVectorMovementOverTime->ResetMovementCounterOnTermination)
							{
								currentVectorMovementOverTime->CurrentTime = 0;
							}

							unsubscribe = true;
						}
						break;
					case EMovementTerminationType::TerminateEachTick:
					default:
						if (currentVectorMovementOverTime->ResetMovementCounterOnTermination)
						{
							currentVectorMovementOverTime->CurrentTime = 0;
						}

						unsubscribe = true;

						break;
					}

					sizeBeforeDeactivation = m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.ActivatedSize();

					if (unsubscribe)
					{

						m_Registry.GetComponent<UStateMachine>(m_VectorMovementsOverTime.At(archIdx, compIdx).StateControllerEntityHandle.GetConstEntity())
							->TriggerTypeSpecificEventRemoval<DStateController_VectorMovementOverTime>
							(m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
					}

					if (sizeBeforeDeactivation == m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.ActivatedSize())
					{
						++stateIdx;
					}
				}

				//Definitely not optimal but will do for now
				for (size_t stateIdx = m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.ActivatedSize();
					stateIdx < m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.Size(); stateIdx++)
				{
					if (m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.At(stateIdx).ResetMovementCounterOnTermination)
					{
						m_VectorMovementsOverTime.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentTime = 0.f;
					}
				}
			}
		}
	}

#pragma endregion


	UUpdateState_Activation::UUpdateState_Activation(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine(registry, UEntityHandle{})),
		m_SceneRegistry(registry)
	{
		UStateMachine stateMachine(registry, UEntityHandle{});
		FState<FActivationState> activationState{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, activationState, FSceneObject{});

		registry.GetComponents(m_ActivationStates, filter);
		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_SceneObjects, filter);
	}

	void UUpdateState_Activation::Update(float deltaSeconds)
	{
		m_EntitiesToChangeState.clear();

		bool timeForAction{ false };
		EEntityActivationAction entityActivationAction{};
		FEntity currentStateControllerEntity{};
		UStateMachine* currentStateMachine{ nullptr };

		int currentSceneIndex{ m_SceneRegistry.Get().CurrentSceneIndex };

		//You have to give each state access to the entity handle they are responsible for, and the entity handle of the state machine

		for (size_t archIdx = 0; archIdx < m_ActivationStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_ActivationStates.NumOfComponents(archIdx); compIdx++)
			{
				if (m_SceneObjects.At(archIdx, compIdx).SceneIndex != currentSceneIndex)
				{
					continue;
				}
				for (size_t stateIdx = 0; stateIdx < m_ActivationStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					entityActivationAction = m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).EntityActivationAction;
					currentStateControllerEntity = m_ActivationStates.At(archIdx, compIdx).StateControllerEntityHandle.GetConstEntity();

					m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentTime += deltaSeconds;

					if (m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentTime >=
						m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).TimeUntilEntityActivationAction)
					{
						m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentTime = 0;

						m_EntitiesToChangeState.push_back(FEntityAndActivationAction(currentStateControllerEntity, entityActivationAction));

						currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

						switch (m_ActivationStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
						{
						case EStateRemovalType::Local:
							currentStateMachine->TriggerTypeSpecificEventRemoval<DStateController_Activation>
								(m_ActivationStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
							break;

						case EStateRemovalType::AllStateMachines:
							currentStateMachine->TriggerState
							(m_ActivationStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
							break;
						}
					}
					
				}
			}
		}


		for (size_t i = 0; i < m_EntitiesToChangeState.size(); i++)
		{
			switch (m_EntitiesToChangeState.at(i).EntityActivationAction)
			{
			case EEntityActivationAction::Activate:
				m_Registry.ActivateEntity(m_EntitiesToChangeState.at(i).Entity);
				break;
			case EEntityActivationAction::Deactivate:
				m_Registry.DeactivateEntity(m_EntitiesToChangeState.at(i).Entity);
				break;
			}
		}
		

	

	}

	

	UUpdateState_Transform::UUpdateState_Transform(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		FState<FTransformState> transform{ };
		FWorldTransform2D worldTransform{};
		UStateMachine stateMachine(registry, UEntityHandle{});

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateMachine, transform, worldTransform);

		registry.GetComponents(m_WorldTransforms,	filter);
		registry.GetComponents(m_TransformStates,	filter);
		registry.GetComponents(m_StateMachines,     filter);
	}

	void UUpdateState_Transform::Update()
	{

		FEntity currentReferenceEntity{};
		UEntityHandleContainer* referencedEntities{ nullptr };
		FTransform2D* referencedTransform{ nullptr };
		bool usedReference{ false };
		UStateMachine* currentStateMachine{ nullptr };

		for (size_t archIdx = 0; archIdx < m_TransformStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_TransformStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_TransformStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					usedReference = false;
					size_t& currentIndex{
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.
							At(stateIdx).Current_Transform_ReferenceEntities_Index };

					referencedEntities =  
						&m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).ReferenceEntitiesFor_Transform ;

					currentReferenceEntity = referencedEntities->EntityAt(currentIndex).GetConstEntity();

					referencedTransform = &m_Registry.GetComponent<FWorldTransform2D>(currentReferenceEntity)->Transform; 
					// if nullptr, entity is invalid and you forgot to fill in referenced entts


					switch (m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).ValueModificationTypeFor_Position)
					{
					case EValueModificationType::OverrideCustom:
						m_WorldTransforms.At(archIdx, compIdx).Transform.Position = 
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Transform.Position;
						break;
					case EValueModificationType::OverrideFromOtherEntity:

						m_WorldTransforms.At(archIdx, compIdx).Transform.Position = referencedTransform->Position;
						usedReference = true;
						break;
					}

					switch (m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).ValueModificationTypeFor_Rotation)
					{
					case EValueModificationType::OverrideCustom:
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationX =
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Transform.RotationX;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY =
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Transform.RotationY;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationZ =
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Transform.RotationZ;
						break;
					case EValueModificationType::OverrideFromOtherEntity:

						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationX = referencedTransform->RotationX;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY = referencedTransform->RotationY;
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationZ = referencedTransform->RotationZ;

						usedReference = true;
						break;
					}

					switch (m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).ValueModificationTypeFor_Scale)
					{
					case EValueModificationType::OverrideCustom:
						m_WorldTransforms.At(archIdx, compIdx).Transform.Scale =
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Transform.Scale;
						break;
					case EValueModificationType::OverrideFromOtherEntity:

						m_WorldTransforms.At(archIdx, compIdx).Transform.Scale = referencedTransform->Scale;

						usedReference = true;
						break;
					}

					if (usedReference && m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).IncreaseIndexAfterUsingReference)
					{
						currentIndex = currentIndex < referencedEntities->NumOfEntities() - 1 ? currentIndex + 1 : currentIndex;
					}

					currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

					switch (m_TransformStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						currentStateMachine->TriggerTypeSpecificEventRemoval<DStateController_Transform>(
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						currentStateMachine->TriggerState(
							m_TransformStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}
	}

	UUpdateState_Targets::UUpdateState_Targets(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		FState<FTargetsOfInterest> targetState{};
		FTarget target{};
		UStateMachine stateMachine{ registry, UEntityHandle{} };

		UComponentFilter filter{};
		filter.SetIncludingFilter(targetState, target, stateMachine);

		registry.GetComponents(m_TargetsOfInterest_States, filter);
		registry.GetComponents(m_Targets, filter);//
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_Targets::Update()
	{
		for (size_t archIdx = 0; archIdx < m_TargetsOfInterest_States.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_TargetsOfInterest_States.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					bool currentIndexIncreases = m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentIndexIncreases;
					size_t& currentTargetIndex = m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).CurrentTargetIndex;
					EValueBehavior valueBehavior = m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).RangeBehaviourOfTargetIndex;
					size_t targetSize = m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Targets.NumOfEntities();
					m_Targets.At(archIdx, compIdx).CurrentTarget =		m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Targets.EntityAt(currentTargetIndex);
					m_Targets.At(archIdx, compIdx).CurrentTargetInfo =	m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).Targets.ValueAt(currentTargetIndex);

					switch (valueBehavior)
					{
					case EValueBehavior::Clamp:

						if (currentIndexIncreases)
						{
							currentTargetIndex = currentTargetIndex < targetSize - 1 ? currentTargetIndex + 1 : currentTargetIndex;
						}
						else
						{
							currentTargetIndex = currentTargetIndex > 0 ? currentTargetIndex - 1 : currentTargetIndex;
						}
						
						break;
					case EValueBehavior::Loop:

						if (currentIndexIncreases)
						{
							currentTargetIndex = currentTargetIndex < targetSize - 1 ? currentTargetIndex + 1 : 0;
						}
						else
						{
							currentTargetIndex = currentTargetIndex > 0 ? currentTargetIndex - 1 : targetSize - 1;
						}

						break;
					case EValueBehavior::Rebound:

						if (currentIndexIncreases)
						{
							currentTargetIndex = currentTargetIndex < targetSize - 1 ? currentTargetIndex + 1 : currentTargetIndex;

							currentIndexIncreases = currentTargetIndex == targetSize - 1 ? !currentIndexIncreases : currentIndexIncreases;
						}
						else
						{
							currentTargetIndex = currentTargetIndex > 0 ? currentTargetIndex - 1 : currentTargetIndex;

							currentIndexIncreases = currentTargetIndex == 0 ? !currentIndexIncreases : currentIndexIncreases;
						}
						
						break;
					}

					size_t previousActivatedSize = m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.ActivatedSize();

					UStateMachine* currentStateMachine = &m_StateMachines.At(archIdx, compIdx);

					switch (m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						currentStateMachine->TriggerTypeSpecificEventRemoval<DStateController_TargetChooser>
							(m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						currentStateMachine->TriggerState
						(m_TargetsOfInterest_States.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ConditionalRemoval);
						break;
					}
				}
			}
		}

	}

}