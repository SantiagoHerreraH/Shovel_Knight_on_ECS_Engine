#pragma once

#include <vector>
#include <typeindex>
#include <unordered_map>
#include <memory>

#include "../../include/Core/UComponentCompartment.h"
#include "../../include/Core/UInternalFilter.h"
#include "../../include/Core/IComponentIterator.h"

namespace Pillar {

	/*
		Wrapper class for component Iteration

		- For now it does not have multiple thread support because I've never coded that
		- 
	*/

	class UComponentIteratorModifier;

	template<typename ComponentType>
	class UComponentIterator : public IComponentIterator{

	public:
		explicit UComponentIterator();
		explicit UComponentIterator(const ComponentType& component);
		~UComponentIterator(); //This must always be destroyed before the registry

		UComponentIterator<ComponentType>& operator=(const UComponentIterator<ComponentType>& other) = delete; 

		void Destroy() override;

		bool IsEmpty() const;
		bool HasSingleComponent() const;

		const size_t NumOfArchetypes() const;
		const DArchetypeId GetArchetype(size_t archetypeIndex) const;

		const size_t TotalNumOfComponents() const;
		const size_t NumOfComponents(size_t archetypeIndex) const;
		const size_t NumOfActivatedComponents(size_t archetypeIndex) const;

		const ComponentType& ConstAt(size_t archetypeIndex, size_t componentIndex) const;
		ComponentType& At(size_t archetypeIndex, size_t componentIndex);

		const std::vector<ComponentType>* ConstDataAt(DArchetypeId id) const;
		std::vector<ComponentType>* DataAt(DArchetypeId id);
		
		const ComponentType& GetDefaultComponent() { return m_DefaultComponent; }

	private:

		UComponentCompartment<ComponentType>* m_ComponentCompartment{ nullptr };
		IComponentIteratorManager* m_ComponentIteratorManager{ nullptr };

		ComponentType m_DefaultComponent;

		std::vector<std::vector<ComponentType>*> m_ComponentArrayPtrs;

		std::unordered_map<DArchetypeId, size_t> m_ArchetypeId_To_Index;
		std::vector<DArchetypeId> m_ArchetypeIds; //Index to Archetype

		std::unordered_set<DArchetypeId> m_DeactivatedArchetypeIds;

		UInternalFilter m_InternalFilter;

		friend class UComponentIteratorModifier;
	};

#pragma region UComponentArrayImplementations

#pragma region Constructor And Destructor

	template<typename ComponentType>
	inline UComponentIterator<ComponentType>::UComponentIterator()
	{
	}

	template<typename ComponentType>
	inline UComponentIterator<ComponentType>::UComponentIterator(const ComponentType& component) :
		m_DefaultComponent(component)
	{
	}

	template<typename ComponentType>
	inline UComponentIterator<ComponentType>::~UComponentIterator()
	{
		Destroy();
	}

	template<typename ComponentType>
	inline void UComponentIterator<ComponentType>::Destroy()
	{
		if (m_ComponentIteratorManager)
		{
			m_ComponentIteratorManager->DestroyComponentIterator((IComponentIterator*)this);
			m_ComponentIteratorManager = nullptr;
		}
		
		//if the URegistry gets destroyed before this, there will be leaks 
		//Takeaway -> Have the systems get destroyed before the URegistry or make these pointers shared or weak pointers

		//Clear() -> compManager.DestroyComponentArray() -> compModifier.ResetArray()
	}

	template<typename ComponentType>
	inline bool UComponentIterator<ComponentType>::IsEmpty() const
	{
		return m_ComponentArrayPtrs.empty();
	}

	template<typename ComponentType>
	inline bool UComponentIterator<ComponentType>::HasSingleComponent() const
	{
		return m_ComponentArrayPtrs.size() == 1 && m_ComponentArrayPtrs.at(0)->size() == 1;
	}

#pragma endregion

#pragma region Num Of
	template<typename ComponentType>
	inline const size_t UComponentIterator<ComponentType>::NumOfArchetypes() const
	{
		return m_ComponentArrayPtrs.size();
	}

	template<typename ComponentType>
	inline const DArchetypeId UComponentIterator<ComponentType>::GetArchetype(size_t archetypeIndex) const
	{
		return m_ArchetypeIds.at(archetypeIndex);
	}

	template<typename ComponentType>
	inline const size_t UComponentIterator<ComponentType>::TotalNumOfComponents() const
	{
		size_t count{0};
		for (size_t i = 0; i < m_ComponentArrayPtrs.size(); i++)
		{
			count += m_ComponentArrayPtrs.at(i)->size();
		}

		return count;
	}

