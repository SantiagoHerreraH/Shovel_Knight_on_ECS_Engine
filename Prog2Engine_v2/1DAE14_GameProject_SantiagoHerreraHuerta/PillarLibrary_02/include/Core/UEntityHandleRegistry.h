#pragma once

#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <typeindex>

#include "../../include/Core//UEntityHandle.h"
#include "../../include/Core/UEntityHandleContainer.h"

namespace Pillar {

	struct FEntityHandleRegistryContainers {
		std::unordered_map<UEntityHandle*, std::unordered_set<UEntityHandleContainer*>>  EntityHandlePtrs_To_SetOf_EntityHandleContainerPtrs;
	};

	struct FEntityHandleInformation {
		std::unordered_map<DEntityId, FEntityHandleRegistryContainers> EntityId_To_EntityHandleRegistryContainer;
	};

	class UEntityHandleRegistry {

	public:
		UEntityHandleRegistry() {}
		void RegisterOrUpdateEntityHandle(UEntityHandle& entityHandle, const FEntity& entity, DHashId hashId);
		void ChangeRegistratedEntity(const FEntity& entityToChange, const FEntity& newEntity, DHashId newHashId);
		void DeregisterEntityHandle(UEntityHandle& entity);
		void DeregisterEntity(const FEntity& entity);

		void BindEntityContainerToEntity(UEntityHandleContainer* container, UEntityHandle& entityHandle);
		void UnbindEntityContainerFromEntity(UEntityHandleContainer* container, UEntityHandle& entityHandle);

	private:
		UEntityHandleModifier m_EntityHandleModifier;
		UEntityHandleContainerModifier m_EntityHandleContainerModifier;
		std::unordered_map< DArchetypeId, FEntityHandleInformation> m_ArchetypeId_To_EntityHandleInformation;
		
	};


}