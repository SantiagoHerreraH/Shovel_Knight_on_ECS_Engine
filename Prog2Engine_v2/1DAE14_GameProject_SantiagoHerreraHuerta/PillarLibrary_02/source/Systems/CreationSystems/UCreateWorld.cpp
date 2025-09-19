#include "../../../include/Systems/CreationSystems/UCreateWorld.h"

namespace Pillar {
	UCreateWorld::UCreateWorld(URegistry& registry) :
		m_CreateComponents(registry),
		m_CreateEntities(registry){}

	void UCreateWorld::Create()
	{
		m_CreateComponents.Create();
		m_CreateEntities.CreateAndSetParams();
	}
}