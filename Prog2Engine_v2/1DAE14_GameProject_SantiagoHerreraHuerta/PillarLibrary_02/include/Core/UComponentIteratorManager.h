
#pragma once

#include "../../include/Core/UComponentIterator.h"
#include "../../include/Core/UArchetypeRegistry.h"

namespace Pillar {

	/*

This class makes sure the vector pointers of the arrays are never invalid

*/

	template<typename ComponentType>
	struct FComponentIteratorInfo {

		std::vector<DArchetypeId> ArchetypeIds;
		std::vector<size_t> Indexes;
		size_t IndexInBigVector;
	};


	/*
	
		Make sure to destroy everything 

	*/

	template<typename ComponentType>
	class UComponentIteratorManager : public IComponentIteratorManager {

	public:
		UComponentIteratorManager( UArchetypeRegistry& archetypeRegistry, UComponentCompartment<ComponentType>& compartment);
		~UComponentIteratorManager();

		void RefreshAllComponentIterators() override;
		void DeactivateArchetype(DArchetypeId id) override; 
		void AddOrRefreshArchetype(DArchetypeId id) override;
		bool DestroyComponentIterator(IComponentIterator* arrayPtr) override;

		void RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, DArchetypeId archetypeId);
		void RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, const UComponentFilter& filter);

	private:

		bool RefreshArchetype(DArchetypeId id);
		bool DestroyComponentArray(UComponentIterator<ComponentType>* componentArray);
		void Register(UComponentIterator<ComponentType>* componentArray, DArchetypeId archetypeId);
		void Register(UComponentIterator<ComponentType>* componentArray);

		UComponentIteratorModifier m_ComponentIteratorModifier;
		UArchetypeRegistry& m_ArchetypeRegistry;
		UComponentCompartment<ComponentType>& m_ComponentCompartment;

		std::unordered_map<DArchetypeId, std::vector<UComponentIterator<ComponentType>*>> m_ArchetypeId_To_ComponentIteratorPtrs;
		std::unordered_map< UComponentIterator<ComponentType>* , FComponentIteratorInfo<ComponentType>> m_ComponentArrays_To_ComponentArrayInfo;
		std::vector<UComponentIterator<ComponentType>*> m_ComponentIteratorPtrs;

	};

