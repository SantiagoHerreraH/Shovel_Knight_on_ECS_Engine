#pragma once

#include <unordered_map>
#include <memory>

#include "../../include/Core/UComponentCompartment.h"
#include "../../include/Core/UComponentFilter.h"
#include "../../include/Core/UArchetypeRegistry.h"
#include "../../include/Core/UComponentIteratorRegistry.h"
#include "../../include/Core/UEntityHandleRegistry.h"

namespace Pillar {

	/*
	
	To Dos:
	- test entity handle and lock mechanics
	- reserve a lot of space before creating archetypes and vectors-> it will save the massive time of deallocating and allocating again a vector of vector of components
	- Eventually, substitute the vector pointers to raw array pointers (my theory is that when vectors move because of reallocation, only the vector wrapper class moves, the raw array probably doesn't)
	- Later on you can implement the dormant archetypes (archetypes that have zero entities)

	*/

	class URegistry;
	class UArchetype;

	class URegistry {

	public:

		URegistry(const int initialNumberOfComponents = 100);
		~URegistry();

		/*
		
			Register
		
		*/

		template<typename ComponentType>
		void RegisterComponent(const ComponentType& defaultComponent);

		/*
			Gets ref (unsafe)
		*/

		template<typename ComponentType>
		ComponentType* const GetComponent(const FEntity& entity); //unsafe for now

		/*
			Copy required
		*/

		template<typename ComponentType>
		bool GetComponent(ComponentType& outComponent, const UEntityHandle& entity);

		template<typename ComponentType>
		bool SetComponent(const ComponentType& component, const UEntityHandle& entity);

		template<typename ComponentType>
		bool GetComponent(ComponentType& outComponent, const FEntity& entity);

		template<typename ComponentType>
		bool SetComponent(const ComponentType& component, const FEntity& entity);

		template<typename ComponentType>
		bool AssignComponent(ComponentType&& component, const FEntity& entity);

		/*
		Returns all of the Components of that type across all archetypes
		*/

		template<typename ComponentType>
		void GetComponents(UComponentIterator<ComponentType>& outComponentArray);

		/*
		Can return the vector of Components of more than one Archetype if they pass the filter
		*/
		template<typename ComponentType>
		bool GetComponents(UComponentIterator<ComponentType>& outComponentArray, const UComponentFilter& filter);

		/*
		Can return the vector of Components of only one Archetype
		*/
		template<typename ComponentType>
		bool GetComponents(UComponentIterator<ComponentType>& outComponentArray, DArchetypeId archetypeId);

		template<typename ... ListOfComponents>
		bool CreateEntity(UEntityHandle& outEntityHandle, ListOfComponents&... components);
		
		template<typename ... ListOfComponents>
		bool CreateEntity(FEntity& outEntity, ListOfComponents&... components);
		template<typename ... ListOfComponents>
		bool CreateEntities(std::vector<FEntity>& outEntities, const size_t numberOfEntitiesToCreate, ListOfComponents&... components);
		void CreateEntities(std::vector<FEntity>& outEntities, size_t count, UArchetype& archetype);

		void DestroyEntity(const FEntity& entity); 
		void DestroyEntities(const std::vector<FEntity>& entities);
		const bool Exists(const FEntity& entity);


		void GetEntities(UEntityHandleContainer& outEntityHandleContainer, const DArchetypeId archetypeId);
		template<typename ComponentType>
		void GetEntity(UEntityHandle& outEntity, const DArchetypeId archetypeId, const size_t componentIndex);
		template<typename ComponentType>
		void GetEntity(FEntity& outEntity, const DArchetypeId archetypeId, const size_t componentIndex);

		template<typename ComponentType>
		const bool AddComponent(UEntityHandle& entityHandle, const ComponentType& component); // you need to be able to create components implictly and know which types by just the Archetype Id
		template<typename ComponentType>
		const bool RemoveComponent(UEntityHandle& entityHandle);

		template<typename ComponentType>
		const bool AddComponent(FEntity& entity, const ComponentType& component); // you need to be able to create components implictly and know which types by just the Archetype Id
		template<typename ComponentType>
		const bool RemoveComponent(FEntity& entity);

		void AddComponents(FEntity& entity, const UArchetype& archetype);
		void RemoveComponents(FEntity& entity, const UArchetype& archetype);

		void ForceDeactivateAll();
		void ForceActivateAll();

		void ActivateEntity(const FEntity& entity);
		void DeactivateEntity(const FEntity& entity);
		const bool IsActivated(const FEntity& entity);

