#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FPhysics.h"
#include "../../../include/Structs/FTransform.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Structs/FObjectCollision.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"

namespace Pillar {

	class UResetPhysicsVariables {
		
	public:
		UResetPhysicsVariables(URegistry& registry);
		void Reset();

	private:
		//only reset Forces (since forces aren't applied per tick, they are applied whenever and affect velocity, which is applied per tick)
		UComponentIterator<FNetForce2D> m_Forces;
	};

	class UApplyEnvironmentForce {

	public:
		UApplyEnvironmentForce(URegistry& registry);

		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FEnvironmentForces> m_EnvironmentForces;
		UComponentIterator<FNetForce2D> m_Forces;
		UComponentIterator<FVelocity> m_Velocities;
	};

	class UApplyForceToVelocity {

	public:
		UApplyForceToVelocity(URegistry& registry);

		void Update(float deltaSeconds);

	private:
		URegistry& m_Registry;

		UComponentIterator<FVelocity> m_Velocities;
		UComponentIterator<FNetForce2D> m_Forces;
		UComponentIterator<FMass> m_Masses;

	};

	class UApplyRectRestrictedTransformChanges {
	public:
		UApplyRectRestrictedTransformChanges(URegistry& registry);

		void Update(float deltaSeconds);

	private:

		UComponentIterator<FWorldTransform2D> m_Transforms; //was local transform before
		UComponentIterator<FVelocity> m_Velocities;//
		UComponentIterator<FRectCollider> m_RectColliders;
		//Put AngularVelocity here afterwards

	};

	class UApplyTransformChanges {
	public:
		UApplyTransformChanges(URegistry& registry);

		void Update(float deltaSeconds);

	private:

		UComponentIterator<FWorldTransform2D> m_Transforms; //was local transform before
		UComponentIterator<FVelocity> m_Velocities;//
		//Put AngularVelocity here afterwards

	};

	class UExecutePhysicsCollision {

	public:
		UExecutePhysicsCollision(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;

		UComponentIterator<FVelocity> m_Velocities;
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityCollisionInfo;
		UComponentIterator<ECollisionBodyType> m_CollisionBodyType;

	};

	class UExecuteRectPhysicsCollision {

	public:
		UExecuteRectPhysicsCollision(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;

		UComponentIterator<FVelocity> m_Velocities;
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityCollisionInfo;
		UComponentIterator<ECollisionBodyType> m_CollisionBodyType;
	};

	class UApplyStickiness {

	public:
		UApplyStickiness(URegistry& registry);
		void Update();

	private:

		void ApplySticky(float& velocityAxisToAffect, float stickyVelocityAxis);

		URegistry& m_Registry;

		UComponentIterator<FSticky> m_Stickies;
		UComponentIterator<FEntityResolveOverlapInfo> m_EntityCollisionInfo;
		UComponentIterator<FVelocity> m_Velocities;
	};

	class UPhysics {

	public:
		UPhysics(URegistry& registry);
		void Update(float deltaSeconds);

	private:

		UApplyEnvironmentForce m_ApplyEnvironmentForces;
		UApplyForceToVelocity m_ApplyForceToVel;
		UExecutePhysicsCollision m_ExecutePhysicsCollision;
		UExecuteRectPhysicsCollision m_ExecuteRectPhysicsCollision;
		UApplyStickiness m_ApplyStickiness;
		UApplyTransformChanges m_ApplyTransformChanges;
		UApplyRectRestrictedTransformChanges m_ApplyRectRestrictedTransformChanges;
		UResetPhysicsVariables m_ResetPhysicsVariables;
	};
}