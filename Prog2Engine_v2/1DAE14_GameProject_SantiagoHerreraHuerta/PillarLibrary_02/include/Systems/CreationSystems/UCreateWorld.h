#pragma once

#include "../../../include/Systems/CreationSystems/UCreateComponents.h"
#include "../../../include/Systems/CreationSystems/UCreateEntities.h"

namespace Pillar {

	class UCreateWorld {

	public:
		UCreateWorld(URegistry& registry);
		void Create();

	private:
		UCreateComponents m_CreateComponents;
		UCreateEntities m_CreateEntities;

	};
}