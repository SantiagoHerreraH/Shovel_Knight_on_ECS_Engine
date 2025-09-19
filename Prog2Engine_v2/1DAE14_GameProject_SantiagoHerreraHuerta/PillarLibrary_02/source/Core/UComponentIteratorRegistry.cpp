
#include "../../include/Core/UComponentIteratorRegistry.h"

namespace Pillar {

	UComponentIteratorRegistry::UComponentIteratorRegistry(UArchetypeRegistry& archetypeRegistry) :
		m_ArchetypeRegistry(archetypeRegistry)
	{
	}

	bool UComponentIteratorRegistry::RefreshComponentIteratorsOfType(const std::type_index& type)
	{
		if (m_TypeIndex_To_ComponentIteratorManager.contains(type))
		{
			m_TypeIndex_To_ComponentIteratorManager.at(type).get()->RefreshAllComponentIterators();
			return true;
		}
		return false;
	}

	void UComponentIteratorRegistry::DeactivateArchetype(DArchetypeId id)
	{
		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(id, archetype);

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_TypeIndex_To_ComponentIteratorManager.contains((*It)))
			{
				m_TypeIndex_To_ComponentIteratorManager.at((*It)).get()->DeactivateArchetype(id);
			}
		}
	}

	void UComponentIteratorRegistry::AddOrRefreshArchetype(DArchetypeId id)
	{
		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(id, archetype);

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_TypeIndex_To_ComponentIteratorManager.contains((*It)))
			{
				m_TypeIndex_To_ComponentIteratorManager.at((*It)).get()->AddOrRefreshArchetype(id);
			}
		}
	}

}

