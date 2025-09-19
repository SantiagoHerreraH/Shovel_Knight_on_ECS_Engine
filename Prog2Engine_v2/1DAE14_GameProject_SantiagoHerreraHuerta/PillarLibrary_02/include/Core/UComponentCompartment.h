#pragma once

#include <memory>
#include "../../include/Core/FEntity.h"

namespace Pillar {

	class IComponentCompartment {

	public:

		virtual void ActivateAll() = 0;
		virtual void DeactivateAll() = 0;
		virtual void Activate(const FEntity& entity) = 0;
		virtual void Deactivate(const FEntity& entity) = 0;
		virtual const bool IsActivated(const FEntity& entity) = 0;

		virtual const bool Create(const FEntity& entity) = 0;
		virtual const bool Destroy(const FEntity& entity) = 0;

		virtual const size_t TotalSizeOfActivated() = 0;
		virtual const size_t SizeOfActivated(DArchetypeId archetype) = 0;
		virtual const size_t Size(DArchetypeId archetype) = 0;

		virtual DEntityId GetEntity(size_t componentIndex, DArchetypeId archetype) = 0;
		virtual void MoveEntity(const FEntity& from, const FEntity& to) = 0;
		
	};

	struct FArchetypeIndexInformation {

		size_t ArchetypeIndex;
		std::unordered_map<DEntityId, size_t> EntityId_To_ComponentIndex;

	};

	template<typename ComponentType>
	class UComponentCompartment : public IComponentCompartment{
	public:
		virtual void ActivateAll() override;
		virtual void DeactivateAll() override;

		void Activate(const FEntity& entity) override;
		void Deactivate(const FEntity& entity) override;
		const bool IsActivated(const FEntity& entity) override;

		const bool Create(const FEntity& entity) override;
		const bool Destroy(const FEntity& entity) override;

		const size_t TotalSizeOfActivated() override;
		const size_t SizeOfActivated(DArchetypeId archetype) override;
		const size_t Size(DArchetypeId archetype);

		DEntityId GetEntity(size_t componentIndex, DArchetypeId archetype) override;

		void MoveEntity(const FEntity& from, const FEntity& to) override;

		/* local functions*/

		bool GetComponent(ComponentType& outComponent, const FEntity& entity);
		bool SetComponent(const ComponentType& component, const FEntity& entity);

		bool AssignComponent(ComponentType&& component, const FEntity& entity);
		ComponentType* GetComponent(const FEntity& entity); 
		std::vector<ComponentType>* GetComponents(DArchetypeId archetypeId);

		bool Create(const FEntity& entity, const ComponentType& component); //maybe I should do an emplace function

		void SetDefault(const ComponentType& defaultComponent);

		void Clear();

	private:

		bool CreateArchetype(const FEntity& entity);

		std::unordered_map<DArchetypeId, FArchetypeIndexInformation> m_ArchetypeId_To_ComponentIndexInformation;
		std::unordered_map<size_t, DArchetypeId> m_ArchetypeIndex_To_ArchetypeId;
		std::vector<std::vector<ComponentType>> m_ArchetypeComponents; // this and the entities have to have the same order
		std::vector<std::vector<DEntityId>>		m_ArchetypeEntities;
		
		std::vector<size_t> m_SizeOfActivated; 

