
#include "../../include/Core/URegistry.h"

namespace Pillar {

#pragma region Destructor

	URegistry::URegistry(const int initialNumberOfComponents) :
		m_CurrentMaxComponentCount(initialNumberOfComponents),
		m_ArchetypeRegistry(initialNumberOfComponents)
	{
	}

	URegistry::~URegistry()
	{
		m_TypeIndex_To_ComponentCompartment.clear();
		m_ArchetypeId_To_Entities.clear();
		m_LockedComponentTypes_Creation.clear();
	}

#pragma endregion

#pragma region DestroyEntity

	void URegistry::CreateEntities(std::vector<FEntity>& outEntities, size_t count, UArchetype& archetype)
	{
		if (count == 0)
		{
			return;
		}

		for (auto It = archetype.GetConstArchetype().begin(); It != archetype.GetConstArchetype().end(); It++)
		{
			if (m_LockedComponentTypes_Creation.contains((*It)) || 
				m_LockedComponentTypes_Activation.contains((*It)) || 
				!m_ArchetypeRegistry.IsTypeRegistered((*It)))
			{
				return;
			}
		}

		DArchetypeId archetypeId{};

		m_ArchetypeRegistry.Archetype_To_ArchetypeId(archetype.GetConstArchetype(), archetypeId);

		outEntities.resize(count);

		bool isNewArchetype{ false };

		if (!m_ArchetypeId_To_Entities.contains(archetypeId))
		{
			isNewArchetype = true;
			m_ArchetypeId_To_Entities.emplace(std::pair(archetypeId, FEntityStateRegistry{}));
		}

		IComponentCompartment* compartment{ nullptr };

		for (size_t i = 0; i < outEntities.size(); i++)
		{
			outEntities.at(i).Archetype_Id = archetypeId;
			outEntities.at(i).Entity_Id = m_ArchetypeRegistry.GetNewEntityId(archetypeId);
			m_ArchetypeId_To_Entities.at(archetypeId).CreatedEntities.emplace(outEntities.at(i).Entity_Id);
			
			for (auto It = archetype.GetConstArchetype().begin(); It != archetype.GetConstArchetype().end(); It++)
			{
				compartment = GetUnsafeComponentCompartment((*It));
				compartment->Create(outEntities.at(i));

				if (isNewArchetype)
				{
					m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType((*It));
				}
			}
		}

		m_ComponentIteratorRegistry.AddOrRefreshArchetype(archetypeId);
	}

	void URegistry::DestroyEntity(const FEntity& entity)
	{
		if (!Exists(entity))
		{
			return;
		}

		if (!m_ArchetypeId_To_Entities.contains(entity.Archetype_Id))
		{
			//archetype doesn't exist yet
			return;
		}

		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_LockedComponentTypes_Creation.contains((*It)))
			{
				return;
			}
		}

		m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.erase(entity.Entity_Id);
		if (m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.empty())
		{
			m_ComponentIteratorRegistry.DeactivateArchetype(entity.Archetype_Id);
		}

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_TypeIndex_To_ComponentCompartment.contains((*It)))
			{
				m_TypeIndex_To_ComponentCompartment.at((*It)).get()->Destroy(entity);
			}
		}
		m_EntityHandleRegistry.DeregisterEntity(entity);
	}

	void URegistry::DestroyEntities(const std::vector<FEntity>& entities)
	{
		for (size_t i = 0; i < entities.size(); i++)
		{
			DestroyEntity(entities[i]);
		}
	}

	const bool URegistry::Exists(const FEntity& entity)
	{
		if (m_ArchetypeId_To_Entities.contains(entity.Archetype_Id))
		{
			return m_ArchetypeId_To_Entities.at(entity.Archetype_Id).CreatedEntities.contains(entity.Entity_Id);
		}
		return false;
	}

	void URegistry::GetEntities(UEntityHandleContainer& outEntityHandleContainer, const DArchetypeId archetypeId)
	{

		DArchetype archetype{};
		if (!m_ArchetypeRegistry.ArchetypeId_To_Archetype(archetypeId, archetype))
		{
			return;
		}
		
		IComponentCompartment* compartment = GetUnsafeComponentCompartment((*archetype.begin()));
		FEntity entity{};
		UEntityHandle entityHandle{};
		outEntityHandleContainer.Clear();

		for (size_t i = 0; i < compartment->Size(archetypeId); i++)
		{
			entity.Entity_Id = compartment->GetEntity(i, archetypeId);
			entity.Archetype_Id = archetypeId;

			m_EntityHandleRegistry.RegisterOrUpdateEntityHandle(entityHandle, entity, m_ArchetypeRegistry.GetHashId(entity));
			outEntityHandleContainer.Add(entityHandle);
		}
		
	}

#pragma endregion