#pragma region Implementations

	template<typename ComponentType>
	inline UComponentIteratorManager<ComponentType>::UComponentIteratorManager(UArchetypeRegistry& archetypeRegistry, UComponentCompartment<ComponentType>& compartment) :
		m_ArchetypeRegistry(archetypeRegistry),
		m_ComponentCompartment(compartment)
	{
	}

	template<typename ComponentType>
	inline UComponentIteratorManager<ComponentType>::~UComponentIteratorManager()
	{
		//should have debug message -> if a component array manager is destroyed, all of its component arrays get cleared as well
		for (size_t i = 0; i < m_ComponentIteratorPtrs.size(); i++)
		{
			m_ComponentIteratorPtrs[i]->Destroy();
		}
		m_ArchetypeId_To_ComponentIteratorPtrs.clear();
		m_ComponentArrays_To_ComponentArrayInfo.clear();
		m_ComponentIteratorPtrs.clear();
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::RefreshAllComponentIterators()
	{
		for (size_t i = 0; i < m_ComponentIteratorPtrs.size(); i++)
		{
			m_ComponentIteratorModifier.RefreshIterator((*m_ComponentIteratorPtrs[i]));
		}
	}

	template<typename ComponentType>
	inline bool UComponentIteratorManager<ComponentType>::RefreshArchetype(DArchetypeId id)
	{
		if (m_ArchetypeId_To_ComponentIteratorPtrs.contains(id))
		{
			std::vector<UComponentIterator<ComponentType>*>& compArrays = m_ArchetypeId_To_ComponentIteratorPtrs.at(id);

			for (size_t i = 0; i < compArrays.size(); i++)
			{
				m_ComponentIteratorModifier.RefreshArrayPtr((*compArrays[i]), id);
			}

			return true;
		}

		return false;
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::DeactivateArchetype(DArchetypeId id)
	{
		if (m_ArchetypeId_To_ComponentIteratorPtrs.contains(id))
		{

			std::vector<UComponentIterator<ComponentType>*>& compArrays = m_ArchetypeId_To_ComponentIteratorPtrs.at(id) ;

			for (size_t i = 0; i < compArrays.size(); i++)
			{
				m_ComponentIteratorModifier.DeactivateArrayPtr(*compArrays[i],id);
			}

		}
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::AddOrRefreshArchetype(DArchetypeId id)
	{
		
		if (!m_ArchetypeId_To_ComponentIteratorPtrs.contains(id))
		{
			DArchetype archetype;
			DArchetypeKey key;
			m_ArchetypeRegistry.ArchetypeId_To_Archetype(id, archetype);
			m_ArchetypeRegistry.Archetype_To_ArchetypeKey(archetype, key);

			m_ArchetypeId_To_ComponentIteratorPtrs.emplace(std::pair(id, std::vector<UComponentIterator<ComponentType>*>{}));

			FComponentIteratorInfo<ComponentType>* current_CompIterator_Info{ nullptr };

			for (size_t i = 0; i < m_ComponentIteratorPtrs.size(); i++)
			{
				if (m_ArchetypeRegistry.KeyPassesInternalFilter(m_ComponentIteratorModifier.GetFilter(*m_ComponentIteratorPtrs[i]), key))
				{
					m_ComponentIteratorModifier.AddArrayPtr(*m_ComponentIteratorPtrs[i], (IComponentIteratorManager*)this, m_ComponentCompartment, id);

					current_CompIterator_Info = &m_ComponentArrays_To_ComponentArrayInfo.at(m_ComponentIteratorPtrs[i]);

					current_CompIterator_Info->ArchetypeIds.emplace_back(id);
					current_CompIterator_Info->Indexes.emplace_back(m_ArchetypeId_To_ComponentIteratorPtrs.at(id).size());

					m_ArchetypeId_To_ComponentIteratorPtrs.at(id).emplace_back(m_ComponentIteratorPtrs[i]);
				}
			}
		}
		else if (m_ArchetypeId_To_ComponentIteratorPtrs.contains(id) && !m_ComponentIteratorPtrs.empty())
		{
			RefreshArchetype(id);
		}
		
	}

	template<typename ComponentType>
	inline bool UComponentIteratorManager<ComponentType>::DestroyComponentIterator(IComponentIterator* arrayPtr)
	{
		UComponentIterator<ComponentType>* componentArray = (UComponentIterator<ComponentType>*)arrayPtr;
		return DestroyComponentArray(componentArray);
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, DArchetypeId archetypeId)
	{
		if (!componentArray)
		{
			//debug message
			return;
		}

		DestroyComponentArray(componentArray);
		AddOrRefreshArchetype(archetypeId);

		DArchetype archetype;
		DArchetypeKey keyIncluding{};
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(archetypeId, archetype);
		m_ArchetypeRegistry.Archetype_To_ArchetypeKey(archetype, keyIncluding);

		UInternalFilter internalFilter{};
		internalFilter.SetIncludingFilter(keyIncluding);

		m_ComponentIteratorModifier.SetFilter(*componentArray, internalFilter);

		Register(componentArray, archetypeId);
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::RegisterComponentArray(UComponentIterator<ComponentType>* componentArray, const UComponentFilter& filter)
	{
		if (!componentArray)
		{
			//debug message
			return;
		}

		DestroyComponentArray(componentArray);

		std::vector<DArchetypeId> archetypeIds;
		m_ArchetypeRegistry.Filter_To_ArchetypeIds(filter, archetypeIds);

		for (size_t i = 0; i < archetypeIds.size(); i++)
		{
			AddOrRefreshArchetype(archetypeIds[i]);
			Register(componentArray, archetypeIds[i]);
		}

		if (archetypeIds.empty())
		{
			Register(componentArray);
		}


		DArchetypeKey keyIncluding{};
		DArchetypeKey keyExcluding{};

		m_ArchetypeRegistry.Archetype_To_ArchetypeKey(filter.GetConstExcludingFilter(), keyExcluding);
		m_ArchetypeRegistry.Archetype_To_ArchetypeKey(filter.GetConstIncludingFilter(), keyIncluding);

		UInternalFilter internalFilter{};
		internalFilter.SetExcludingFilter(keyExcluding);
		internalFilter.SetIncludingFilter(keyIncluding);

		m_ComponentIteratorModifier.SetFilter(*componentArray, internalFilter);

	}

	template<typename ComponentType>
	inline bool UComponentIteratorManager<ComponentType>::DestroyComponentArray(UComponentIterator<ComponentType>* componentArray)
	{
		
		if (m_ComponentArrays_To_ComponentArrayInfo.contains(componentArray))
		{
			std::vector<DArchetypeId>& ids = m_ComponentArrays_To_ComponentArrayInfo.at(componentArray).ArchetypeIds;
			std::vector<size_t>& indexes = m_ComponentArrays_To_ComponentArrayInfo.at(componentArray).Indexes;
			size_t indexInVector = m_ComponentArrays_To_ComponentArrayInfo.at(componentArray).IndexInBigVector;
			std::vector<UComponentIterator<ComponentType>*>* compArrays{ nullptr };

			UComponentIterator<ComponentType>* lastCompArray{ nullptr };

			//even though its On^2 -> the size of the array won't be that big, so its probably faster than iterating through a map
			for (size_t i = 0; i < ids.size(); i++)
			{
				
				compArrays = &m_ArchetypeId_To_ComponentIteratorPtrs.at(ids[i]);

				lastCompArray = compArrays->at(compArrays->size() - 1);
				
				for (size_t j = 0; j < m_ComponentArrays_To_ComponentArrayInfo.at(lastCompArray).ArchetypeIds.size(); ++j)
				{
					if (m_ComponentArrays_To_ComponentArrayInfo.at(lastCompArray).ArchetypeIds[j] == ids[i])
					{
						m_ComponentArrays_To_ComponentArrayInfo.at(lastCompArray).Indexes[j] = indexes[i];
						break;
					}
				}
				
				//switch comp array pointers
				compArrays->at(indexes[i]) = lastCompArray;
				compArrays->pop_back();
				
			}

			m_ComponentIteratorPtrs[indexInVector] = m_ComponentIteratorPtrs[m_ComponentIteratorPtrs.size() - 1];
			m_ComponentArrays_To_ComponentArrayInfo.at(m_ComponentIteratorPtrs[indexInVector]).IndexInBigVector = indexInVector;
			m_ComponentIteratorPtrs.pop_back();
			
			
			m_ComponentArrays_To_ComponentArrayInfo.at(componentArray).ArchetypeIds.clear();
			m_ComponentArrays_To_ComponentArrayInfo.at(componentArray).Indexes.clear();
			m_ComponentArrays_To_ComponentArrayInfo.erase(componentArray);

			m_ComponentIteratorModifier.ClearArray(*componentArray);
			
			return true;
		}	

		return false;

	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::Register(UComponentIterator<ComponentType>* componentArray, DArchetypeId archetypeId)
	{

		std::vector<UComponentIterator<ComponentType>*>* compArrayVec = nullptr;

		if (!m_ArchetypeId_To_ComponentIteratorPtrs.contains(archetypeId))
		{
			m_ArchetypeId_To_ComponentIteratorPtrs.emplace(
				std::pair(
					archetypeId, std::vector<UComponentIterator<ComponentType>*>{}));
		}

		compArrayVec = &m_ArchetypeId_To_ComponentIteratorPtrs.at(archetypeId);
		compArrayVec->emplace_back(componentArray);


		FComponentIteratorInfo<ComponentType>* info{ nullptr };

		if (!m_ComponentArrays_To_ComponentArrayInfo.contains(componentArray))
		{
			m_ComponentIteratorPtrs.emplace_back(componentArray);

			m_ComponentArrays_To_ComponentArrayInfo.emplace(
				std::pair(
					componentArray,
					FComponentIteratorInfo<ComponentType>{std::vector<DArchetypeId>{}, std::vector<size_t>{}, m_ComponentIteratorPtrs.size() - 1}));
		}

		info = &m_ComponentArrays_To_ComponentArrayInfo.at(componentArray);
		info->Indexes.emplace_back(compArrayVec->size() - 1);
		info->ArchetypeIds.emplace_back(archetypeId);

		m_ComponentIteratorModifier.AddArrayPtr(*componentArray, (IComponentIteratorManager*)this, m_ComponentCompartment, archetypeId);
	}

	template<typename ComponentType>
	inline void UComponentIteratorManager<ComponentType>::Register(UComponentIterator<ComponentType>* componentArray)
	{
		m_ComponentIteratorPtrs.emplace_back(componentArray);

		FComponentIteratorInfo<ComponentType>* info{ nullptr };

		if (!m_ComponentArrays_To_ComponentArrayInfo.contains(componentArray))
		{
			m_ComponentArrays_To_ComponentArrayInfo.emplace(
				std::pair(
					componentArray,
					FComponentIteratorInfo<ComponentType>{std::vector<DArchetypeId>{}, std::vector<size_t>{}, m_ComponentIteratorPtrs.size() - 1}));
		}
	}

	//register componentArrays without the archetype ids -> have a function for that

#pragma endregion

}