		//make sure to reserve a lot of space for the components and archetypes on creation
		std::unique_ptr<ComponentType> m_DefaultComponent;
	};

#pragma region Activation

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::ActivateAll()
	{
		for (size_t i = 0; i < m_ArchetypeEntities.size(); i++)
		{
			m_SizeOfActivated.at(i) = m_ArchetypeEntities.at(i).size();
		}
	}

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::DeactivateAll()
	{
		for (size_t i = 0; i < m_SizeOfActivated.size(); i++)
		{
			m_SizeOfActivated.at(i) = 0;
		}
	}

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::Activate(const FEntity& entity)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			if (m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
			{
				size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);
				size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;

				size_t sizeOfActivated = m_SizeOfActivated[archetypeIndex];

				if (componentIndex == sizeOfActivated)
				{
					++m_SizeOfActivated[archetypeIndex];
				}
				else if (componentIndex > sizeOfActivated)
				{
					ComponentType temp		= std::move(m_ArchetypeComponents[archetypeIndex][sizeOfActivated]);
					DEntityId tempEntity	= m_ArchetypeEntities[archetypeIndex][sizeOfActivated];

					//Do swap here instead of assignment
					m_ArchetypeComponents[archetypeIndex][sizeOfActivated] = std::move(m_ArchetypeComponents[archetypeIndex][componentIndex]);
					m_ArchetypeEntities[archetypeIndex][sizeOfActivated]   = m_ArchetypeEntities[archetypeIndex][componentIndex];

					m_ArchetypeComponents[archetypeIndex][componentIndex]	= std::move(temp);
					m_ArchetypeEntities[archetypeIndex][componentIndex]		= tempEntity;

					m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id) = sizeOfActivated;
					m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(tempEntity) = componentIndex;

					++m_SizeOfActivated[archetypeIndex];
				}
			}
			
		}
		
	}

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::Deactivate(const FEntity& entity)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			if (m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
			{
				size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);
				size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;

				size_t sizeOfActivated = m_SizeOfActivated[archetypeIndex];

				if (componentIndex == (sizeOfActivated - 1))
				{
					--m_SizeOfActivated[archetypeIndex];
				}
				else if (componentIndex < sizeOfActivated)
				{
					ComponentType temp = std::move(m_ArchetypeComponents[archetypeIndex][sizeOfActivated - 1]);
					DEntityId tempEntity = m_ArchetypeEntities[archetypeIndex][sizeOfActivated - 1];

					m_ArchetypeComponents[archetypeIndex][sizeOfActivated - 1] = std::move(m_ArchetypeComponents[archetypeIndex][componentIndex]);
					m_ArchetypeEntities[archetypeIndex][sizeOfActivated   - 1] = m_ArchetypeEntities[archetypeIndex][componentIndex];

					m_ArchetypeComponents[archetypeIndex][componentIndex] = std::move(temp);
					m_ArchetypeEntities[archetypeIndex][componentIndex] = tempEntity;

					m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id) = sizeOfActivated - 1;
					m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(tempEntity) = componentIndex;

					--m_SizeOfActivated[archetypeIndex];
				}
			}

		}
	}

	template<typename ComponentType>
	inline const bool UComponentCompartment<ComponentType>::IsActivated(const FEntity& entity)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			if (m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
			{
				size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);
				size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;

				return componentIndex < m_SizeOfActivated[archetypeIndex];
			}
		}

		return false;
	}


#pragma endregion

#pragma region Get Entity

	template<typename ComponentType>
	inline DEntityId UComponentCompartment<ComponentType>::GetEntity(size_t componentIndex, DArchetypeId archetype)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(archetype))
		{
			size_t matrixIndex = m_ArchetypeId_To_ComponentIndexInformation.at(archetype).ArchetypeIndex;

			return m_ArchetypeEntities[matrixIndex][componentIndex];
		}

		//debug message
		return DEntityId{0};
	}

#pragma endregion

#pragma region Move Entity

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::MoveEntity(const FEntity& from, const FEntity& to)
	{
		
		ComponentType temp{ *m_DefaultComponent.get()};
		Create(from, temp);
		GetComponent(temp, from);
		
		if (!Create(to, temp))
		{
			SetComponent(temp, to);
		}
		if (!IsActivated(from))
		{
			Deactivate(to);
		}

		Destroy(from);
		
	}

#pragma endregion

#pragma region Get And Set Component

	template<typename ComponentType>
	inline ComponentType* UComponentCompartment<ComponentType>::GetComponent(const FEntity& entity)
	{
		if (!m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			//debug message here
			return nullptr;
		}
		if (!m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
		{
			return nullptr;
		}

		size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;
		size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);

		return &m_ArchetypeComponents[archetypeIndex][componentIndex];
	}

	template<typename ComponentType>
	inline bool UComponentCompartment<ComponentType>::GetComponent(ComponentType& outComponent, const FEntity& entity)
	{
		if (!m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			//debug message here
			return false;
		}
		if (!m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
		{
			return false;
		}

		size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;
		size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);

		outComponent = m_ArchetypeComponents[archetypeIndex][componentIndex];

		return true;
	}

	template<typename ComponentType>
	inline bool UComponentCompartment<ComponentType>::SetComponent(const ComponentType& component, const FEntity& entity)
	{
		if (!m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			//debug message here
			return false;
		}
		if (!m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
		{
			return false;
		}

		size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;
		size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);

		m_ArchetypeComponents[archetypeIndex][componentIndex] = component;

		return true;
	}

	template<typename ComponentType>
	inline bool UComponentCompartment<ComponentType>::AssignComponent(ComponentType&& component, const FEntity& entity)
	{
		if (!m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			//debug message here
			return false;
		}
		if (!m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
		{
			return false;
		}

		size_t archetypeIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;
		size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);

		m_ArchetypeComponents[archetypeIndex][componentIndex] = component; // my understanding is that I don't need to do std::move here since it's already an rvalue reference

		return true;
	}

#pragma endregion

#pragma region GetComponents

	template<typename ComponentType>
	inline std::vector<ComponentType>* UComponentCompartment<ComponentType>::GetComponents(DArchetypeId archetypeId)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(archetypeId))
		{
			return &m_ArchetypeComponents[m_ArchetypeId_To_ComponentIndexInformation.at(archetypeId).ArchetypeIndex];
		}
		return nullptr;
	}