	template<typename ComponentType>
	inline const size_t UComponentIterator<ComponentType>::NumOfComponents(size_t archetypeIndex) const
	{
		if (archetypeIndex < m_ComponentArrayPtrs.size())
		{
			return m_ComponentArrayPtrs.at(archetypeIndex)->size();
		}
		
		return 0;
	}

	template<typename ComponentType>
	inline const size_t UComponentIterator<ComponentType>::NumOfActivatedComponents(size_t archetypeIndex) const
	{
		if (m_ComponentCompartment && archetypeIndex < m_ComponentArrayPtrs.size())
		{
			return m_ComponentCompartment->SizeOfActivated(m_ArchetypeIds[archetypeIndex]);
		}
		return size_t{ 0 };
	}

#pragma endregion

#pragma region At

	template<typename ComponentType>
	inline const ComponentType& UComponentIterator<ComponentType>::ConstAt(size_t archetypeIndex, size_t componentIndex) const
	{
		if (archetypeIndex < m_ComponentArrayPtrs.size() &&
			componentIndex < m_ComponentArrayPtrs.at(archetypeIndex)->size())
		{
			return m_ComponentArrayPtrs.at(archetypeIndex)->at(componentIndex);
		}

		return m_DefaultComponent;
	}

	template<typename ComponentType>
	inline ComponentType& UComponentIterator<ComponentType>::At(size_t archetypeIndex, size_t componentIndex) 
	{
		if (archetypeIndex < m_ComponentArrayPtrs.size() && 
			componentIndex < m_ComponentArrayPtrs.at(archetypeIndex)->size())
		{
			return m_ComponentArrayPtrs.at(archetypeIndex)->at(componentIndex);
		}

		return m_DefaultComponent;
	}

	template<typename ComponentType>
	inline const std::vector<ComponentType>* UComponentIterator<ComponentType>::ConstDataAt(DArchetypeId id) const
	{
		return DataAt(id);
	}

	template<typename ComponentType>
	inline std::vector<ComponentType>* UComponentIterator<ComponentType>::DataAt(DArchetypeId id)
	{
		if (m_ArchetypeId_To_Index.contains(id))
		{
			return m_ComponentArrayPtrs.at(m_ArchetypeId_To_Index.at(id));
		}

		//debug message here
		return nullptr;
	}

#pragma endregion

#pragma endregion



	class UComponentIteratorModifier {

	public:
		template<typename ComponentType>
		ComponentType& GetDefaultComponent(UComponentIterator<ComponentType>& componentIterator);
		template<typename ComponentType>
		void ClearArray(UComponentIterator<ComponentType>& componentIterator); // check if there are any calls to this
		template<typename ComponentType>
		void RefreshArrayPtr(UComponentIterator<ComponentType>& componentIterator, DArchetypeId archetypeId);
		template<typename ComponentType>
		void RefreshIterator(UComponentIterator<ComponentType>& componentIterator);
		template<typename ComponentType>
		bool AddArrayPtr(UComponentIterator<ComponentType>& componentIterator, IComponentIteratorManager* manager, UComponentCompartment<ComponentType>& compartment, DArchetypeId id);
		template<typename ComponentType>
		bool DeactivateArrayPtr(UComponentIterator<ComponentType>& componentIterator, DArchetypeId id); // doesn't change filter, just removes array ptr
		template<typename ComponentType>
		void SetFilter(UComponentIterator<ComponentType>& componentIterator, const UInternalFilter& filter);
		template<typename ComponentType>
		const UInternalFilter& GetFilter(UComponentIterator<ComponentType>& componentIterator);

	};

#pragma region Add and Remove ArrayPtr

	template<typename ComponentType>
	inline ComponentType& UComponentIteratorModifier::GetDefaultComponent(UComponentIterator<ComponentType>& componentIterator)
	{
		return componentIterator.m_DefaultComponent;
	}

	template<typename ComponentType>
	inline void UComponentIteratorModifier::ClearArray(UComponentIterator<ComponentType>& componentArray)
	{
		componentArray.m_ComponentCompartment = nullptr;
		componentArray.m_ComponentArrayPtrs.clear();
		componentArray.m_ArchetypeId_To_Index.clear();
		componentArray.m_ArchetypeIds.clear();
		componentArray.m_DeactivatedArchetypeIds.clear();
	}

