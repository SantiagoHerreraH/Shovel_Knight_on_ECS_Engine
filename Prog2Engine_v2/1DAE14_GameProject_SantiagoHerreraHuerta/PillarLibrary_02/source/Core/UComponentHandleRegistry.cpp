
#include "../../include/Core/UComponentHandleRegistry.h"

namespace Pillar {
	UComponentHandleRegistry::UComponentHandleRegistry(UArchetypeRegistry& archetypeRegistry) :
		m_ArchetypeRegistry(archetypeRegistry)
	{
	}
	void UComponentHandleRegistry::RefreshEntity(const FEntity& entity)
	{
		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);

		for (auto typeIt = archetype.begin();  typeIt != archetype.end();  typeIt++)
		{
			if (m_TypeIndex_To_ComponentHandleManager.contains((*typeIt)))
			{
				m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshEntity(entity);
			}
		}
	}
	void UComponentHandleRegistry::ChangeEntity(const FEntity& oldEntity, const FEntity& newEntity)
	{
		DArchetype newArchetype;
		DArchetype oldArchetype;

		m_ArchetypeRegistry.ArchetypeId_To_Archetype(oldEntity.Archetype_Id, oldArchetype);
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(newEntity.Archetype_Id, newArchetype);

		if (oldArchetype.size() > newArchetype.size()) //if removed component
		{
			for (auto typeIt = oldArchetype.begin(); typeIt != oldArchetype.end(); typeIt++)
			{
				if (m_TypeIndex_To_ComponentHandleManager.contains((*typeIt)))
				{
					if (newArchetype.contains((*typeIt)))
					{
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->ChangeEntity(oldEntity, newEntity);
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshAllComponentHandles(); //not optimal at all, but for now this is enough
					}
					else
					{
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->DeleteEntity(oldEntity);
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshAllComponentHandles();
					}
				}
				
			}
		}
		else if(oldArchetype.size() < newArchetype.size()) //comp added (no option for same size)
		{
			for (auto typeIt = newArchetype.begin(); typeIt != newArchetype.end(); typeIt++)
			{
				if (m_TypeIndex_To_ComponentHandleManager.contains((*typeIt)))
				{
					if (oldArchetype.contains((*typeIt)))
					{
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->ChangeEntity(oldEntity, newEntity);
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshAllComponentHandles();
					}
					else
					{
						m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshAllComponentHandles();
					}
				}
			}
		}

		
	}
	void UComponentHandleRegistry::RefreshArchetype(DArchetypeId archetypeId)
	{
		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(archetypeId, archetype);

		for (auto typeIt = archetype.begin(); typeIt != archetype.end(); typeIt++)
		{
			if (m_TypeIndex_To_ComponentHandleManager.contains((*typeIt)))
			{
				m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->RefreshArchetype(archetypeId);
			}
		}
	}
	void UComponentHandleRegistry::DeleteEntity(const FEntity& entity)
	{
		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);

		for (auto typeIt = archetype.begin(); typeIt != archetype.end(); typeIt++)
		{
			if (m_TypeIndex_To_ComponentHandleManager.contains((*typeIt)))
			{
				m_TypeIndex_To_ComponentHandleManager.at((*typeIt)).get()->DeleteEntity(entity);
			}
		}
	}
}