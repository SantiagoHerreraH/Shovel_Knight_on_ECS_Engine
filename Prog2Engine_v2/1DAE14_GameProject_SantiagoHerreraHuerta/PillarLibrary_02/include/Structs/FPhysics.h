#pragma once

#include <string>
#include <unordered_map>

#include "../../include/Structs/FVector.h"
#include "../../include/Structs/FMovement.h"
#include "../../include/Systems/TemplateSystems/TActivatableVector.h"

namespace Pillar {

	struct FStickyInformation {
		float MultipleOnVelocity{1.f};
	};

	struct FSticky {
		TActivatableFlatMap<std::string, FStickyInformation> Tag_To_StickyInformation;
	};

	struct FEnvironmentForces {
		TActivatableVector<FMovement> ForcesToApplyPerTick;
	};

	struct FNetForce2D {
		FPoint2f NetForcePerTick;
	};

	struct FMass {
		float Kilograms{1.f};
	};

	struct FVelocity {
		FPoint2f DistancePerSecond;
	};

	struct FAngularVelocity {
		double DegreesPerSecond;
	};

	struct FNetTorque2D {
		double NetTorquePerTick;
	};

	struct FFrictionCoefficient {
		float Value;
	};

	struct FDensity {
		float Value;
	};


	//so instead of making gravity a component, make systems that play with different environmental forces
	//struct FGravity {
	//	FPoint2f Value; // 
	//};

	//force/mass * delta seconds squared = position 
	//force/mass * delta seconds = velocity
	// I think the maximum normal force an object can exert is gravity times mass
	//kinetic friction = coefficient of kinetic friction * normal force -> meaning, the harder the object is pressed to the ground, the more friction (molecules intertwine more)
	//my theory is that the max normal force = the objects mass
	//I think it has to do with the normal force an object return upon collision
	//quantity of motion = mass * velocity
	// change in object's quantity of motion is a force
	//force = mass * acceleration
	//momentum = mass * velocity = force * seconds
	//impulse or force = momentum / seconds
	//collisions can be elastic or inelastic
	//elastic -> all kinetic energy gets transferred -> collided target conserves velocity of entity that started he collision
	//inelastic -> what happens when objects stick together
	//momentum is always conserved in a system (think about energy) -> this means -> mass * velocity (before) = mass * velocity (after) 
	//center of mass = average position of all of mass in a system
	// each individual mass * distance from start point / total mass
	//Two kinds of friction:
	//kinetic friction = force that slows stuff down as it slides
	//static friction = force to overcome to make things move in the first place
	//more roughness = more friction -> 
	//roughness -> coefficient of kinetic friction
	//how hard the materials are pressed together puts more of their surfaces in contact with each other
	//friction is a vector
	//kinetic friction = coeficient of kinetic friction (depends on material) * Normal force(force that the contact surface returns to the object that is pressing down)
	//max static friction = coeficient of static friction * normal force

}

