#pragma once

#include <unordered_map>
#include <unordered_set>

#include "../../include/Core/UComponentHandle.h"
#include "../../include/Core/UComponentCompartment.h"

namespace Pillar {

	template<typename ComponentType>
	class UComponentHandleManager : public IComponentHandleManager {

	public:
		UComponentHandleManager(UComponentCompartment<ComponentType>* compartment);

		void RefreshAllComponentHandles() override; // this needs to be called after the entity handles are updated // called when new Archetype is added
		void RefreshEntity(const FEntity& entity) override; // call this when activate/ deactivate
		void ChangeEntity(const FEntity& oldEntity, const FEntity& newEntity) override; // call this when add/remove component
		void RefreshArchetype(DArchetypeId archetype); // call this when a new entity is created in an archetype
		void DeleteEntity(const FEntity& entity)override;
		void DeregisterComponentHandle(IComponentHandle* componentHandle)override; //call this on componentHandle destructor
		void Clear() override; //call this when clear component type

		void RegisterComponentHandle(UComponentHandle<ComponentType>* componentHandle, const UEntityHandle& entityHandle); //upon creation, after filling the entity handle

	private:
		void RegisterEntity(const FEntity& entity);

		UComponentHandleModifier m_ComponentHandleModifier{};
		UComponentCompartment<ComponentType>* m_ComponentCompartment;

		std::unordered_map<DArchetypeId, std::unordered_map<DEntityId, std::unordered_set<UComponentHandle<ComponentType>*>>> m_ComponentHandleData;
	};

	template<typename ComponentType>
	inline UComponentHandleManager<ComponentType>::UComponentHandleManager(UComponentCompartment<ComponentType>* compartment) :
		m_ComponentCompartment(compartment)
	{
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::RefreshAllComponentHandles()
	{
		for (auto archIt = m_ComponentHandleData.begin(); archIt != m_ComponentHandleData.end(); archIt++)
		{
			for (auto entityIt = archIt->second.begin(); entityIt != archIt->second.end(); entityIt++)
			{
				for (auto handleIt = entityIt->second.begin(); handleIt != entityIt->second.end(); handleIt++)
				{
					m_ComponentHandleModifier.SetComponentInfo(*(*handleIt), m_ComponentCompartment->GetComponent((*handleIt)->EntityHandle().GetConstEntity()), (*handleIt)->ConstEntityHandle(), this);
				}
			}
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::RefreshEntity(const FEntity& entity)
	{
		if (m_ComponentHandleData.contains(entity.Archetype_Id) && 
			m_ComponentHandleData.at(entity.Archetype_Id).contains(entity.Entity_Id))
		{
			for (auto handleIt = m_ComponentHandleData.at(entity.Archetype_Id).at(entity.Entity_Id).begin(); 
				handleIt != m_ComponentHandleData.at(entity.Archetype_Id).at(entity.Entity_Id).end(); handleIt++)
			{
				m_ComponentHandleModifier.SetComponentInfo(*(*handleIt), m_ComponentCompartment->GetComponent((*handleIt)->EntityHandle().GetConstEntity()), (*handleIt)->ConstEntityHandle(), this);
			}
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::ChangeEntity(const FEntity& oldEntity, const FEntity& newEntity)
	{
		if (m_ComponentHandleData.contains(oldEntity.Archetype_Id) &&
			m_ComponentHandleData.at(oldEntity.Archetype_Id).contains(oldEntity.Entity_Id))
		{
			RegisterEntity(newEntity);

			m_ComponentHandleData.at(newEntity.Archetype_Id).at(newEntity.Entity_Id).merge(m_ComponentHandleData.at(oldEntity.Archetype_Id).at(oldEntity.Entity_Id));

			for (auto handleIt = m_ComponentHandleData.at(newEntity.Archetype_Id).at(newEntity.Entity_Id).begin();
				handleIt != m_ComponentHandleData.at(newEntity.Archetype_Id).at(newEntity.Entity_Id).end(); handleIt++)
			{
				m_ComponentHandleModifier.SetComponentInfo(*(*handleIt), m_ComponentCompartment->GetComponent((*handleIt)->EntityHandle().GetConstEntity()), (*handleIt)->ConstEntityHandle(), this);
			}

			DeleteEntity(oldEntity);
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::RefreshArchetype(DArchetypeId archetype)
	{
		if (m_ComponentHandleData.contains(archetype))
		{
			FEntity currentEntity{}; 
			currentEntity.Archetype_Id = archetype;

			for (auto entityIt = m_ComponentHandleData.at(archetype).begin(); 
				entityIt != m_ComponentHandleData.at(archetype).end(); entityIt++)
			{
				currentEntity.Entity_Id = entityIt->first;
				RefreshEntity(currentEntity);
			}
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::DeleteEntity(const FEntity& entity)
	{
		if (m_ComponentHandleData.contains(entity.Archetype_Id) &&
			m_ComponentHandleData.at(entity.Archetype_Id).contains(entity.Entity_Id))
		{
			for (auto handleIt = m_ComponentHandleData.at(entity.Archetype_Id).at(entity.Entity_Id).begin();
				handleIt != m_ComponentHandleData.at(entity.Archetype_Id).at(entity.Entity_Id).end(); handleIt++)
			{
				m_ComponentHandleModifier.NullComponentHandle(*(*handleIt));
			}

			m_ComponentHandleData.at(entity.Archetype_Id).erase(entity.Entity_Id);
		}
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::DeregisterComponentHandle(IComponentHandle* componentHandle)
	{
		const FEntity& entity{ componentHandle->EntityHandle().GetConstEntity() };
		if (m_ComponentHandleData.contains(entity.Archetype_Id))
		{
			if (m_ComponentHandleData.at(entity.Archetype_Id).contains(entity.Entity_Id))
			{
				UComponentHandle<ComponentType>* ptr = (UComponentHandle<ComponentType>*)componentHandle;
				m_ComponentHandleData.at(entity.Archetype_Id).at(entity.Entity_Id).erase(ptr);
			}
		}
	}
	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::Clear()
	{
		m_ComponentHandleData.clear();
	}
	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::RegisterComponentHandle(UComponentHandle<ComponentType>* componentHandle, const UEntityHandle& entityHandle)
	{
		RegisterEntity(entityHandle.GetConstEntity());

		m_ComponentHandleData.
			at(entityHandle.GetConstEntity().Archetype_Id).
			at(entityHandle.GetConstEntity().Entity_Id).
			emplace(componentHandle);

		m_ComponentHandleModifier.SetComponentInfo(*componentHandle, m_ComponentCompartment->GetComponent(entityHandle.GetConstEntity()), entityHandle, this);
	}

	template<typename ComponentType>
	inline void UComponentHandleManager<ComponentType>::RegisterEntity(const FEntity& entity)
	{
		if (!m_ComponentHandleData.contains(entity.Archetype_Id))
		{
			m_ComponentHandleData.emplace(
				std::pair(
					entity.Archetype_Id,
					std::unordered_map<DEntityId, std::unordered_set<UComponentHandle<ComponentType>*>>{}));
		}
		if (!m_ComponentHandleData.at(entity.Archetype_Id).contains(entity.Entity_Id))
		{
			m_ComponentHandleData.at(entity.Archetype_Id).emplace(
				std::pair(
					entity.Entity_Id,
					std::unordered_set<UComponentHandle<ComponentType>*>{}));
		}
	}
	
}