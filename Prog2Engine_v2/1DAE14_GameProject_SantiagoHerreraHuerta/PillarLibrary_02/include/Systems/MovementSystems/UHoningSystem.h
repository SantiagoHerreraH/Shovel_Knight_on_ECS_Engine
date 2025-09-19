#pragma once

#include "../../Core/URegistry.h"
#include "../../Systems/TransformSystems/URealTransform2D.h"
#include "../../Structs/FMovement.h"
#include "../../Structs/FPhysics.h"
#include "../../Systems/MovementSystems/UMovementData.h"


namespace Pillar {

	class UCalculateTargetInformation final{

	public:

		UCalculateTargetInformation(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;

		UComponentIterator<FTarget> m_Targets;
		UComponentIterator<URealTransform2D> m_RealTransforms;
	};

	class UExecuteSimpleHomingAction final{

	public:
		UExecuteSimpleHomingAction(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;

		UComponentIterator<FHomingInformation>			m_HomingInfo;
		UComponentIterator<FTarget>						m_Targets;
		UComponentIterator<FVelocity>					m_Velocities;
		UComponentIterator<FNetForce2D>					m_Forces;

	};

	class UExecuteSimpleLookAtRotation final {

	public:
		UExecuteSimpleLookAtRotation(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;

		UComponentIterator<FLookAtRotationInfo>			m_LookAtRotationInfo;
		UComponentIterator<FTarget>						m_Targets;
		UComponentIterator<FWorldTransform2D>			m_WorldTransforms;

	};
	
	/*
	
	Movement system in general
	- Calculate any necessary info to determine movement
	- Calculate movement based on info retrieved
	- Set Movement based on calculated info
	
	*/
}