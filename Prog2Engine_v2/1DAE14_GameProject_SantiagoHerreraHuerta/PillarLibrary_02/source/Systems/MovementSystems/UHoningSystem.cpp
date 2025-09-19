#include "../../../include/Systems/MovementSystems/UHoningSystem.h"
#include "../../../include/Systems/MathSystems/SVectors.h"

namespace Pillar {

	UExecuteSimpleHomingAction::UExecuteSimpleHomingAction(URegistry& registry) :
		m_Registry(registry)
	{
		FHomingInformation homingInfo{};
		FTarget targets{};
		FVelocity velocity{};
		FNetForce2D force{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(homingInfo, targets, velocity, force);

		m_Registry.GetComponents(m_HomingInfo, filter);
		m_Registry.GetComponents(m_Velocities, filter);
		m_Registry.GetComponents(m_Targets, filter);
		m_Registry.GetComponents(m_Forces, filter);
	}
	void UExecuteSimpleHomingAction::Update()
	{
		m_Registry.Lock<FHomingInformation>();
		m_Registry.Lock<FTarget>();
		m_Registry.Lock<FVelocity>();

		FPoint2f distanceFromTarget{};
		FPoint2f movementToApply{};

		FHomingInformation* currentHomingInfo{ nullptr };

		for (size_t archIdx = 0; archIdx < m_Velocities.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Velocities.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentHomingInfo = &m_HomingInfo.At(archIdx, compIdx);
				distanceFromTarget = m_Targets.At(archIdx, compIdx).CurrentTargetInfo.DistanceFromTarget;

				if (currentHomingInfo->MovementIsRelativeToDistance)
				{
					movementToApply =
						SVectors::Scale(distanceFromTarget, m_HomingInfo.At(archIdx, compIdx).
							MovementMagnitudePerDistance.GetSpeed(SVectors::Length(distanceFromTarget)));
				}
				else
				{
					movementToApply =
						SVectors::Scale(SVectors::NormalizeVector(distanceFromTarget), m_HomingInfo.At(archIdx, compIdx).
							MovementMagnitudePerDistance.GetSpeed(SVectors::Length(distanceFromTarget)));

				}


				if (currentHomingInfo->IsActive)
				{
					switch (currentHomingInfo->MovementType)
					{
					case EMovementType::AddVelocity:
						
						if (currentHomingInfo->ApplyOnX)
						{
							m_Velocities.At(archIdx, compIdx).DistancePerSecond.X += movementToApply.X;
						}
						if (currentHomingInfo->ApplyOnY)
						{
							m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y += movementToApply.Y;
						}
						
						break;
					case EMovementType::OverrideVelocity:

						if (currentHomingInfo->ApplyOnX)
						{
							m_Velocities.At(archIdx, compIdx).DistancePerSecond.X = movementToApply.X;
						}
						if (currentHomingInfo->ApplyOnY)
						{
							m_Velocities.At(archIdx, compIdx).DistancePerSecond.Y = movementToApply.Y;
						}

						break;
					case EMovementType::AddForce:

						if (currentHomingInfo->ApplyOnX)
						{
							m_Forces.At(archIdx, compIdx).NetForcePerTick.X += movementToApply.X;
						}
						if (currentHomingInfo->ApplyOnY)
						{
							m_Forces.At(archIdx, compIdx).NetForcePerTick.Y += movementToApply.Y;
						}

						break;
					case EMovementType::OverrideForce:

						if (currentHomingInfo->ApplyOnX)
						{
							m_Forces.At(archIdx, compIdx).NetForcePerTick.X = movementToApply.X;
						}
						if (currentHomingInfo->ApplyOnY)
						{
							m_Forces.At(archIdx, compIdx).NetForcePerTick.Y = movementToApply.Y;
						}

						break;
					}
				}	
			
			}
		}

		m_Registry.Unlock<FHomingInformation>();
		m_Registry.Unlock<FTarget>();
		m_Registry.Unlock<FVelocity>();
	}

	UCalculateTargetInformation::UCalculateTargetInformation(URegistry& registry) :
		m_Registry(registry)
	{
		URealTransform2D realTransform;
		FTarget targets{};
		UComponentFilter filter{};
		filter.SetIncludingFilter(targets, realTransform);

		m_Registry.GetComponents(m_Targets, filter);
		m_Registry.GetComponents(m_RealTransforms, filter);
	}
	void UCalculateTargetInformation::Update()
	{
		m_Registry.Lock<URealTransform2D>();

		URealTransform2D* entityToFollowRealTransform{ nullptr };

		for (size_t archIdx = 0; archIdx < m_Targets.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Targets.NumOfActivatedComponents(archIdx); compIdx++)
			{
				entityToFollowRealTransform = m_Registry.GetComponent<URealTransform2D>(m_Targets.At(archIdx, compIdx).CurrentTarget.GetConstEntity());
				m_Targets.At(archIdx, compIdx).CurrentTargetInfo.DistanceFromTarget =
					SVectors::Subtract(
						entityToFollowRealTransform->GetConstRealWorldTransform().Position,
						m_RealTransforms.At(archIdx, compIdx).GetConstRealWorldTransform().Position);

				entityToFollowRealTransform = m_Registry.GetComponent<URealTransform2D>(m_Targets.At(archIdx, compIdx).PlayerTarget.GetConstEntity());
				m_Targets.At(archIdx, compIdx).CurrentPlayerTargetInfo.DistanceFromTarget =
					SVectors::Subtract(
						entityToFollowRealTransform->GetConstRealWorldTransform().Position,
						m_RealTransforms.At(archIdx, compIdx).GetConstRealWorldTransform().Position);


			}
		}

		m_Registry.Unlock<URealTransform2D>();
	}

	UExecuteSimpleLookAtRotation::UExecuteSimpleLookAtRotation(URegistry& registry) :
		m_Registry(registry)
	{
		FLookAtRotationInfo lookAtInfo{};
		FTarget targets{};
		FWorldTransform2D worldTransform{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(lookAtInfo, targets, worldTransform);

		m_Registry.GetComponents(m_LookAtRotationInfo, filter);
		m_Registry.GetComponents(m_WorldTransforms, filter);
		m_Registry.GetComponents(m_Targets, filter);
	}

	void UExecuteSimpleLookAtRotation::Update()
	{
		for (size_t archIdx = 0; archIdx < m_LookAtRotationInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_LookAtRotationInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				if (m_LookAtRotationInfo.At(archIdx, compIdx).IsActive)
				{

					if (m_LookAtRotationInfo.At(archIdx, compIdx).ApplyOnY)
					{
						if (m_Targets.At(archIdx, compIdx).CurrentTargetInfo.DistanceFromTarget.X < 0)
						{
							m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY = 180;
						}
						else
						{
							m_WorldTransforms.At(archIdx, compIdx).Transform.RotationY = 0;
						}
						
					}
					if (m_LookAtRotationInfo.At(archIdx, compIdx).ApplyOnZ)
					{
						m_WorldTransforms.At(archIdx, compIdx).Transform.RotationZ = SVectors::Direction(m_Targets.At(archIdx, compIdx).CurrentTargetInfo.DistanceFromTarget).AngleInDegrees;
					}
					
				}

			}
		}
	}

}