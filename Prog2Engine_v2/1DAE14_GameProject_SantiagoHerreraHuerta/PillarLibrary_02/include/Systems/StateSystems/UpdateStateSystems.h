#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Structs/FVector.h"
#include "../../../include/Structs/FMovement.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FLife.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FSceneGraphStructure.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Systems/DrawSystems/UTexture.h"

namespace Pillar {

	using DStateController_VectorMovement			= UStateController<FVectorMovement,				true,  true>;
	using DStateController_VectorMovementOverTime	= UStateController<FVectorMovementOverTime,		false, true>;
	using DStateController_RotationTransformation	= UStateController<FRotationTransformation,		true,  true>;
	using DStateController_Activation				= UStateController<FActivationState,			false, true>;
	using DStateController_Transform				= UStateController<FTransformState,				false, true>;
	using DStateController_TargetChooser			= UStateController<FTargetsOfInterest,			false, true>;

	class SMovementCalculation {
	public:
		static void ApplyMovementType(
			EMovementType movementType, const FPoint2f& movement, FVelocity& velocityToModify,
			FNetForce2D& netForceToModify, bool applyMovementOnXAxis, bool applyMovementOnYAxis);
		static FPoint2f GetMovementDirectionBasedOnMovementType(URegistry* const registry, EMovementDirectionType movementDirectionType, const FEntity& entityToGetDirectionFrom, const FPoint2f& customMovement);
		static FPoint2f CalculateVectorMovementOverTime(const FVectorMovementOverTime& movement, const FPoint2f& movementDirection);
	};

	class UUpdateState_RotationTransformation {
	public:
		UUpdateState_RotationTransformation(URegistry& registry);
		void Update(); 

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FRotationTransformation>> m_RotationTransformationStates;
		UComponentIterator<FWorldTransform2D> m_WorldTransforms;
	};

	class UUpdateState_VectorMove final{

	public:
		UUpdateState_VectorMove(URegistry& registry);
		void Update(); 

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FVectorMovement>> m_VectorMovements;
		UComponentIterator<FVelocity> m_Velocities;
		UComponentIterator<FNetForce2D> m_NetForces;
	};

	class UUpdateState_VectorMoveOverTime final{

	public:
		UUpdateState_VectorMoveOverTime(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FVectorMovementOverTime>> m_VectorMovementsOverTime;
		UComponentIterator<FVelocity> m_Velocities;
		UComponentIterator<FNetForce2D> m_NetForces;
	};

	struct FEntityAndActivationAction {
		FEntityAndActivationAction() {}
		explicit FEntityAndActivationAction(const FEntity& entity, EEntityActivationAction action) : Entity(entity), EntityActivationAction(action){};

		FEntity Entity{};
		EEntityActivationAction EntityActivationAction{};
	};

	class UUpdateState_Activation {
	public:
		UUpdateState_Activation(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		URegistry& m_Registry;
		
		UComponentIterator<FSceneObject> m_SceneObjects;
		URegistrySingleton<FSceneRegistry> m_SceneRegistry;

		UComponentIterator<FState<FActivationState>> m_ActivationStates;
		UComponentIterator<UStateMachine> m_StateMachines;
		std::vector<FEntityAndActivationAction> m_EntitiesToChangeState{};
	};

	class UUpdateState_Transform {
	public:
		UUpdateState_Transform(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FTransformState>> m_TransformStates;
		UComponentIterator<FWorldTransform2D>		m_WorldTransforms;
		UComponentIterator<UStateMachine>			m_StateMachines;
	};

	class UUpdateState_Targets {
	public:
		UUpdateState_Targets(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FTargetsOfInterest>> m_TargetsOfInterest_States;
		UComponentIterator<FTarget>					   m_Targets;
		UComponentIterator<UStateMachine>				m_StateMachines;
	};

	/*
	template class
	class UpdateStateSetter

	template class
	class UpdateStateOverTime
	
	---> And then have some manager loop through all of these

	*/
}