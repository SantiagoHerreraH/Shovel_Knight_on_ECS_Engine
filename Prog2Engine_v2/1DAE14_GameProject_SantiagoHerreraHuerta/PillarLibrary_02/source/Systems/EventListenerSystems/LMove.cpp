#include "../../../include/Systems/EventListenersSystems/LMove.h"
#include "../../../include/Systems/CreationSystems/UCreateEvents.h"
#include "../../../include/Systems/CurveSystems/UCurveSystem.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Structs/FPhysics.h"

namespace Pillar {
	


	//LVectorMove::LVectorMove(URegistry& registry) :
	//	UEventListener(registry, SEventListenerNames::VectorMove){}

	//void LVectorMove::Update()
	//{
	//	FVectorMovement currentVectorMovement{};
	//	FVelocity currentVelocity{};
	//	FNetForce2D currentNetForce{};

	//	for (size_t i = 0; i < GetSubscribedEntityHandles()->NumOfEntities(); i++)
	//	{
	//		m_Registry.GetComponent<FVectorMovement>(currentVectorMovement, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());
	//		
	//		switch (currentVectorMovement.MovementType)
	//		{
	//		case EMovementType::AddForce:

	//			m_Registry.GetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentNetForce.NetForcePerTick.X += currentVectorMovement.Movement.X;
	//			currentNetForce.NetForcePerTick.Y += currentVectorMovement.Movement.Y;

	//			m_Registry.SetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::OverrideForce:

	//			m_Registry.GetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentNetForce.NetForcePerTick.X = currentVectorMovement.Movement.X;
	//			currentNetForce.NetForcePerTick.Y = currentVectorMovement.Movement.Y;
	//			
	//			m_Registry.SetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::AddVelocity:

	//			m_Registry.GetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentVelocity.DistancePerSecond.X += currentVectorMovement.Movement.X;
	//			currentVelocity.DistancePerSecond.Y += currentVectorMovement.Movement.Y;

	//			m_Registry.SetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::OverrideVelocity:

	//			m_Registry.GetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentVelocity.DistancePerSecond.X = currentVectorMovement.Movement.X;
	//			currentVelocity.DistancePerSecond.Y = currentVectorMovement.Movement.Y;

	//			m_Registry.SetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());
	//			break;
	//		default:
	//			break;
	//		}

	//		UnsubscribeEntityHandle(GetSubscribedEntityHandles()->ConstEntityAt(i));
	//	}
	//}

	//LVectorMoveOverTime::LVectorMoveOverTime(URegistry& registry) :
	//	UEventListener(registry, SEventListenerNames::VectorMoveOverTime)
	//{
	//}

	//void LVectorMoveOverTime::Update(float deltaSeconds)
	//{
	//	FVectorMovementOverTime currentVectorMovementOverTime{};
	//	FPoint2f currentMovement{};
	//	FVelocity currentVelocity{};
	//	FNetForce2D currentNetForce{};
	//	bool unsubscribe{false};

	//	for (size_t i = 0; i < GetSubscribedEntityHandles()->NumOfEntities(); i++)
	//	{
	//		m_Registry.GetComponent<FVectorMovementOverTime>(currentVectorMovementOverTime, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//		currentMovement = CalculateVectorMovementOverTime(currentVectorMovementOverTime);

	//		switch (currentVectorMovementOverTime.MovementType)
	//		{
	//		case EMovementType::AddForce:

	//			m_Registry.GetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentNetForce.NetForcePerTick.X += currentMovement.X;
	//			currentNetForce.NetForcePerTick.Y += currentMovement.Y;

	//			m_Registry.SetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::OverrideForce:

	//			m_Registry.GetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentNetForce.NetForcePerTick.X = currentMovement.X;
	//			currentNetForce.NetForcePerTick.Y = currentMovement.Y;

	//			m_Registry.SetComponent<FNetForce2D>(currentNetForce, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::AddVelocity:

	//			m_Registry.GetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentVelocity.DistancePerSecond.X += currentMovement.X;
	//			currentVelocity.DistancePerSecond.Y += currentMovement.Y;

	//			m_Registry.SetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			break;
	//		case EMovementType::OverrideVelocity:

	//			m_Registry.GetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());

	//			currentVelocity.DistancePerSecond.X = currentMovement.X;
	//			currentVelocity.DistancePerSecond.Y = currentMovement.Y;

	//			m_Registry.SetComponent<FVelocity>(currentVelocity, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());
	//			break;
	//		default:
	//			break;
	//		}

	//		currentVectorMovementOverTime.CurrentTime += deltaSeconds;

	//		switch (currentVectorMovementOverTime.MovementTerminationType)
	//		{
	//		case EMovementTerminationType::TerminateAfterTimeTranspired:
	//			if (currentVectorMovementOverTime.CurrentTime >= currentVectorMovementOverTime.TimeOfMovement)
	//			{
	//				if (currentVectorMovementOverTime.ResetMovementCounterOnTermination)
	//				{
	//					currentVectorMovementOverTime.CurrentTime = 0;
	//				}

	//				unsubscribe = true;
	//			}
	//			break;
	//		case EMovementTerminationType::TerminateEachTick:
	//		default:
	//			if (currentVectorMovementOverTime.ResetMovementCounterOnTermination)
	//			{
	//				currentVectorMovementOverTime.CurrentTime = 0;
	//			}

	//			unsubscribe = true;

	//			break;
	//		}

	//		m_Registry.SetComponent<FVectorMovementOverTime>(currentVectorMovementOverTime, GetSubscribedEntityHandles()->ConstEntityAt(i).GetConstEntity());
	//		
	//		if (unsubscribe)
	//		{
	//			UnsubscribeEntityHandle(GetSubscribedEntityHandles()->EntityAt(i));
	//		}
	//		

	//	}
	//}

	//FPoint2f LVectorMoveOverTime::CalculateVectorMovementOverTime(const FVectorMovementOverTime& movement)
	//{
	//	float timeOfMovement = movement.TimeOfMovement == 0 ? 0.000001 : movement.TimeOfMovement;
	//	double currentCurveValue = SCurves::CurveEvaluator.EvaluateCurve(movement.ChangeInMagnitude, movement.CurrentTime/ timeOfMovement);
	//	return SVectors::Scale(movement.MovementDirection, currentCurveValue * (movement.TopMagnitude - movement.BottomMagnitude));;
	//}

}