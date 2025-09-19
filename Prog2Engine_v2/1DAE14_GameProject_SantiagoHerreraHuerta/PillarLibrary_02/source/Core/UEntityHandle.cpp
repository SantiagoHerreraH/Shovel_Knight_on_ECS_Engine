#include "../1DAE14_GameProject_SantiagoHerreraHuerta/pch.h"
#include "../../include/Core/UEntityHandle.h"
#include "../../include/Core/UEntityHandleRegistry.h"

namespace Pillar {

	void UEntityHandleModifier::SetEntityInformation(UEntityHandle& entityHandle, UEntityHandleRegistry* entityRegistry, FEntity newEntity, DHashId hashId)
	{
		entityHandle.m_Entity = newEntity;
		entityHandle.m_EntityRegistry = entityRegistry;
		entityHandle.m_HashId = hashId;
	}

	void UEntityHandleModifier::DeleteEntity(UEntityHandle& entityHandle)
	{
		entityHandle.m_EntityRegistry = nullptr;
		entityHandle.m_Entity = {0,0};
		entityHandle.m_HashId = 0;
	}

	UEntityHandleRegistry* const UEntityHandleModifier::GetEntityRegistry(UEntityHandle& entityHandle)
	{
		return entityHandle.m_EntityRegistry;
	}



	UEntityHandle::UEntityHandle(const UEntityHandle& other)
	{
		if (other.m_EntityRegistry)
		{
			m_EntityRegistry = other.m_EntityRegistry;
			m_Entity = other.m_Entity;
			m_HashId = other.m_HashId;
			m_EntityRegistry->RegisterOrUpdateEntityHandle(*this, other.m_Entity, m_HashId);
		}
	}

	UEntityHandle::UEntityHandle(UEntityHandle&& other) noexcept
	{

		if (other.m_EntityRegistry)
		{
			m_EntityRegistry = other.m_EntityRegistry;
			m_Entity = other.m_Entity;
			m_HashId = other.m_HashId;
			m_EntityRegistry->RegisterOrUpdateEntityHandle(*this, other.m_Entity, m_HashId); //not necessary changing the other 
		}
	}

	UEntityHandle::~UEntityHandle()
	{
		if (m_EntityRegistry)
		{
			m_EntityRegistry->DeregisterEntityHandle(*this);
			m_EntityRegistry = nullptr;
		}
	}

	UEntityHandle& UEntityHandle::operator=(UEntityHandle& other)
	{
		if (m_EntityRegistry)
		{
			m_EntityRegistry->DeregisterEntityHandle(*this);
		}

		m_EntityRegistry = other.m_EntityRegistry;
		m_Entity = other.m_Entity;
		m_HashId = other.m_HashId;

		if (other.m_EntityRegistry)
		{
			m_EntityRegistry->RegisterOrUpdateEntityHandle(*this, other.m_Entity, m_HashId);
		}

		return *this;
	}


	UEntityHandle& UEntityHandle::operator=(const UEntityHandle& other)
	{
		if (m_EntityRegistry)
		{
			m_EntityRegistry->DeregisterEntityHandle(*this);
		}

		m_EntityRegistry = other.m_EntityRegistry;
		m_Entity = other.m_Entity;
		m_HashId = other.m_HashId;

		if (other.m_EntityRegistry)
		{
			m_EntityRegistry->RegisterOrUpdateEntityHandle(*this, other.m_Entity, m_HashId);
		}

		return *this;
	}

	bool operator==(const UEntityHandle& lhs, const UEntityHandle& rhs)
	{
		return
			(lhs.GetConstEntity().Archetype_Id == rhs.GetConstEntity().Archetype_Id) &&
			(lhs.GetConstEntity().Entity_Id == rhs.GetConstEntity().Entity_Id);
	}

}

