
#include "../../include/Core/UArchetypeRegistry.h"


namespace Pillar {

#pragma region Constructor

	UArchetypeRegistry::UArchetypeRegistry(const int initialNumberOfComponents) :
		m_CurrentMaxNumberOfComponents(initialNumberOfComponents)
	{
	}

#pragma endregion

#pragma region ComponentDefinitions

#pragma region Component

	const bool UArchetypeRegistry::IsTypeRegistered(const std::type_index& typeIndex)
	{
		return m_TypeIndex_To_ComponentId.contains(typeIndex);
	}

	const bool UArchetypeRegistry::RegisterType(const std::type_index& typeIndex)
	{
		if (m_TypeIndex_To_ComponentId.contains(typeIndex))
		{
			return false;
		}

		m_TypeIndex_To_ComponentId.emplace(std::pair(typeIndex, m_CurrentComponentId));
		++m_CurrentComponentId;

		return true;
	}

	const DComponentId UArchetypeRegistry::GetComponentId(const std::type_index& typeIndex)
	{
		if (!m_TypeIndex_To_ComponentId.contains(typeIndex))
		{
			return m_DEFAULT_COMPONENT_ID;
		}

		return m_TypeIndex_To_ComponentId.at(typeIndex);
	}

#pragma endregion

#pragma region Archetype

	const bool UArchetypeRegistry::IsArchetypeKeyRegistered(const DArchetypeKey& archetypeKey)
	{
		return m_ArchetypeKey_To_ArchetypeId.contains(archetypeKey);
	}

	const bool UArchetypeRegistry::RegisterArchetype(const DArchetype& archetype)
	{
		DArchetypeKey key;
		bool isNewArchetype{};
		Archetype_To_ArchetypeKey(archetype, key, isNewArchetype);
		return isNewArchetype;
	}

	DEntityId UArchetypeRegistry::GetNewEntityId(DArchetypeId archetypeId)
	{
		if (!m_ArchetypeId_To_EntityId.contains(archetypeId))
		{
			//debug message
			return m_DEFAULT_ENTITY_ID;
		}

		++m_CurrentHashId;
		++m_ArchetypeId_To_EntityId.at(archetypeId);

		m_FEntity_To_HashId.emplace(std::pair(archetypeId, std::unordered_map<DEntityId, DHashId>{}));
		m_FEntity_To_HashId.at(archetypeId).emplace(std::pair(m_ArchetypeId_To_EntityId.at(archetypeId), m_CurrentHashId));

		return m_ArchetypeId_To_EntityId.at(archetypeId);
	}

	DHashId UArchetypeRegistry::GetHashId(const FEntity& entity)
	{
		if (!m_FEntity_To_HashId.contains(entity.Archetype_Id))
		{
			return m_DEFAULT_HASH_ID;
		}
		if (!m_FEntity_To_HashId.at(entity.Archetype_Id).contains(entity.Entity_Id))
		{
			return m_DEFAULT_HASH_ID;
		}

		return m_FEntity_To_HashId.at(entity.Archetype_Id).at(entity.Entity_Id);
	}

	const DArchetypeId UArchetypeRegistry::GetArchetypeId(const DArchetypeKey& archetypeKey)
	{
		if (!m_ArchetypeKey_To_ArchetypeId.contains(archetypeKey))
		{
			return m_DEFAULT_ARCHETYPE_ID;
		}

		return m_ArchetypeKey_To_ArchetypeId.at(archetypeKey);
	}

	void UArchetypeRegistry::ResizeAllArchetypeKeys()
	{
		int oldComponentNum = m_CurrentMaxNumberOfComponents;
		m_CurrentMaxNumberOfComponents *= 2;

		std::unordered_map<DArchetypeKey, DArchetypeId> tempMap;
		tempMap.reserve(m_ArchetypeKey_To_ArchetypeId.size());

		DArchetypeKey key;
		key.resize(m_CurrentMaxNumberOfComponents);

		int counter{0};
		for (auto It = m_ArchetypeKey_To_ArchetypeId.begin(); It != m_ArchetypeKey_To_ArchetypeId.end(); It++)
		{
			for (size_t i = 0; i < (*It).first.size(); i++)
			{
				key[i] = (*It).first.at(i);
			}

			tempMap.emplace(std::pair(key, (*It).second));
			m_ArchetypeKeys[counter] = key; //should be okay
			key.assign(oldComponentNum, false);

			++counter;
		}

		m_ArchetypeKey_To_ArchetypeId.swap(tempMap);
		tempMap.clear();
	}

#pragma endregion

#pragma region Filter To ...


	void UArchetypeRegistry::Filter_To_ArchetypeIds(const UComponentFilter& filter, std::vector<DArchetypeId>& outArchetypeIds)
	{
		std::vector<DArchetypeKey> archetypeKeys;
		FilterToArchetypeKeys(filter, archetypeKeys);

		outArchetypeIds.clear();
		outArchetypeIds.reserve(archetypeKeys.size());

		for (size_t i = 0; i < archetypeKeys.size(); i++)
		{
			outArchetypeIds.emplace_back(m_ArchetypeKey_To_ArchetypeId.at(archetypeKeys[i]));
			
		}
	}

	bool UArchetypeRegistry::ArchetypeId_To_Archetype(DArchetypeId id, DArchetype& outArchetype)
	{
		if (m_ArchetypeId_To_Archetype.contains(id))
		{
			outArchetype = m_ArchetypeId_To_Archetype.at(id);

			return true;
		}

		return false;
	}

