#include "../../../include/Systems/PhysicsSystems/UPhysics.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Structs/FTag.h"
#include "../../../include/Systems/MathSystems/STools.h"

namespace Pillar {
	UPhysics::UPhysics(URegistry& registry) :
		m_ApplyForceToVel(registry),
		m_ApplyTransformChanges(registry),
		m_ApplyRectRestrictedTransformChanges(registry),
		m_ResetPhysicsVariables(registry),
		m_ExecutePhysicsCollision(registry),
		m_ApplyEnvironmentForces(registry),
		m_ApplyStickiness(registry),
		m_ExecuteRectPhysicsCollision(registry)
	{}

	void UPhysics::Update(float deltaSeconds)
	{
		m_ApplyEnvironmentForces.Update();
		m_ApplyForceToVel.Update(deltaSeconds);
		m_ExecuteRectPhysicsCollision.Update(); 
		m_ApplyStickiness.Update();
		m_ApplyTransformChanges.Update(deltaSeconds);
		m_ApplyRectRestrictedTransformChanges.Update(deltaSeconds);
		m_ResetPhysicsVariables.Reset();
	}

	UApplyForceToVelocity::UApplyForceToVelocity(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		FVelocity velocity;
		FNetForce2D force;
		FMass mass;
		filter.SetIncludingFilter(velocity, force, mass);

		m_Registry.GetComponents(m_Velocities, filter);
		m_Registry.GetComponents(m_Forces, filter);
		m_Registry.GetComponents(m_Masses, filter);
	}

	void UApplyForceToVelocity::Update(float deltaSeconds)
	{

		FVelocity* velocity{nullptr};

		for (size_t archetypeIdx = 0; archetypeIdx < m_Velocities.NumOfArchetypes(); archetypeIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Velocities.NumOfActivatedComponents(archetypeIdx); compIdx++)
			{
				velocity = &m_Velocities.At(archetypeIdx, compIdx);

				velocity->DistancePerSecond.X += deltaSeconds * m_Forces.At(archetypeIdx, compIdx).NetForcePerTick.X / m_Masses.At(archetypeIdx, compIdx).Kilograms;
				velocity->DistancePerSecond.Y += deltaSeconds * m_Forces.At(archetypeIdx, compIdx).NetForcePerTick.Y / m_Masses.At(archetypeIdx, compIdx).Kilograms;
			}
		}
	}

	UApplyTransformChanges::UApplyTransformChanges(URegistry& registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FWorldTransform2D{}, FVelocity{});
		filter.SetExcludingFilter(FRectCollider{});

		registry.GetComponents(m_Transforms, filter);
		registry.GetComponents(m_Velocities, filter);
	}

	void UApplyTransformChanges::Update(float deltaSeconds)
	{
		FWorldTransform2D* transform{ nullptr }; 
		FVelocity* velocity{ nullptr };

		for (size_t archIdx = 0; archIdx < m_Transforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Transforms.NumOfActivatedComponents(archIdx); compIdx++)
			{
				transform = &m_Transforms.At(archIdx, compIdx);
				velocity = &m_Velocities.At(archIdx, compIdx);

				transform->Transform.Position.X += velocity->DistancePerSecond.X * deltaSeconds;
				transform->Transform.Position.Y += velocity->DistancePerSecond.Y * deltaSeconds;
			}
		}
	}

	UApplyRectRestrictedTransformChanges::UApplyRectRestrictedTransformChanges(URegistry& registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FWorldTransform2D{}, FVelocity{}, FRectCollider{});

		registry.GetComponents(m_Transforms, filter);
		registry.GetComponents(m_Velocities, filter);
		registry.GetComponents(m_RectColliders, filter);
	}

	void UApplyRectRestrictedTransformChanges::Update(float deltaSeconds)
	{
		FWorldTransform2D* transform{nullptr}; //was local transform before
		FVelocity* velocity{nullptr};

		float currentX{};
		float currentY{};

		float currentRangeX{};
		float currentRangeY{};

		const float delta{ 1 };

		for (size_t archIdx = 0; archIdx < m_Transforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Transforms.NumOfActivatedComponents(archIdx); compIdx++)
			{
				transform = &m_Transforms.At(archIdx, compIdx);
				velocity = &m_Velocities.At(archIdx, compIdx);

				currentX = velocity->DistancePerSecond.X * deltaSeconds;
				currentY = velocity->DistancePerSecond.Y * deltaSeconds;
				
				currentRangeX = (m_RectColliders.At(archIdx, compIdx).CurrentRect.Width )/2.f ;
				currentRangeY = (m_RectColliders.At(archIdx, compIdx).CurrentRect.Height )/2.f;

				currentX = 
					currentX < -currentRangeX ? -currentRangeX :
					currentX > currentRangeX ? currentRangeX : currentX;

				currentY =
					currentY < -currentRangeY ? -currentRangeY :
					currentY > currentRangeY ? currentRangeY : currentY;

				transform->Transform.Position.X += currentX;
				transform->Transform.Position.Y += currentY;
			}
		}
	}

	UResetPhysicsVariables::UResetPhysicsVariables(URegistry& registry)
	{
		registry.GetComponents(m_Forces);
	}

	void UResetPhysicsVariables::Reset()
	{
		for (size_t archIdx = 0; archIdx < m_Forces.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Forces.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_Forces.At(archIdx, compIdx).NetForcePerTick.X = 0;
				m_Forces.At(archIdx, compIdx).NetForcePerTick.Y= 0;
			}
		}
	}