#pragma region Activation and Deactivation

	void URegistry::AddComponents(FEntity& entity, const UArchetype& archetype)
	{
		for (auto It = archetype.GetConstArchetype().begin(); It != archetype.GetConstArchetype().end(); ++It)
		{
			AddComponent(entity, (*It));
		}
	}

	void URegistry::RemoveComponents(FEntity& entity, const UArchetype& archetype)
	{
		for (auto It = archetype.GetConstArchetype().begin(); It != archetype.GetConstArchetype().end(); It++)
		{
			RemoveComponent(entity, (*It));
		}
	}

	void URegistry::ForceDeactivateAll()
	{
		for (auto It = m_TypeIndex_To_ComponentCompartment.begin(); It != m_TypeIndex_To_ComponentCompartment.end(); It++)
		{
			m_TypeIndex_To_ComponentCompartment.at(It->first).get()->DeactivateAll();
		}
	}

	void URegistry::ForceActivateAll()
	{
		for (auto It = m_TypeIndex_To_ComponentCompartment.begin(); It != m_TypeIndex_To_ComponentCompartment.end(); It++)
		{
			m_TypeIndex_To_ComponentCompartment.at(It->first).get()->ActivateAll();
		}
	}

	void URegistry::ActivateEntity(const FEntity& entity)
	{
		if (!Exists(entity) || IsActivated(entity))
		{
			return;
		}

		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_LockedComponentTypes_Activation.contains((*It)))
			{
				return;
			}
		}

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->Activate(entity);
		}

	}

	void URegistry::DeactivateEntity(const FEntity& entity)
	{
		if (!Exists(entity) || !IsActivated(entity))
		{
			return;
		}

		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			if (m_LockedComponentTypes_Activation.contains((*It)))
			{
				return;
			}
		}

		for (auto It = archetype.begin(); It != archetype.end(); It++)
		{
			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->Deactivate(entity);
		}
	}

	const bool URegistry::IsActivated(const FEntity& entity)
	{
		if (!Exists(entity))
		{
			return false;
		}

		DArchetype archetype;
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, archetype);
		
		//for now, lets just check one component
		return m_TypeIndex_To_ComponentCompartment.at((*archetype.begin())).get()->IsActivated(entity);
		
	}

	bool URegistry::AddComponent(FEntity& entity, const std::type_index& typeId)
	{
		
		if (!Exists(entity))
		{
			return false;
		}
		if (!m_ArchetypeRegistry.IsTypeRegistered(typeId))
		{
			return false;
		}
		if (m_LockedComponentTypes_Creation.contains(typeId))
		{
			return false;
		}
		if (m_LockedComponentTypes_Activation.contains(typeId))
		{
			return false;
		}

		FEntity newEntity;

		DArchetype oldArchetype{};
		DArchetype newArchetype{};
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, oldArchetype);

		if (oldArchetype.contains(typeId))
		{
			//debug message -> already has type
			return false;
		}

		newArchetype = oldArchetype;
		newArchetype.emplace(typeId);

		bool isNewArchetype = m_ArchetypeRegistry.RegisterArchetype(newArchetype);
		m_ArchetypeRegistry.Archetype_To_ArchetypeId(newArchetype, newEntity.Archetype_Id);
		newEntity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(newEntity.Archetype_Id);


		for (auto It = oldArchetype.begin(); It != oldArchetype.end(); ++It)
		{
			if (!m_TypeIndex_To_ComponentCompartment.contains((*It)))
			{
				//debug message -> something wrong happened
			}

			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->MoveEntity(entity, newEntity);

			if (isNewArchetype)
			{
				m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType((*It)); //refresh each compartment of type that is being moved
			}
		}

		IComponentCompartment* compartment = GetUnsafeComponentCompartment(typeId);

		compartment->Create(newEntity);

		m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType(typeId); //refresh the compartment of type that was just added an archetype array
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

	bool URegistry::RemoveComponent(FEntity& entity, const std::type_index& typeId)
	{
		if (!Exists(entity))
		{
			return false;
		}
		if (!m_ArchetypeRegistry.IsTypeRegistered(typeId))
		{
			return false;
		}
		if (m_LockedComponentTypes_Creation.contains(typeId))
		{
			return false;
		}
		if (m_LockedComponentTypes_Activation.contains(typeId))
		{
			return false;
		}


		FEntity newEntity;

		DArchetype oldArchetype{};
		DArchetype newArchetype{};
		m_ArchetypeRegistry.ArchetypeId_To_Archetype(entity.Archetype_Id, oldArchetype);

		if (!oldArchetype.contains(typeId))
		{
			//debug message -> doesn't have type
			return false;
		}

		newArchetype = oldArchetype;
		newArchetype.erase(typeId);

		bool isNewArchetype = !m_ArchetypeRegistry.RegisterArchetype(newArchetype);
		m_ArchetypeRegistry.Archetype_To_ArchetypeId(newArchetype, newEntity.Archetype_Id);
		newEntity.Entity_Id = m_ArchetypeRegistry.GetNewEntityId(newEntity.Archetype_Id);


		for (auto It = newArchetype.begin(); It != newArchetype.end(); ++It)
		{
			if (!m_TypeIndex_To_ComponentCompartment.contains((*It)))
			{
				//debug message -> something wrong happened
			}

			m_TypeIndex_To_ComponentCompartment.at((*It)).get()->MoveEntity(entity, newEntity);

			if (isNewArchetype)
			{
				m_ComponentIteratorRegistry.RefreshComponentIteratorsOfType((*It));
			}
		}

		IComponentCompartment* compartment = GetUnsafeComponentCompartment(typeId);

		compartment->Destroy(entity);

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

	IComponentCompartment* URegistry::GetUnsafeComponentCompartment(const std::type_index& typeId)
	{
		if (m_TypeIndex_To_ComponentCompartment.contains(typeId))
		{
			return m_TypeIndex_To_ComponentCompartment.at(typeId).get();
		}

		return nullptr;
	}

#pragma region

#pragma region DeactivateArchetype



#pragma endregion

#pragma region UArchetype

	UArchetype::UArchetype(const UArchetype& other):
		m_Archetype(other.m_Archetype)
	{
	}
	UArchetype::UArchetype(UArchetype&& other) noexcept
	{
		m_Archetype.swap(other.m_Archetype);
	}

	UArchetype& UArchetype::operator=(const UArchetype& other)
	{
		m_Archetype = other.m_Archetype;

		return *this;
	}

#pragma endregion
}