		template<typename ComponentType>
		void Lock();
		template<typename ComponentType>
		void Unlock();
		template<typename ComponentType>
		bool IsLocked(); //make const

		template<typename ComponentType>
		void LockCreationAndDestructionOfComponentType();
		template<typename ComponentType>
		void UnlockCreationAndDestructionOfComponentType();
		template<typename ComponentType>
		bool IsComponentTypeCreationLocked();

		template<typename ComponentType>
		void LockActivationAndDeactivationOfComponentType();
		template<typename ComponentType>
		void UnlockActivationAndDeactivationOfComponentType();
		template<typename ComponentType>
		bool IsComponentTypeActivationLocked();

		template<typename ComponentType>
		void ClearComponentsOfType();

	private:

		bool AddComponent(FEntity& entity, const std::type_index& typeId);
		bool RemoveComponent(FEntity& entity, const std::type_index& typeId);

		template<typename ComponentType>
		UComponentCompartment<ComponentType>& GetComponentCompartment();
		IComponentCompartment* GetUnsafeComponentCompartment(const std::type_index& typeId);

		template<typename Component, typename ... ListOfComponents>
		void Implementation_CreateEntity(FEntity& outEntity, bool isNewArchetype, Component& component, ListOfComponents& ... components);
		void Implementation_CreateEntity(FEntity& outEntity, bool isNewArchetype) {};

		template<typename Component, typename ...ListOfComponents>
		void Implementation_CreateEntities(std::vector<FEntity>& outEntities, bool isNewArchetype, Component& component, ListOfComponents&... components);
		void Implementation_CreateEntities(std::vector<FEntity>& outEntities, bool isNewArchetype) {};

		int m_CurrentMaxComponentCount;

		std::unordered_map<std::type_index, std::unique_ptr<IComponentCompartment>> m_TypeIndex_To_ComponentCompartment;

		struct FEntityStateRegistry {
			std::unordered_set<DEntityId> CreatedEntities;
		};

		std::unordered_map<DArchetypeId, FEntityStateRegistry> m_ArchetypeId_To_Entities;
		DArchetype m_LockedComponentTypes_Creation;
		DArchetype m_LockedComponentTypes_Activation;

		UEntityHandleRegistry m_EntityHandleRegistry{};
		UArchetypeRegistry m_ArchetypeRegistry;
		UComponentIteratorRegistry m_ComponentIteratorRegistry{ m_ArchetypeRegistry };

	};

#pragma region Get And Set Component

	template<typename ComponentType>
	inline void URegistry::RegisterComponent(const ComponentType& defaultComponent)
	{
		GetComponentCompartment<ComponentType>().SetDefault(defaultComponent);
	}

	template<typename ComponentType>
	inline ComponentType* const URegistry::GetComponent(const FEntity& entity)
	{
		UComponentCompartment<ComponentType>& compartment{ GetComponentCompartment<ComponentType>() };
		return compartment.GetComponent(entity);
	}

	template<typename ComponentType>
	inline bool URegistry::GetComponent(ComponentType& outComponent, const UEntityHandle& entityHandle)
	{
		if (!entityHandle.IsValid())
		{
			return false;
		}

		return GetComponent(outComponent, entityHandle.GetConstEntity());
	}

	template<typename ComponentType>
	inline bool URegistry::SetComponent(const ComponentType& component, const UEntityHandle& entityHandle)
	{
		if (!entityHandle.IsValid())
		{
			return false;
		}

		return SetComponent(component, entityHandle.GetConstEntity());
	}

	template<typename ComponentType>
	inline bool URegistry::GetComponent(ComponentType& outComponent, const FEntity& entity)
	{
		std::type_index typeId = typeid(outComponent);
		if (!m_TypeIndex_To_ComponentCompartment.contains(typeId))
		{
			//debug message here 
			return false;
		}

		UComponentCompartment<ComponentType>* compArray{ (UComponentCompartment<ComponentType>*)m_TypeIndex_To_ComponentCompartment.at(typeId).get() };

		return compArray->GetComponent(outComponent, entity);
	}

	template<typename ComponentType>
	inline bool URegistry::SetComponent(const ComponentType& component, const FEntity& entity)
	{
		std::type_index typeId = typeid(component);
		if (!m_TypeIndex_To_ComponentCompartment.contains(typeId))
		{
			//debug message here 
			return false;
		}

		UComponentCompartment<ComponentType>* compArray{ (UComponentCompartment<ComponentType>*)m_TypeIndex_To_ComponentCompartment.at(typeId).get() };

		return compArray->SetComponent(component, entity);
	}

