#pragma once

#include <unordered_map>

#include "../../include/Core/FEntity.h"
#include "../../include/Core/UComponentFilter.h"
#include "../../include/Core/UInternalFilter.h"


/*
	Can probably do a function that is from archetypeId to archetype key
*/
namespace Pillar {

	class UArchetypeRegistry {
	public:
		UArchetypeRegistry(const int initialNumberOfComponents = 100);

		template<typename ComponentType>
		bool ArchetypeIdHasType(DArchetypeId id);
		template<typename ComponentType>
		bool FilterHasType(const UComponentFilter& filter);

		void Filter_To_ArchetypeIds(const UComponentFilter& filter, std::vector<DArchetypeId>& outArchetypeIds);
		bool ArchetypeId_To_Archetype(DArchetypeId id , DArchetype& outArchetype);
		bool Archetype_To_ArchetypeId(const DArchetype& archetype, DArchetypeId& outId);

		const bool IsTypeRegistered(const std::type_index& typeIndex);
		const bool RegisterType(const std::type_index& typeIndex); 
		const bool RegisterArchetype(const DArchetype& archetype);

		template<typename Component, typename ... ListOfComponents>
		void ComponentsToArchetypeId(DArchetypeId& outId, Component& component, ListOfComponents& ... components);
		template<typename Component, typename ... ListOfComponents>
		bool ComponentsToArchetypeId(const DArchetype& typesToOmit, DArchetypeId& outId, Component& component, ListOfComponents& ... components);
		DEntityId GetNewEntityId(DArchetypeId archetype);
		DHashId GetHashId(const FEntity& entity);

		void Archetype_To_ArchetypeKey(const DArchetype& archetype, DArchetypeKey& outKey, bool& isNewArchetype); //do the bool this way for it to not be confusing
		void Archetype_To_ArchetypeKey(const DArchetype& archetype, DArchetypeKey& outKey); 
		bool KeyPassesInternalFilter(const UInternalFilter& filter, const DArchetypeKey& key);

	private:

		const bool RegisterArchetypeKey(const DArchetypeKey& key, const DArchetype& archetype);

		bool KeyPassesIncludingFilter(const DArchetypeKey& includingFilter, const DArchetypeKey& key);
		bool KeyPassesExcludingFilter(const DArchetypeKey& excludingFilter, const DArchetypeKey& key);

		void ComponentsToArchetypeId(DArchetypeId& outId);								 
		bool ComponentsToArchetypeId(const DArchetype& typesToOmit, DArchetypeId& outId);

		void FilterToArchetypeKeys(const UComponentFilter& filter, std::vector<DArchetypeKey>& outArchetypeKeys);

		const DComponentId GetComponentId(const std::type_index& typeIndex);
		const DComponentId NumOfComponentsRegistered() { return m_CurrentComponentId - 1; }
		
		const bool IsArchetypeKeyRegistered(const DArchetypeKey& archetypeKey);
		const DArchetypeId GetArchetypeId(const DArchetypeKey& archetypeKey);
		const DArchetypeId NumOfArchetypesRegistered() { return m_CurrentArchetypeId - 1; }

		void ResizeAllArchetypeKeys();

		int m_CurrentMaxNumberOfComponents{};

		std::unordered_map<std::type_index, DComponentId> m_TypeIndex_To_ComponentId;
		DComponentId m_CurrentComponentId{1}; // leave it in one
		const DComponentId m_DEFAULT_COMPONENT_ID{0};

		std::vector<DArchetypeKey> m_ArchetypeKeys;//not ordered
		std::unordered_map<DArchetypeKey, DArchetypeId> m_ArchetypeKey_To_ArchetypeId;
		std::unordered_map<DArchetypeId, DArchetype> m_ArchetypeId_To_Archetype;
		DArchetypeId m_CurrentArchetypeId{1}; // leave it in one
		const DArchetypeId m_DEFAULT_ARCHETYPE_ID{0};

		std::unordered_map<DArchetypeId, std::unordered_map<DEntityId, DHashId>> m_FEntity_To_HashId;
		DHashId m_CurrentHashId{0};
		const DHashId m_DEFAULT_HASH_ID{ 0 };

		std::unordered_map<DArchetypeId, DEntityId> m_ArchetypeId_To_EntityId; // for creation
		const DEntityId m_DEFAULT_ENTITY_ID{ 0 };

		DArchetype m_ArchetypeForOperations;
	};

	template<typename Component, typename ...ListOfComponents>
	inline void UArchetypeRegistry::ComponentsToArchetypeId(DArchetypeId& outId, Component& component, ListOfComponents & ...components)
	{
		std::type_index id = typeid(component);
		RegisterType(id);
		m_ArchetypeForOperations.emplace(id);

		ComponentsToArchetypeId(outId, components...);
	}

	template<typename Component, typename ...ListOfComponents>
	inline bool UArchetypeRegistry::ComponentsToArchetypeId(const DArchetype& typesToOmit, DArchetypeId& outId, Component& component, ListOfComponents & ...components)
	{
		
		std::type_index id = typeid(component);
		RegisterType(id);

		if (typesToOmit.contains(id))
		{
			m_ArchetypeForOperations.clear();
			return false;
		}
		
		m_ArchetypeForOperations.emplace(id);

		return ComponentsToArchetypeId(typesToOmit, outId, components...);;
	}

#pragma region HasType

	template<typename ComponentType>
	inline bool UArchetypeRegistry::ArchetypeIdHasType(DArchetypeId id)
	{
		std::type_index typeId{ typeid(ComponentType) };

		if (m_ArchetypeId_To_Archetype.contains(id))
		{
			return m_ArchetypeId_To_Archetype.at(id).contains(typeId);
		}
		return false;
	}

	template<typename ComponentType>
	inline bool UArchetypeRegistry::FilterHasType(const UComponentFilter& filter)
	{
		DArchetypeKey keyIncluding;
		DArchetypeKey keyExcluding;

		Archetype_To_ArchetypeKey(filter.GetConstIncludingFilter(), keyIncluding);
		Archetype_To_ArchetypeKey(filter.GetConstExcludingFilter(), keyExcluding);

		std::type_index id{ typeid(ComponentType) };
		DComponentId compId{ GetComponentId(id) };

		if (keyIncluding[compId - 1] && !keyExcluding[compId - 1])
		{
			return true;
		}
		
		return false;
	}

#pragma endregion

}