	bool UArchetypeRegistry::Archetype_To_ArchetypeId(const DArchetype& archetype, DArchetypeId& outId)
	{
		DArchetypeKey archetypeKey;
		Archetype_To_ArchetypeKey(archetype, archetypeKey);

		if (m_ArchetypeKey_To_ArchetypeId.contains(archetypeKey))
		{
			outId = m_ArchetypeKey_To_ArchetypeId.at(archetypeKey);

			return true;
		}

		outId = 0;
		//debug message
		return false;
	}

	void UArchetypeRegistry::ComponentsToArchetypeId(DArchetypeId& outId)
	{
		DArchetypeKey key;
		bool isNewArchetype{};
		Archetype_To_ArchetypeKey(m_ArchetypeForOperations, key, isNewArchetype);
		outId = GetArchetypeId(key);

		m_ArchetypeForOperations.clear();
	}

	bool UArchetypeRegistry::ComponentsToArchetypeId(const DArchetype& typesToOmit, DArchetypeId& outId) //whats going on with this functions?
	{
		DArchetypeKey key;
		Archetype_To_ArchetypeKey(m_ArchetypeForOperations, key);
		outId = GetArchetypeId(key);

		m_ArchetypeForOperations.clear();

		return true;
	}
	
	void UArchetypeRegistry::FilterToArchetypeKeys(const UComponentFilter& filter, std::vector<DArchetypeKey>& outArchetypeKeys)
	{
		DArchetypeKey excludingFilter;
		DArchetypeKey includingFilter;

		
		Archetype_To_ArchetypeKey(filter.GetConstExcludingFilter(), excludingFilter);
		Archetype_To_ArchetypeKey(filter.GetConstIncludingFilter(), includingFilter);

		outArchetypeKeys.clear();

		outArchetypeKeys.reserve(NumOfArchetypesRegistered());

		for (size_t i = 0; i < m_ArchetypeKeys.size(); i++)
		{

			if (KeyPassesIncludingFilter(includingFilter, m_ArchetypeKeys[i]))
			{
				if (KeyPassesExcludingFilter(excludingFilter, m_ArchetypeKeys[i]))
				{
					outArchetypeKeys.emplace_back(m_ArchetypeKeys[i]);
				}
			}
		}
	}

	void UArchetypeRegistry::Archetype_To_ArchetypeKey(const DArchetype& archetype, DArchetypeKey& outKey, bool& isNewArchetype)
	{

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			UArchetypeRegistry::RegisterType((*It));
		}

		if (NumOfComponentsRegistered() > m_CurrentMaxNumberOfComponents )
		{
			ResizeAllArchetypeKeys();
		}

		outKey.resize(m_CurrentMaxNumberOfComponents);
		bool defaultState = false;
		outKey.assign(outKey.size(), defaultState);

		DComponentId id;


		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{

			id = UArchetypeRegistry::GetComponentId((*It));

			//Probably do a check before accessing this
			outKey[id - 1] = true;
		}

		isNewArchetype = RegisterArchetypeKey(outKey, archetype);
	}

	void UArchetypeRegistry::Archetype_To_ArchetypeKey(const DArchetype& archetype, DArchetypeKey& outKey)
	{
		bool isNewArchetype{};
		Archetype_To_ArchetypeKey(archetype, outKey, isNewArchetype);
	}


#pragma endregion

#pragma endregion

	const bool UArchetypeRegistry::RegisterArchetypeKey(const DArchetypeKey& key, const DArchetype& archetype)
	{
		if (m_ArchetypeKey_To_ArchetypeId.contains(key))
		{
			return false;
		}

		m_ArchetypeKey_To_ArchetypeId.emplace(std::pair(key, m_CurrentArchetypeId));
		m_ArchetypeId_To_EntityId.emplace(std::pair(m_CurrentArchetypeId, 0));
		m_ArchetypeId_To_Archetype.emplace(std::pair(m_CurrentArchetypeId, archetype));
		++m_CurrentArchetypeId;
		m_ArchetypeKeys.emplace_back(key);

		return true;
	}

	bool UArchetypeRegistry::KeyPassesIncludingFilter(const DArchetypeKey& includingFilter, const DArchetypeKey& key)
	{
		bool passedConditions{ false };

		size_t smallerSizeInIncluding =
			key.size() < includingFilter.size() ?
			key.size() : includingFilter.size();


		for (size_t j = 0; j < smallerSizeInIncluding; j++)
		{
			if (key[j] && includingFilter[j])
			{
				passedConditions = true;

			}
			if (!key[j] && includingFilter[j])
			{
				passedConditions = false;
				break;
			}
		}

		if (passedConditions && key.size() < includingFilter.size())
		{

			for (size_t j = smallerSizeInIncluding; j < includingFilter.size(); j++)
			{
				if (includingFilter[j])
				{
					passedConditions = false;
				}
			}
		}

		return passedConditions;
	}

	bool UArchetypeRegistry::KeyPassesExcludingFilter(const DArchetypeKey& excludingFilter, const DArchetypeKey& key)
	{
		bool passedConditions{ true };

		size_t smallerSizeInExcluding =
			key.size() < excludingFilter.size() ?
			key.size() : excludingFilter.size();

		
		for (size_t j = 0; j < smallerSizeInExcluding; j++)
		{
			if (key[j] && excludingFilter[j])
			{
				passedConditions = false;
				break;
			}
		}
		

		return passedConditions;
	}

	bool UArchetypeRegistry::KeyPassesInternalFilter(const UInternalFilter& filter, const DArchetypeKey& key)
	{
		return 
			KeyPassesExcludingFilter(filter.GetConstExcludingFilter(), key) &&
			KeyPassesIncludingFilter(filter.GetConstIncludingFilter(), key);
	}


}