	template<typename ComponentType>
	inline bool URegistry::AssignComponent(ComponentType&& component, const FEntity& entity)
	{
		std::type_index typeId = typeid(component);
		if (!m_TypeIndex_To_ComponentCompartment.contains(typeId))
		{
			//debug message here 
			return false;
		}

		UComponentCompartment<ComponentType>* compArray{ (UComponentCompartment<ComponentType>*)m_TypeIndex_To_ComponentCompartment.at(typeId).get() };

		return compArray->AssignComponent(component, entity);
	}

#pragma endregion

#pragma region GetComponents

	template<typename ComponentType>
	inline void URegistry::GetComponents(UComponentIterator<ComponentType>& outComponentArray)
	{
		UComponentFilter filter{};
		ComponentType temp{ outComponentArray.GetDefaultComponent()};
		filter.SetIncludingFilter(temp);

		GetComponents(outComponentArray, filter);
	}

	template<typename ComponentType>
	inline bool URegistry::GetComponents(UComponentIterator<ComponentType>& outComponentArray, const UComponentFilter& filter)
	{
		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();

		if (!m_ArchetypeRegistry.FilterHasType<ComponentType>(filter))
		{
			return false;
		}

		m_ComponentIteratorRegistry.RegisterComponentArray(&outComponentArray, filter, &compartment);

		return true;

	}

	template<typename ComponentType>
	inline bool URegistry::GetComponents(UComponentIterator<ComponentType>& outComponentArray, DArchetypeId archetypeId)
	{
		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();

		if (!m_ArchetypeRegistry.ArchetypeIdHasType<ComponentType>(archetypeId))
		{
			return false;
		}

		m_ComponentIteratorRegistry.RegisterComponentArray(&outComponentArray, archetypeId, &compartment);

		return true;
	}

#pragma endregion

#pragma region Lock Creation and Destruction

	template<typename ComponentType>
	inline void URegistry::LockCreationAndDestructionOfComponentType()
	{
		std::type_index id{ typeid(ComponentType) };
		m_LockedComponentTypes_Creation.emplace(id);
	}

	template<typename ComponentType>
	inline void URegistry::UnlockCreationAndDestructionOfComponentType()
	{
		std::type_index id{ typeid(ComponentType) };
		m_LockedComponentTypes_Creation.erase(id);
	}

	template<typename ComponentType>
	inline bool URegistry::IsComponentTypeCreationLocked()
	{
		std::type_index id{ typeid(ComponentType) };
		return m_LockedComponentTypes_Creation.contains(id);
	}

#pragma endregion

#pragma region Loack Activation and Deactivation

	template<typename ComponentType>
	inline void URegistry::LockActivationAndDeactivationOfComponentType()
	{
		std::type_index id{ typeid(ComponentType) };
		m_LockedComponentTypes_Activation.emplace(id);
	}

	template<typename ComponentType>
	inline void URegistry::UnlockActivationAndDeactivationOfComponentType()
	{
		std::type_index id{ typeid(ComponentType) };
		m_LockedComponentTypes_Activation.erase(id);
	}

	template<typename ComponentType>
	inline bool URegistry::IsComponentTypeActivationLocked()
	{
		std::type_index id{ typeid(ComponentType) };
		return m_LockedComponentTypes_Activation.contains(id);
	}

#pragma endregion

#pragma region Clear Components Of Type

	template<typename ComponentType>
	inline void URegistry::ClearComponentsOfType()
	{
		std::type_index id{ typeid(ComponentType) };

		if (m_LockedComponentTypes_Creation.contains(id))
		{
			//debug message
			return;
		}
		
		UComponentCompartment<ComponentType>& compartment{GetComponentCompartment<ComponentType>()};
		compartment.Clear();
		m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType(id);
	}

#pragma endregion
	
#pragma region GetCompartment

	template<typename ComponentType>
	inline UComponentCompartment<ComponentType>& URegistry::GetComponentCompartment()
	{
		std::type_index typeId = typeid(ComponentType);
		if (!m_TypeIndex_To_ComponentCompartment.contains(typeId))
		{
			m_TypeIndex_To_ComponentCompartment.emplace(std::pair(typeId, std::make_unique<UComponentCompartment<ComponentType>>()));
			m_ArchetypeRegistry.RegisterType(typeId);
		}

		UComponentCompartment<ComponentType>* compArray{ (UComponentCompartment<ComponentType>*)m_TypeIndex_To_ComponentCompartment.at(typeId).get() };

		return *compArray;
	}

#pragma endregion

#pragma region Create Entity


