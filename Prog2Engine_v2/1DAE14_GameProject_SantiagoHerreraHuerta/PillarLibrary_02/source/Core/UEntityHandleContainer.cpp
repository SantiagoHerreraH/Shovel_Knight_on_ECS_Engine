
#include "../../include/Core/UEntityHandleContainer.h"
#include "../../include/Core/UEntityHandleRegistry.h"

namespace Pillar {

	UEntityHandleContainer::UEntityHandleContainer(UEntityHandleContainer& other) :
		m_EntityHashes_To_Index(other.m_EntityHashes_To_Index),
		m_EntityHandles(other.m_EntityHandles)
	{
		RegisterAllEntityHandles();
	}

	UEntityHandleContainer::UEntityHandleContainer(const UEntityHandleContainer& other) :
		m_EntityHashes_To_Index(other.m_EntityHashes_To_Index),
		m_EntityHandles(other.m_EntityHandles)
	{
		RegisterAllEntityHandles();
	}

	UEntityHandleContainer::UEntityHandleContainer(UEntityHandleContainer&& other) noexcept
	{
		m_EntityHandles.swap(other.m_EntityHandles);
		m_EntityHashes_To_Index.swap(other.m_EntityHashes_To_Index);
		RegisterAllEntityHandles();
	}

	UEntityHandleContainer::~UEntityHandleContainer()
	{
		for (size_t i = 0; i < m_EntityHandles.size(); i++)
		{
			m_EntityHandleModifier.GetEntityRegistry(m_EntityHandles[i])->UnbindEntityContainerFromEntity(this, m_EntityHandles[i]);
		}
	}

	void UEntityHandleContainer::RegisterAllEntityHandles()
	{
		for (size_t i = 0; i < m_EntityHandles.size(); i++)
		{
			m_EntityHandleModifier.GetEntityRegistry(m_EntityHandles[i])->BindEntityContainerToEntity(this, m_EntityHandles[i]);
		}
	}

#pragma region OperatorOverloading

	UEntityHandleContainer& UEntityHandleContainer::operator=(UEntityHandleContainer& other)
	{

		if (&other != this)
		{
			m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
			m_EntityHandles = other.m_EntityHandles;
			RegisterAllEntityHandles();
		}

		return *this;
	}
	UEntityHandleContainer& UEntityHandleContainer::operator=(const UEntityHandleContainer& other)
	{
		if (&other != this)
		{
			m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
			m_EntityHandles = other.m_EntityHandles;
			RegisterAllEntityHandles();
		}
		

		return *this;
	}
	UEntityHandleContainer& UEntityHandleContainer::operator=(UEntityHandleContainer&& other)noexcept
	{
		if (&other != this)
		{
			m_EntityHashes_To_Index.swap(other.m_EntityHashes_To_Index);
			m_EntityHandles.swap(other.m_EntityHandles);
			RegisterAllEntityHandles();
		}
		
		return *this;
	}

#pragma endregion 

	void UEntityHandleContainer::Reserve(size_t number)
	{
		m_EntityHashes_To_Index.reserve(number);
		m_EntityHandles.reserve(number);
	}

	const size_t Pillar::UEntityHandleContainer::NumOfEntities() const
	{
		return m_EntityHandles.size();
	}

	const UEntityHandle& Pillar::UEntityHandleContainer::ConstEntityAt(size_t index) const
	{
		return m_EntityHandles.at(index);
	}

	UEntityHandle UEntityHandleContainer::EntityAt(size_t index)
	{
		return m_EntityHandles.at(index);
	}

	const bool UEntityHandleContainer::Set(UEntityHandle& entityHandle, size_t index)
	{
		if (!m_EntityHashes_To_Index.contains(entityHandle.GetHashId()) && index < m_EntityHandles.size() && entityHandle.IsValid())
		{
			m_EntityHashes_To_Index.emplace(std::pair(entityHandle.GetHashId(), index));
			m_EntityHandles.at(index) = entityHandle;

			UEntityHandleRegistry* handleRegistry{ m_EntityHandleModifier.GetEntityRegistry(entityHandle) };
			if (handleRegistry)
			{
				handleRegistry->BindEntityContainerToEntity(this, m_EntityHandles.at(index));
			}

			return true;
		}

		return false;
	}

	const bool Pillar::UEntityHandleContainer::Contains(const UEntityHandle& entityHandle)const
	{
		return m_EntityHashes_To_Index.contains(entityHandle.GetHashId());
	}

	const bool Pillar::UEntityHandleContainer::Add(const UEntityHandle& entityHandle)
	{
		if (entityHandle.IsValid() && !m_EntityHashes_To_Index.contains(entityHandle.GetHashId()))
		{
			m_EntityHashes_To_Index.emplace(std::pair(entityHandle.GetHashId(), m_EntityHandles.size()));
			m_EntityHandles.push_back(entityHandle);
			m_EntityHandleModifier.GetEntityRegistry(const_cast<UEntityHandle&>(entityHandle))->BindEntityContainerToEntity(this, m_EntityHandles.at(m_EntityHandles.size() - 1));

			return true;
		}

		return false;
	}

	void Pillar::UEntityHandleContainer::SwapRemove(const UEntityHandle& entityHandle)
	{
		if (m_EntityHashes_To_Index.contains(entityHandle.GetHashId()))
		{
			size_t index{ m_EntityHashes_To_Index.at(entityHandle.GetHashId()) };
			std::swap(m_EntityHandles.at(index), m_EntityHandles.at(m_EntityHandles.size() - 1));
			m_EntityHashes_To_Index.at(m_EntityHandles.at(index).GetHashId()) = index;
			m_EntityHandles.pop_back();

			m_EntityHashes_To_Index.erase(entityHandle.GetHashId());
		}
	}

	void UEntityHandleContainer::Erase(const UEntityHandle& entityHandle)
	{
		m_EntityHandles.erase(m_EntityHandles.begin() + m_EntityHashes_To_Index.at(entityHandle.GetHashId()));
		m_EntityHashes_To_Index.erase(entityHandle.GetHashId());
	}

	void UEntityHandleContainer::Erase(size_t index)
	{
		m_EntityHashes_To_Index.erase(m_EntityHandles.at(index).GetHashId());
		m_EntityHandles.erase(m_EntityHandles.begin() + index);
	}

	void Pillar::UEntityHandleContainer::Clear()
	{
		m_EntityHashes_To_Index.clear();
		m_EntityHandles.clear();
	}

	void UEntityHandleContainerModifier::ReHash(UEntityHandleContainer* container, DHashId oldHashId, DHashId newHashId)
	{
		if (container->m_EntityHashes_To_Index.contains(oldHashId))
		{
			size_t index{ container->m_EntityHashes_To_Index.at(oldHashId)};
			container->m_EntityHashes_To_Index.emplace(std::pair(newHashId, index));
			container->m_EntityHashes_To_Index.erase(oldHashId);
		}
	}

}


