
#include "../../include/Core/UEntityHandleRegistry.h"

namespace Pillar {

	void UEntityHandleRegistry::RegisterOrUpdateEntityHandle(UEntityHandle& entityHandle, const FEntity& entity, DHashId hashId)
	{
		if (!m_ArchetypeId_To_EntityHandleInformation.contains(entity.Archetype_Id))
		{
			m_ArchetypeId_To_EntityHandleInformation.emplace(std::pair(entity.Archetype_Id, FEntityHandleInformation{}));
		}

		if (!m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.contains(entity.Entity_Id))
		{
			m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).
				EntityId_To_EntityHandleRegistryContainer.emplace(std::pair(entity.Entity_Id, FEntityHandleRegistryContainers{}));
		}
		
		bool registeredBefore = m_ArchetypeId_To_EntityHandleInformation
			.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer
			.at(entity.Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.contains(&entityHandle);

		bool containsRegisteredEntity = entityHandle.IsValid() ? m_ArchetypeId_To_EntityHandleInformation
			.at(entityHandle.GetConstEntity().Archetype_Id).EntityId_To_EntityHandleRegistryContainer
			.at(entityHandle.GetConstEntity().Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.contains(&entityHandle) : false;

		if (!registeredBefore || !containsRegisteredEntity)
		{
			m_EntityHandleModifier.SetEntityInformation(entityHandle, this, entity, hashId);

			m_ArchetypeId_To_EntityHandleInformation.
				at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.
				at(entity.Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.
				emplace(std::pair(&entityHandle, std::unordered_set<UEntityHandleContainer*>{}));
		}
		else
		{
			DeregisterEntityHandle(entityHandle);
			RegisterOrUpdateEntityHandle(entityHandle, entity, hashId);
		}
	}
	void UEntityHandleRegistry::ChangeRegistratedEntity(const FEntity& entityToChange, const FEntity& newEntity, DHashId newHashId)
	{
		//can be optimized
		
		if (m_ArchetypeId_To_EntityHandleInformation.contains(entityToChange.Archetype_Id))
		{
			if (m_ArchetypeId_To_EntityHandleInformation.at(entityToChange.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.contains(entityToChange.Entity_Id))
			{
				std::unordered_map<UEntityHandle*, std::unordered_set<UEntityHandleContainer*>>& map = m_ArchetypeId_To_EntityHandleInformation.at(entityToChange.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.at(entityToChange.Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs;
			
				DHashId oldHashId{};

				for (auto It = map.begin(); It != map.end(); It++)
				{
					oldHashId = (*It).first->GetHashId();
					RegisterOrUpdateEntityHandle(*(*It).first, newEntity, newHashId);

					for (auto containerIt = (*It).second.begin(); containerIt != (*It).second.end(); containerIt++)
					{
						m_EntityHandleContainerModifier.ReHash((*containerIt), oldHashId, newHashId );
						BindEntityContainerToEntity((*containerIt), *(*It).first);
					}
				}

				m_ArchetypeId_To_EntityHandleInformation.at(entityToChange.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.erase(entityToChange.Entity_Id);

			}
		}

		
	}
	void UEntityHandleRegistry::DeregisterEntityHandle(UEntityHandle& entityHandle)
	{
		if (m_ArchetypeId_To_EntityHandleInformation.contains(entityHandle.GetConstEntity().Archetype_Id))
		{
			if (m_ArchetypeId_To_EntityHandleInformation.at(entityHandle.GetConstEntity().Archetype_Id).EntityId_To_EntityHandleRegistryContainer.contains(entityHandle.GetConstEntity().Entity_Id))
			{
				
				m_ArchetypeId_To_EntityHandleInformation.at(entityHandle.GetConstEntity().Archetype_Id).EntityId_To_EntityHandleRegistryContainer.at(entityHandle.GetConstEntity().Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.erase(&entityHandle);
				
			}
		}
	}
	void UEntityHandleRegistry::DeregisterEntity(const FEntity& entity)
	{
		if (m_ArchetypeId_To_EntityHandleInformation.contains(entity.Archetype_Id))
		{
			if (m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.contains(entity.Entity_Id))
			{
				auto end = m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.at(entity.Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.end();
				auto begin = m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.at(entity.Entity_Id).EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.begin();

				for (auto handleIt = begin; handleIt != end ; handleIt++)
				{
					m_EntityHandleModifier.DeleteEntity(*handleIt->first);
				}

				m_ArchetypeId_To_EntityHandleInformation.at(entity.Archetype_Id).EntityId_To_EntityHandleRegistryContainer.erase(entity.Entity_Id);

			}
		}
		
	}
	void UEntityHandleRegistry::BindEntityContainerToEntity(UEntityHandleContainer* container, UEntityHandle& entityHandle)
	{
		if (m_ArchetypeId_To_EntityHandleInformation.contains(entityHandle.GetConstEntity().Archetype_Id))
		{
			if (m_ArchetypeId_To_EntityHandleInformation.
				at(entityHandle.GetConstEntity().Archetype_Id).
				EntityId_To_EntityHandleRegistryContainer.
				contains(entityHandle.GetConstEntity().Entity_Id))
			{
				m_ArchetypeId_To_EntityHandleInformation.at(entityHandle.GetConstEntity().Archetype_Id).
				EntityId_To_EntityHandleRegistryContainer.at(entityHandle.GetConstEntity().Entity_Id).
				EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.at(&entityHandle).emplace(container);

			}
		}
	}
	void UEntityHandleRegistry::UnbindEntityContainerFromEntity(UEntityHandleContainer* container, UEntityHandle& entityHandle)
	{
		if (m_ArchetypeId_To_EntityHandleInformation.contains(entityHandle.GetConstEntity().Archetype_Id))
		{
			if (m_ArchetypeId_To_EntityHandleInformation.
				at(entityHandle.GetConstEntity().Archetype_Id).
				EntityId_To_EntityHandleRegistryContainer.
				contains(entityHandle.GetConstEntity().Entity_Id))
			{
				m_ArchetypeId_To_EntityHandleInformation.at(entityHandle.GetConstEntity().Archetype_Id).
					EntityId_To_EntityHandleRegistryContainer.at(entityHandle.GetConstEntity().Entity_Id).
					EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs.at(&entityHandle).erase(container);

			}
		}
	}
}