#pragma endregion

#pragma region Create


	template<typename ComponentType>
	inline bool UComponentCompartment<ComponentType>::Create(const FEntity& entity, const ComponentType& component)
	{
		SetDefault(component);
		CreateArchetype(entity);

		if (m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
		{
			//debug message ->Entity already exists
			return false;
		}

		size_t matrixIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;
		size_t compIndex = m_ArchetypeComponents[matrixIndex].size();

		m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.emplace(std::pair(entity.Entity_Id, compIndex));

		++m_SizeOfActivated[matrixIndex];
		m_ArchetypeComponents[matrixIndex].emplace_back(component);
		m_ArchetypeEntities[matrixIndex].emplace_back(entity.Entity_Id);

		return true;

	}

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::SetDefault(const ComponentType& defaultComponent)
	{
		if (!m_DefaultComponent)
		{
			m_DefaultComponent = std::make_unique<ComponentType>(defaultComponent);
		}
	}

	template<typename ComponentType>
	inline void UComponentCompartment<ComponentType>::Clear()
	{
		m_ArchetypeId_To_ComponentIndexInformation.clear();
		m_ArchetypeIndex_To_ArchetypeId.clear();
		m_ArchetypeComponents.clear(); // this and the entities have to have the same order
		m_ArchetypeEntities.clear();
		m_SizeOfActivated.clear();
	}

	template<typename ComponentType>
	inline bool UComponentCompartment<ComponentType>::CreateArchetype(const FEntity& entity)
	{
		if (!m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			//Create Archetype
			m_ArchetypeId_To_ComponentIndexInformation.emplace(
				std::pair(
					entity.Archetype_Id,
					FArchetypeIndexInformation{
						{m_ArchetypeComponents.size()},
						std::unordered_map<DEntityId, size_t>{} }));
			
			m_ArchetypeIndex_To_ArchetypeId.emplace(std::pair(m_ArchetypeComponents.size(), entity.Archetype_Id));

			m_ArchetypeComponents.emplace_back();
			m_ArchetypeEntities.emplace_back();
			m_SizeOfActivated.emplace_back();

			return true;
		}

		return false;
	}

#pragma endregion
	
#pragma region Destroy

	template<typename ComponentType>
	inline const bool UComponentCompartment<ComponentType>::Create(const FEntity& entity)
	{
		if (!m_DefaultComponent)
		{
			return false;
		}
		return Create(entity, *m_DefaultComponent.get());
	}

	template<typename ComponentType>
	inline const bool UComponentCompartment<ComponentType>::Destroy(const FEntity& entity)
	{

		Deactivate(entity);

		if (m_ArchetypeId_To_ComponentIndexInformation.contains(entity.Archetype_Id))
		{
			if (m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.contains(entity.Entity_Id))
			{
				size_t componentIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entity.Entity_Id);
				size_t matrixIndex = m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).ArchetypeIndex;

				std::vector<ComponentType>& archetypeVector = m_ArchetypeComponents[matrixIndex];
				std::vector<DEntityId>& entityVector = m_ArchetypeEntities[matrixIndex];

				archetypeVector[componentIndex] = archetypeVector[archetypeVector.size() - 1];
				entityVector[componentIndex]    = entityVector[archetypeVector.size() - 1];

				m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.at(entityVector[componentIndex]) = componentIndex;
				m_ArchetypeId_To_ComponentIndexInformation.at(entity.Archetype_Id).EntityId_To_ComponentIndex.erase(entity.Entity_Id);

				archetypeVector.pop_back();
				entityVector.pop_back();

				return true;
			}
		}
		return false;
	}

	template<typename ComponentType>
	inline const size_t UComponentCompartment<ComponentType>::TotalSizeOfActivated()
	{
		size_t counter{};

		for (size_t i = 0; i < m_SizeOfActivated.size(); i++)
		{
			counter += m_SizeOfActivated[i];
		}

		return counter;
	}

	template<typename ComponentType>
	inline const size_t UComponentCompartment<ComponentType>::SizeOfActivated(DArchetypeId archetype)
	{

		if (m_ArchetypeId_To_ComponentIndexInformation.contains(archetype))
		{
			return m_SizeOfActivated[m_ArchetypeId_To_ComponentIndexInformation.at(archetype).ArchetypeIndex];
		}
		//debug message
		return 0;
	}

	template<typename ComponentType>
	inline const size_t UComponentCompartment<ComponentType>::Size(DArchetypeId archetype)
	{
		if (m_ArchetypeId_To_ComponentIndexInformation.contains(archetype))
		{
			return m_ArchetypeComponents.at(m_ArchetypeId_To_ComponentIndexInformation.at(archetype).ArchetypeIndex).size();
		}
		//debug message
		return 0;
	}

#pragma endregion
}
