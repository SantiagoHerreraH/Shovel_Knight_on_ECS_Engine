#include "../../../include/Systems/EventListenersSystems/UEventListenerSystems.h"


namespace Pillar {

	UEventListenerSystems::UEventListenerSystems(URegistry& registry) :
		m_VectorMove(registry),
		m_VectorMoveOverTime(registry){}

	void UEventListenerSystems::Update(float deltaSeconds)
	{
		m_VectorMove.Update();
		m_VectorMoveOverTime.Update(deltaSeconds);
	}

}