	template<typename ...ListOfComponents>
	inline bool URegistry::CreateEntity(UEntityHandle& outEntityHandle, ListOfComponents & ...components)
	{
		FEntity entity{};

		if (!m_ArchetypeRegistry.ComponentsToArchetypeId(m_LockedComponentTypes_Creation, entity.Archetype_Id, components...))
		{
			//debug message -> can't create entity because type x is locked
			return false;
		}

		entity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(entity.Archetype_Id);

		bool isNewArchetype{ false };

		if (!m_ArchetypeId_To_Entities.contains(entity.Archetype_Id))
		{
			isNewArchetype = true;
			m_ArchetypeId_To_Entities.emplace(std::pair(entity.Archetype_Id, FEntityStateRegistry{}));
		}
		m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.emplace(entity.Entity_Id);

		Implementation_CreateEntity(entity, isNewArchetype, components...);

		m_ComponentIteratorRegistry.AddOrRefreshArchetype(entity.Archetype_Id);

		m_EntityHandleRegistry.RegisterOrUpdateEntityHandle(outEntityHandle, entity, m_ArchetypeRegistry.GetHashId(entity));

		return true;
	}

	template<typename ...ListOfComponents>
	inline bool URegistry::CreateEntity(FEntity& outEntity, ListOfComponents & ...components)
	{

		if (!m_ArchetypeRegistry.ComponentsToArchetypeId(m_LockedComponentTypes_Creation, outEntity.Archetype_Id, components...))
		{
			//debug message -> can't create entity because type x is locked
			return false;
		}

		outEntity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(outEntity.Archetype_Id);

		bool isNewArchetype{false};

		if (!m_ArchetypeId_To_Entities.contains(outEntity.Archetype_Id))
		{
			isNewArchetype = true;
			m_ArchetypeId_To_Entities.emplace(std::pair(outEntity.Archetype_Id, FEntityStateRegistry{}));
		}
		m_ArchetypeId_To_Entities.at(outEntity.Archetype_Id).CreatedEntities.emplace(outEntity.Entity_Id);

		Implementation_CreateEntity(outEntity, isNewArchetype, components...);

		m_ComponentIteratorRegistry.AddOrRefreshArchetype(outEntity.Archetype_Id);

		return true;
	}

	template<typename Component, typename ...ListOfComponents>
	inline void URegistry::Implementation_CreateEntity(FEntity& outEntity, bool isNewArchetype, Component& component, ListOfComponents & ...components)
	{
		UComponentCompartment<Component>& compartment = GetComponentCompartment<Component>();

		compartment.Create(outEntity, component);

		if (isNewArchetype)
		{
			std::type_index id = typeid(component);
			m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType(id);
		}

		Implementation_CreateEntity(outEntity, isNewArchetype, components...);
	}


#pragma endregion

#pragma region Create Entities


	template<typename ...ListOfComponents>
	inline bool URegistry::CreateEntities(std::vector<FEntity>& outEntities, const size_t numberOfEntitiesToCreate, ListOfComponents & ...components)
	{
		if (numberOfEntitiesToCreate == 0)
		{
			return false;
		}

		outEntities.resize(numberOfEntitiesToCreate);

		if (!m_ArchetypeRegistry.ComponentsToArchetypeId(m_LockedComponentTypes_Creation, outEntities[0].Archetype_Id, components...))
		{
			//debug message -> cant create entities because x component type is locked
			return false;
		}

		
		outEntities[0].Entity_Id = m_ArchetypeRegistry.GetNewEntityId(outEntities[0].Archetype_Id);

		bool isNewArchetype{ false };

		if (!m_ArchetypeId_To_Entities.contains(outEntities[0].Archetype_Id))
		{
			isNewArchetype = true;
			m_ArchetypeId_To_Entities.emplace(std::pair(outEntities[0].Archetype_Id, FEntityStateRegistry{}));
		}
		m_ArchetypeId_To_Entities.at(outEntities[0].Archetype_Id).CreatedEntities.emplace(outEntities[0].Entity_Id);

		for (size_t i = 1; i < numberOfEntitiesToCreate; i++)
		{
			outEntities[i].Archetype_Id = outEntities[0].Archetype_Id;
			outEntities[i].Entity_Id = m_ArchetypeRegistry.GetNewEntityId(outEntities[i].Archetype_Id);

			m_ArchetypeId_To_Entities.at(outEntities[i].Archetype_Id).CreatedEntities.emplace(outEntities[i].Entity_Id);
		}

		Implementation_CreateEntities(outEntities, isNewArchetype, components...);

		m_ComponentIteratorRegistry.AddOrRefreshArchetype(outEntities[0].Archetype_Id);
		
		return true;
	}

