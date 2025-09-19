#pragma once

#include <typeindex>

#include "../../include/Core/FEntity.h"

namespace Pillar {

	class IComponentIterator {
		virtual void Destroy() = 0;
	};

	class IComponentIteratorManager {

	public:

		virtual void RefreshAllComponentIterators() = 0;
		virtual void DeactivateArchetype(DArchetypeId id) = 0;
		virtual void AddOrRefreshArchetype(DArchetypeId id) = 0;
		virtual bool DestroyComponentIterator(IComponentIterator* arrayPtr) = 0;

	};
}
