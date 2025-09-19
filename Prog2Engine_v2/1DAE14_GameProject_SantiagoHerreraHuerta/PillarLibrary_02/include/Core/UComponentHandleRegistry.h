#pragma once

#include <typeindex>
#include <unordered_map>
#include <memory>

#include "../../include/Core/UArchetypeRegistry.h"
#include "../../include/Core/UComponentHandleManager.h"

namespace Pillar {

	class UComponentHandleRegistry {

	public:
		UComponentHandleRegistry(UArchetypeRegistry& archetypeRegistry);

		template<typename ComponentType>
		void RefreshType(); // this needs to be called after the entity handles are updated // called when new Archetype is added
		
		void RefreshEntity(const FEntity& entity) ; // call this when activate/ deactivate
		void ChangeEntity(const FEntity& oldEntity, const FEntity& newEntity); // call this when add/remove component
		void RefreshArchetype(DArchetypeId archetypeId); // call this when a new entity is created in an archetype
		
		void DeleteEntity(const FEntity& entity); //call this when destroying an entity

		template<typename ComponentType>
		void RegisterComponentHandle(UComponentHandle<ComponentType>* componentHandle, const UEntityHandle& entityHandle, UComponentCompartment<ComponentType>* compartment); //upon creation, after filling the entity handle
		

		template<typename ComponentType>
		void Clear(); //call this when clear component type

	private:
		UArchetypeRegistry& m_ArchetypeRegistry;

		std::unordered_map<std::type_index, std::unique_ptr<IComponentHandleManager>> m_TypeIndex_To_ComponentHandleManager;

	};

	template<typename ComponentType>
	inline void UComponentHandleRegistry::RefreshType()
	{
		std::type_index id{ typeid(ComponentType) };
		if (m_TypeIndex_To_ComponentHandleManager.contains(id))
		{
			m_TypeIndex_To_ComponentHandleManager.at(id).get()->RefreshAllComponentHandles();
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleRegistry::RegisterComponentHandle(UComponentHandle<ComponentType>* componentHandle, const UEntityHandle& entityHandle, UComponentCompartment<ComponentType>* compartment)
	{
		std::type_index id{ typeid(ComponentType) };
		if (!m_TypeIndex_To_ComponentHandleManager.contains(id))
		{
			m_TypeIndex_To_ComponentHandleManager.emplace(std::pair(id, std::make_unique<UComponentHandleManager<ComponentType>>(compartment)));
		}
		UComponentHandleManager<ComponentType>* manager = (UComponentHandleManager<ComponentType>*)m_TypeIndex_To_ComponentHandleManager.at(id).get();
		manager->RegisterComponentHandle(componentHandle, entityHandle);
	}

	template<typename ComponentType>
	inline void UComponentHandleRegistry::Clear()
	{
		std::type_index id{ typeid(ComponentType) };
		if (m_TypeIndex_To_ComponentHandleManager.contains(id))
		{
			m_TypeIndex_To_ComponentHandleManager.at(id).get()->Clear();
		}
	}

}