	template<typename Component, typename ...ListOfComponents>
	inline void URegistry::Implementation_CreateEntities(std::vector<FEntity>& outEntities, bool isNewArchetype, Component& component, ListOfComponents & ...components)
	{
		UComponentCompartment<Component>& compartment = GetComponentCompartment<Component>();

		for (size_t i = 0; i < outEntities.size(); i++)
		{
			compartment.Create(outEntities[i], component); //do a function that creates many entities at once
		}

		if (isNewArchetype)
		{
			std::type_index id = typeid(component);
			m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType(id);
		}

		Implementation_CreateEntities(outEntities, isNewArchetype, components...);
		
	}

#pragma endregion

#pragma region GetEntity

	template<typename ComponentType>
	inline void URegistry::GetEntity(UEntityHandle& outEntity, const DArchetypeId archetypeId, const size_t componentIndex)
	{
		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();
		
		FEntity entity;

		entity.Entity_Id = compartment.GetEntity(componentIndex, archetypeId);
		entity.Archetype_Id = archetypeId;

		m_EntityHandleRegistry.RegisterOrUpdateEntityHandle(outEntity, entity, m_ArchetypeRegistry.GetHashId(entity));
	}

	template<typename ComponentType>
	inline void URegistry::GetEntity(FEntity& outEntity, const DArchetypeId archetypeId, const size_t componentIndex)
	{
		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();

		outEntity.Entity_Id = compartment.GetEntity(componentIndex, archetypeId);
		outEntity.Archetype_Id = archetypeId;

	}

	template<typename ComponentType>
	inline const bool URegistry::AddComponent(UEntityHandle& entityHandle, const ComponentType& component)
	{
		FEntity entity{ entityHandle.GetConstEntity() };
		return AddComponent<ComponentType>(entity, component);
	}

	template<typename ComponentType>
	inline const bool URegistry::RemoveComponent(UEntityHandle& entityHandle)
	{
		FEntity entity{ entityHandle.GetConstEntity() };
		return RemoveComponent<ComponentType>(entity);
	}

#pragma endregion

#pragma region Add And Remove Component

	template<typename ComponentType>
	inline const bool URegistry::AddComponent(FEntity& entity, const ComponentType& component)
	{
		std::type_index id = typeid(component);

		if (!Exists(entity))
		{
			return false;
		}
		if (m_LockedComponentTypes_Creation.contains(id))
		{
			return false;
		}
		if (m_LockedComponentTypes_Activation.contains(id))
		{
			return false;
		}
		
		FEntity newEntity;

		DArchetype oldArchetype;
		DArchetype newArchetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, oldArchetype);

		if (oldArchetype.contains(id))
		{
			//debug message -> already has type
			return false;
		}

		newArchetype = oldArchetype;
		newArchetype.emplace(id);

		bool isNewArchetype = m_ArchetypeRegistry.RegisterArchetype(newArchetype);
		m_ArchetypeRegistry.Archetype_To_ArchetypeId(newArchetype, newEntity.Archetype_Id);
		newEntity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(newEntity.Archetype_Id);
		

		for (auto It = oldArchetype.begin(); It != oldArchetype.end(); ++It)
		{
			//if (!m_TypeIndex_To_ComponentCompartment.contains((*It)))
			//{
			//	//debug message -> something wrong happened
			//}

			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->MoveEntity(entity, newEntity);

			if (isNewArchetype)
			{
				m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType((*It));//refresh the compartment of type id that just moved arrays from one arhctype to another
			}
		}

		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();

		compartment.Create(newEntity, component);

		m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType(id);// refresh the compartment of type id that just created a new archetype array
		m_ComponentIteratorRegistry.AddOrRefreshArchetype(newEntity.Archetype_Id);

		if (!m_ArchetypeId_To_Entities.contains(newEntity.Archetype_Id))
		{
			m_ArchetypeId_To_Entities.emplace(std::pair(newEntity.Archetype_Id, FEntityStateRegistry{}));
		}