#pragma region ExecutePhysicsCollision

	UExecutePhysicsCollision::UExecutePhysicsCollision(URegistry& registry) :
		m_Registry(registry)
	{
		FVelocity velocity{};
		ECollisionBodyType physicsBodyType{};
		FEntityResolveOverlapInfo entityOverlapInfo{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(velocity, physicsBodyType, entityOverlapInfo);

		m_Registry.GetComponents(m_Velocities, filter);
		m_Registry.GetComponents(m_CollisionBodyType, filter);
		m_Registry.GetComponents(m_EntityCollisionInfo, filter);
	}

	void UExecutePhysicsCollision::Update()
	{
		ECollisionBodyType otherCompPhysicsBodyType{};

		FPoint2f currentTranslationAmount{};
		FVelocity currentVelocity{};
		double percentageOfScalar{};

		FPoint2f projectedVelocityOnCollisionNormal{};
		float dotBetweenCollisionNormalAndProjectedVelocity{};
		float finalLength{};
		double velocityLengthScalar{};


		for (size_t archIdx = 0; archIdx < m_EntityCollisionInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EntityCollisionInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t enttIdx = 0; enttIdx < m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
				{
					if (m_CollisionBodyType.At(archIdx, compIdx) != ECollisionBodyType::Solid)
					{
						break;
					}

					const FEntityResolveOverlapInfo& info = m_EntityCollisionInfo.At(archIdx, compIdx);
					const UEntityHandle& otherEntityHandle = m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx);
					const FResolveOverlapInfo& resolveOverlapInfo = m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(enttIdx);

					m_Registry.GetComponent<ECollisionBodyType>(otherCompPhysicsBodyType, otherEntityHandle);
					
					if (otherCompPhysicsBodyType == ECollisionBodyType::OverlapAll)
					{
						continue;
					}

					percentageOfScalar = 0.5;

					currentVelocity = m_Velocities.At(archIdx, compIdx);

					projectedVelocityOnCollisionNormal = SVectors::ProjectedVector(resolveOverlapInfo.TranslationVector, currentVelocity.DistancePerSecond);
					dotBetweenCollisionNormalAndProjectedVelocity = SVectors::DotProduct(projectedVelocityOnCollisionNormal, resolveOverlapInfo.TranslationVector);
					velocityLengthScalar = dotBetweenCollisionNormalAndProjectedVelocity < 0 ? -1 : 1;

					currentTranslationAmount = SVectors::Scale(projectedVelocityOnCollisionNormal, velocityLengthScalar * percentageOfScalar);

					m_Velocities.At(archIdx, compIdx).DistancePerSecond.X += currentTranslationAmount.X;
					m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y += currentTranslationAmount.Y;
				}
			}
		}
	}

