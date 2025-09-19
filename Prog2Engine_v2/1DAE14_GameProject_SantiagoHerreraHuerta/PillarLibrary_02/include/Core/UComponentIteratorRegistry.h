#pragma once

#include "../../include/Core/UComponentIteratorManager.h"

namespace Pillar {

	class UComponentIteratorRegistry {

	public:
		UComponentIteratorRegistry(UArchetypeRegistry& archetypeRegistry);
		~UComponentIteratorRegistry() { m_TypeIndex_To_ComponentIteratorManager.clear(); }
		
		bool RefreshComponentIteratorsOfType(const std::type_index& type);

		void DeactivateArchetype(DArchetypeId id);
		void AddOrRefreshArchetype(DArchetypeId id);

		template <typename ComponentType>
		void RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, DArchetypeId& id, UComponentCompartment<ComponentType>* compartment);
		template <typename ComponentType>
		void RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, const UComponentFilter& filter, UComponentCompartment<ComponentType>* compartment);


	private:

		UArchetypeRegistry& m_ArchetypeRegistry;

		std::unordered_map<std::type_index, std::unique_ptr<IComponentIteratorManager>> m_TypeIndex_To_ComponentIteratorManager;

	};

#pragma region Implementations

	template<typename ComponentType>
	inline void UComponentIteratorRegistry::RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, DArchetypeId& archetypeId, UComponentCompartment<ComponentType>* compartment)
	{
		std::type_index id = typeid(ComponentType);

		if (!m_TypeIndex_To_ComponentIteratorManager.contains(id))
		{
			m_TypeIndex_To_ComponentIteratorManager.emplace(std::pair(id, std::make_unique<UComponentIteratorManager<ComponentType>>(m_ArchetypeRegistry, *compartment)));
		}

		UComponentIteratorManager<ComponentType>* manager = (UComponentIteratorManager<ComponentType>*)m_TypeIndex_To_ComponentIteratorManager.at(id).get();
		manager->RegisterComponentArray(componentArray, archetypeId);
	}

	template<typename ComponentType>
	inline void UComponentIteratorRegistry::RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, const UComponentFilter& filter, UComponentCompartment<ComponentType>* compartment)
	{
		std::type_index id = typeid(ComponentType);

		if (!m_TypeIndex_To_ComponentIteratorManager.contains(id))
		{
			m_TypeIndex_To_ComponentIteratorManager.emplace(std::pair(id, std::make_unique<UComponentIteratorManager<ComponentType>>(m_ArchetypeRegistry, *compartment)));
		}

		UComponentIteratorManager<ComponentType>* manager = (UComponentIteratorManager<ComponentType>*)m_TypeIndex_To_ComponentIteratorManager.at(id).get();
		manager->RegisterComponentArray(componentArray, filter);
	}

#pragma endregion
}