		m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.erase(entity.Entity_Id);
		m_ArchetypeId_To_Entities.at(newEntity.Archetype_Id).CreatedEntities.emplace(newEntity.Entity_Id);

		if (m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.empty())
		{
			m_ComponentIteratorRegistry.DeactivateArchetype(entity.Archetype_Id);
		}
		
		m_EntityHandleRegistry.ChangeRegistratedEntity(entity, newEntity, m_ArchetypeRegistry.GetHashId(newEntity));

		entity = newEntity;

		return true;
	}

	template<typename ComponentType>
	inline const bool URegistry::RemoveComponent(FEntity& entity)
	{
		if (!Exists(entity))
		{
			return false;
		}

		std::type_index id = typeid(ComponentType);

		if (m_LockedComponentTypes_Creation.contains(id))
		{
			return false;
		}
		if (m_LockedComponentTypes_Activation.contains(id))
		{
			return false;
		}

		
		FEntity newEntity;

		DArchetype oldArchetype;
		DArchetype newArchetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, oldArchetype);

		if (!oldArchetype.contains(id))
		{
			//debug message -> doesn't have type
			return false;
		}

		newArchetype = oldArchetype;
		newArchetype.erase(id);

		bool isNewArchetype = !m_ArchetypeRegistry.RegisterArchetype(newArchetype);
		m_ArchetypeRegistry.Archetype_To_ArchetypeId(newArchetype, newEntity.Archetype_Id);
		newEntity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(newEntity.Archetype_Id);


		for (auto It = newArchetype.begin(); It != newArchetype.end(); ++It)
		{
			//if (!m_TypeIndex_To_ComponentCompartment.contains((*It)))
			//{
			//	//debug message -> something wrong happened
			//}

			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->MoveEntity(entity, newEntity);

			if (isNewArchetype)
			{
				m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType((*It)); 
				//here you remove all of the array pointers inside the component iterators that are no longer valid
			}
		}

		UComponentCompartment<ComponentType>& compartment = GetComponentCompartment<ComponentType>();

		compartment.Destroy(entity);

		m_ComponentIteratorRegistry.AddOrRefreshArchetype(newEntity.Archetype_Id);
		//here you add the new valid array pointers of the new archetype

		if (!m_ArchetypeId_To_Entities.contains(newEntity.Archetype_Id))
		{
			m_ArchetypeId_To_Entities.emplace(std::pair(newEntity.Archetype_Id, FEntityStateRegistry{}));
		}

		m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.erase(entity.Entity_Id);
		m_ArchetypeId_To_Entities.at(newEntity.Archetype_Id).CreatedEntities.emplace(newEntity.Entity_Id);

		if (m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.empty())
		{
			m_ComponentIteratorRegistry.DeactivateArchetype(entity.Archetype_Id);
		}

		m_EntityHandleRegistry.ChangeRegistratedEntity(entity, newEntity, m_ArchetypeRegistry.GetHashId(newEntity));

		entity = newEntity;

		return true;
	}

	template<typename ComponentType>
	inline void URegistry::Lock()
	{
		LockCreationAndDestructionOfComponentType<ComponentType>();
		LockActivationAndDeactivationOfComponentType<ComponentType>();
	}

	template<typename ComponentType>
	inline void URegistry::Unlock()
	{
		UnlockCreationAndDestructionOfComponentType<ComponentType>();
		UnlockActivationAndDeactivationOfComponentType<ComponentType>();
	}

	template<typename ComponentType>
	inline bool URegistry::IsLocked()
	{
		return IsComponentTypeCreationLocked<ComponentType>() && IsComponentTypeActivationLocked<ComponentType>();
	}

#pragma endregion


	class UArchetype {

	public:

		UArchetype(const UArchetype& other);
		UArchetype(UArchetype&& other)noexcept;

		template<class ... Components>
		UArchetype(URegistry& registry, Components& ... components) {
			SetFilter(registry, components...);
		}

		UArchetype& operator=(const UArchetype& other);

		const DArchetype& GetConstArchetype()const { return m_Archetype; }

	private:
		template<typename ComponentType, typename ... ListOfComponents>
		void SetFilter(URegistry& registry, ComponentType& component, ListOfComponents&... components) {
			m_Archetype.emplace(typeid(component));
			registry.RegisterComponent<ComponentType>(component);
			SetFilter(registry, components...);
		}
		void SetFilter(URegistry& registry) {}

		DArchetype m_Archetype;
	};

}