#pragma endregion

	UApplyEnvironmentForce::UApplyEnvironmentForce(URegistry& registry) :
		m_Registry(registry)
	{
		FEnvironmentForces envForces;
		FVelocity velocity{};
		FNetForce2D force;
		UComponentFilter filter{};
		filter.SetIncludingFilter(envForces, force, velocity);

		registry.GetComponents(m_EnvironmentForces, filter);
		registry.GetComponents(m_Forces, filter);
		registry.GetComponents(m_Velocities, filter);
	}

	void UApplyEnvironmentForce::Update()
	{
		m_Registry.Lock<FVector>();
		for (size_t archIdx = 0; archIdx < m_EnvironmentForces.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EnvironmentForces.NumOfActivatedComponents(archIdx); compIdx++)
			{
				//m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.Create(FPoint2f(1000,100));
				for (size_t forceIdx = 0; forceIdx < m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.NumOfActivatedComponents(); forceIdx++)
				{
					switch (m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).MovementType)
					{
					case EMovementType::AddVelocity:
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.X += m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.X;
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y += m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.Y;
						break;
					case EMovementType::OverrideVelocity:
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.X = m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.X;
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y = m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.Y;
						break;
					case EMovementType::AddForce:
						m_Forces.At(archIdx, compIdx).NetForcePerTick.X += m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.X;
						m_Forces.At(archIdx, compIdx).NetForcePerTick.Y += m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.Y;
						break;
					case EMovementType::OverrideForce:
						m_Forces.At(archIdx, compIdx).NetForcePerTick.X = m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.X;
						m_Forces.At(archIdx, compIdx).NetForcePerTick.Y = m_EnvironmentForces.At(archIdx, compIdx).ForcesToApplyPerTick.At(forceIdx).Movement.Y;
						break;
					default:
						break;
					}
					
				}
			}
		}
	}

	UApplyStickiness::UApplyStickiness(URegistry& registry) :
		m_Registry(registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FVelocity{}, URealTransform2D{}, FSticky{}, FEntityResolveOverlapInfo{});

		registry.GetComponents(m_Stickies, filter);
		registry.GetComponents(m_EntityCollisionInfo, filter);
		registry.GetComponents(m_Velocities, filter);
	}

	void UApplyStickiness::Update()
	{
		FNetForce2D* currentOtherNetForce{ nullptr };
		FVelocity* currentOtherVelocity{ nullptr };
		FTag* currentOtherTag{ nullptr };
		FStickyInformation* currentStickyInfo{nullptr};
		FPoint2f stickyVelocity{};

		for (size_t archIdx = 0; archIdx < m_EntityCollisionInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EntityCollisionInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{

				for (size_t enttIdx = 0; enttIdx < m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
				{
					currentOtherTag = m_Registry.GetComponent<FTag>(m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx).GetConstEntity());

					if (currentOtherTag && 
						m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ValueAt(enttIdx).DirectionOfCollided.Y == 1 &&
						m_Stickies.At(archIdx, compIdx).Tag_To_StickyInformation.Contains(currentOtherTag->Name))
					{
						m_Stickies.At(archIdx, compIdx).Tag_To_StickyInformation.ActivateAt(enttIdx);
						currentStickyInfo = &m_Stickies.At(archIdx, compIdx).Tag_To_StickyInformation.ValueAt(currentOtherTag->Name);

						currentOtherVelocity = m_Registry.GetComponent<FVelocity>(m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx).GetConstEntity());

						if (currentOtherVelocity)
						{
							stickyVelocity.X =  m_Velocities.At(archIdx, compIdx).DistancePerSecond.X * currentStickyInfo->MultipleOnVelocity;
							stickyVelocity.Y =  m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y * currentStickyInfo->MultipleOnVelocity;

							ApplySticky(currentOtherVelocity->DistancePerSecond.X, stickyVelocity.X);
							ApplySticky(currentOtherVelocity->DistancePerSecond.Y, stickyVelocity.Y);
						}

					}
				}

			}
		}
	}

	void UApplyStickiness::ApplySticky(float& velocityAxisToAffect, float stickyVelocityAxis)
	{
		if (stickyVelocityAxis >= 0 && velocityAxisToAffect >= 0)
		{
			velocityAxisToAffect =
				velocityAxisToAffect < stickyVelocityAxis ?
				stickyVelocityAxis :
				velocityAxisToAffect;
		}
		else if (stickyVelocityAxis < 0 && velocityAxisToAffect < 0)
		{
			velocityAxisToAffect =
				velocityAxisToAffect > stickyVelocityAxis ?
				stickyVelocityAxis :
				velocityAxisToAffect;
		}
		else
		{
			velocityAxisToAffect += stickyVelocityAxis;
		}
	}

	UExecuteRectPhysicsCollision::UExecuteRectPhysicsCollision(URegistry& registry) :
		m_Registry(registry)
	{
		FVelocity velocity{};
		ECollisionBodyType physicsBodyType{};
		FEntityResolveOverlapInfo entityOverlapInfo{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(velocity, physicsBodyType, entityOverlapInfo);
		filter.SetExcludingFilter(FStaticColliderFlag{});

		m_Registry.GetComponents(m_Velocities, filter);
		m_Registry.GetComponents(m_CollisionBodyType, filter);
		m_Registry.GetComponents(m_EntityCollisionInfo, filter);
	}

	void UExecuteRectPhysicsCollision::Update()
	{
		ECollisionBodyType otherCompPhysicsBodyType{};

		FPoint2f currentTranslationAmount{};
		FVelocity* currentVelocity{nullptr};
		double percentageOfScalar{};

		FPoint2f projectedVelocityOnCollisionNormal{};
		float dotBetweenCollisionNormalAndProjectedVelocity{};
		float finalLength{};
		double velocityLengthScalar{};


		for (size_t archIdx = 0; archIdx < m_EntityCollisionInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_EntityCollisionInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t enttIdx = 0; enttIdx < m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
				{
					if (m_CollisionBodyType.At(archIdx, compIdx) == ECollisionBodyType::OverlapAll)
					{
						break;
					}

					const FEntityResolveOverlapInfo& info = m_EntityCollisionInfo.At(archIdx, compIdx);
					const UEntityHandle& otherEntityHandle = m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx);
					const FResolveOverlapInfo& resolveOverlapInfo = m_EntityCollisionInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstValueAt(enttIdx);

					m_Registry.GetComponent<ECollisionBodyType>(otherCompPhysicsBodyType, otherEntityHandle);

					if (otherCompPhysicsBodyType != ECollisionBodyType::Solid)
					{
						continue;
					}

					currentVelocity = &m_Velocities.At(archIdx, compIdx);
					
					if (resolveOverlapInfo.DirectionOfCollided.X == 1 &&
						currentVelocity->DistancePerSecond.X > 0)
					{
						currentVelocity->DistancePerSecond.X = 0;
					}
					else if (
						resolveOverlapInfo.DirectionOfCollided.X == -1 &&
						currentVelocity->DistancePerSecond.X < 0)
					{
						currentVelocity->DistancePerSecond.X = 0;
					}
					if (
						resolveOverlapInfo.DirectionOfCollided.Y == 1 &&
						currentVelocity->DistancePerSecond.Y > 0)
					{
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y = 0;
					}
					else if (resolveOverlapInfo.DirectionOfCollided.Y == -1 &&
						currentVelocity->DistancePerSecond.Y < 0)
					{
						m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y = 0;
					}

				}
			}
		}
	}

}
