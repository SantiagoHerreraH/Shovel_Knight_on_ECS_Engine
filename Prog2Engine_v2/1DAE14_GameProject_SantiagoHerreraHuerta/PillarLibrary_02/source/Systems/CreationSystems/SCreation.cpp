#include "../../../include/Systems/CreationSystems/SCreation.h"

namespace Pillar {

	FVectorMovementOverTime SMovementCreation::GetHitMovement(const UEntityHandle& handle)
	{
		FVectorMovementOverTime receiveDamage{};
		receiveDamage.MovementDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.EntityToGetMovementDirectionInfoFrom = handle;
		receiveDamage.BottomMagnitude = 800;
		receiveDamage.NormalMagnitude = 1000;
		receiveDamage.TopMagnitude = 2100;
		receiveDamage.TimeOfMovement = 0.2;
		receiveDamage.MovementType = EMovementType::OverrideForce;
		receiveDamage.ApplyMovementOnXAxis = true;
		receiveDamage.ApplyMovementOnYAxis = true;
		receiveDamage.MovementTerminationType = EMovementTerminationType::TerminateAfterTimeTranspired;
		receiveDamage.ResetMovementCounterOnTermination = true;

		receiveDamage.OverrideMovementOnStart = true;
		receiveDamage.MovementTypeForStartOverride = EMovementType::OverrideVelocity;
		receiveDamage.MovementValueForOverrideOnStart = FPoint2f{ 0,0 };
		receiveDamage.ApplyStartMovementOnXAxis = true;
		receiveDamage.ApplyStartMovementOnYAxis = true;

		FCurvePoint beginCurvePoint{};
		beginCurvePoint.Point = { 0, 10 };
		beginCurvePoint.RightTangentPoint.AngleInDegrees = 0;// 120;
		beginCurvePoint.RightTangentPoint.Length = 2;

		FCurvePoint endCurvePoint{};
		endCurvePoint.Point = { 1,0 };
		endCurvePoint.LeftTangentPoint.AngleInDegrees = 60;
		endCurvePoint.LeftTangentPoint.Length = 2;
		receiveDamage.ChangeInMagnitude.Curve.push_back(beginCurvePoint);
		receiveDamage.ChangeInMagnitude.Curve.push_back(endCurvePoint);
		receiveDamage.ChangeInMagnitude.CurveBehavior = EValueBehavior::Clamp;
		return receiveDamage;
	}
	FVectorMovementOverTime SMovementCreation::GetJumpMovement()
	{
		FVectorMovementOverTime playerJumpMovement{};
		playerJumpMovement.MovementDirection = { 0,1 };
		playerJumpMovement.BottomMagnitude = 1200;
		playerJumpMovement.NormalMagnitude = 2000;
		playerJumpMovement.TopMagnitude = 2700;
		playerJumpMovement.TimeOfMovement = 0.25;
		playerJumpMovement.MovementType = EMovementType::OverrideForce;
		playerJumpMovement.ApplyMovementOnXAxis = false;
		playerJumpMovement.ApplyMovementOnYAxis = true;
		playerJumpMovement.MovementTerminationType = EMovementTerminationType::TerminateAfterTimeTranspired;
		playerJumpMovement.ResetMovementCounterOnTermination = true;

		playerJumpMovement.OverrideMovementOnStart = true;
		playerJumpMovement.MovementTypeForStartOverride = EMovementType::OverrideVelocity;
		playerJumpMovement.MovementValueForOverrideOnStart = FPoint2f{ 0,0 };
		playerJumpMovement.ApplyStartMovementOnXAxis = false;
		playerJumpMovement.ApplyStartMovementOnYAxis = true;

		FCurvePoint beginCurvePoint{};
		beginCurvePoint.Point = { 0, 10 };
		beginCurvePoint.RightTangentPoint.AngleInDegrees = 0;// 120;
		beginCurvePoint.RightTangentPoint.Length = 2;
		FCurvePoint endCurvePoint{};
		endCurvePoint.Point = { 1,0 };
		endCurvePoint.LeftTangentPoint.AngleInDegrees = 60;
		endCurvePoint.LeftTangentPoint.Length = 2;
		playerJumpMovement.ChangeInMagnitude.Curve.push_back(beginCurvePoint);
		playerJumpMovement.ChangeInMagnitude.Curve.push_back(endCurvePoint);
		playerJumpMovement.ChangeInMagnitude.CurveBehavior = EValueBehavior::Clamp;
		return playerJumpMovement;
	}
}