	template<typename ComponentType>
	inline void UComponentIteratorModifier::RefreshArrayPtr(UComponentIterator<ComponentType>& componentIterator, DArchetypeId archetypeId)
	{
		if (componentIterator.m_DeactivatedArchetypeIds.contains(archetypeId))
		{
			AddArrayPtr(componentIterator, (IComponentIteratorManager*)componentIterator.m_ComponentIteratorManager, *componentIterator.m_ComponentCompartment, archetypeId);
		}
		else if (componentIterator.m_ArchetypeId_To_Index.contains(archetypeId)) 
		{
			std::vector<ComponentType>* components = componentIterator.m_ComponentCompartment->GetComponents(archetypeId);
			
			if (!components)
			{
				DeactivateArrayPtr(componentIterator, archetypeId);
			}
			else if (components->empty())
			{
				DeactivateArrayPtr(componentIterator, archetypeId);
			}
			else
			{
				componentIterator.m_ComponentArrayPtrs[componentIterator.m_ArchetypeId_To_Index.at(archetypeId)] = components;
			}
		}
	}

	template<typename ComponentType>
	inline void UComponentIteratorModifier::RefreshIterator(UComponentIterator<ComponentType>& componentIterator)
	{
		for (size_t i = 0; i < componentIterator.m_ArchetypeIds.size(); i++)
		{
			RefreshArrayPtr(componentIterator, componentIterator.m_ArchetypeIds[i]);
		}
	}

	template<typename ComponentType>
	inline bool UComponentIteratorModifier::AddArrayPtr(UComponentIterator<ComponentType>& componentIterator, IComponentIteratorManager* manager, UComponentCompartment<ComponentType>& compartment, DArchetypeId archetypeId)
	{
		std::vector<ComponentType>* components{ compartment.GetComponents(archetypeId) };

		if (!componentIterator.m_ArchetypeId_To_Index.contains(archetypeId))
		{

			if (components && !components->empty())
			{
				componentIterator.m_ArchetypeId_To_Index.emplace(std::pair(archetypeId, componentIterator.m_ComponentArrayPtrs.size()));
				componentIterator.m_ComponentArrayPtrs.emplace_back(components);
				componentIterator.m_ArchetypeIds.emplace_back(archetypeId);

				componentIterator.m_DeactivatedArchetypeIds.erase(archetypeId);
			}
			else
			{
				componentIterator.m_DeactivatedArchetypeIds.emplace(archetypeId);
			}
			
			if (!componentIterator.m_ComponentCompartment)
			{
				componentIterator.m_ComponentIteratorManager = manager;//this is okay -> because you can reset the array without nulling the manager
				componentIterator.m_ComponentCompartment = &compartment;
			}

			return true;
		}

		return false;
	}
	/*
		CHECK THIS
	*/
	template<typename ComponentType>
	inline bool UComponentIteratorModifier::DeactivateArrayPtr(UComponentIterator<ComponentType>& componentIterator, DArchetypeId archetypeId)
	{
		if (componentIterator.m_ArchetypeId_To_Index.contains(archetypeId))
		{

			const size_t indexOfRemoved = componentIterator.m_ArchetypeId_To_Index.at(archetypeId);
			const size_t lastIndex = componentIterator.m_ArchetypeIds.size() - 1;
			const DArchetypeId lastId = componentIterator.m_ArchetypeIds[lastIndex];

			componentIterator.m_ComponentArrayPtrs[indexOfRemoved] = componentIterator.m_ComponentArrayPtrs[lastIndex];
			componentIterator.m_ArchetypeIds[indexOfRemoved] = componentIterator.m_ArchetypeIds[lastIndex];

			componentIterator.m_ComponentArrayPtrs.pop_back();
			componentIterator.m_ArchetypeIds.pop_back();

			componentIterator.m_ArchetypeId_To_Index.at(lastId) = indexOfRemoved;
			componentIterator.m_ArchetypeId_To_Index.erase(archetypeId);

			componentIterator.m_DeactivatedArchetypeIds.emplace(archetypeId);
			return true;
			
		}

		return false;
	}

	template<typename ComponentType>
	inline void UComponentIteratorModifier::SetFilter(UComponentIterator<ComponentType>& componentIterator, const UInternalFilter& filter)
	{
		componentIterator.m_InternalFilter = filter;
	}

	template<typename ComponentType>
	inline const UInternalFilter& UComponentIteratorModifier::GetFilter(UComponentIterator<ComponentType>& componentIterator)
	{
		return componentIterator.m_InternalFilter;
	}

#pragma endregion

}