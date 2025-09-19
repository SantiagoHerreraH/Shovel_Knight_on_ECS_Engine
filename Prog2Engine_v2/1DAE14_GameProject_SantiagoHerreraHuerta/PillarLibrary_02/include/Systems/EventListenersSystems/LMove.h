#pragma once

#include "../../../include/Systems/EventSystems/UEventManager.h"
#include "../../../include/Structs/FMovement.h"

namespace Pillar {

	//The entities are removed from the event listeners each tick ? <- Maybe 
	//when are entities removed from the triggered list

	//class LVectorMove : public UEventListener {

	//public: 
	//	LVectorMove(URegistry& registry);
	//	void Update(); //this needs to go after velocity and the forces has been calculated (maybe not)
	//};

	//class LVectorMoveOverTime : public UEventListener {
	//
	//public:
	//	LVectorMoveOverTime(URegistry& registry);
	//	void Update(float deltaSeconds);

	//private:
	//	FPoint2f CalculateVectorMovementOverTime(const FVectorMovementOverTime& movement);
	//};
}