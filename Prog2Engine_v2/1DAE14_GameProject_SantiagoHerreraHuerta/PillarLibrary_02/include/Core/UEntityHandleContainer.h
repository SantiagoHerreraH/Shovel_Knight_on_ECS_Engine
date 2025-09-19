#pragma once

#include <unordered_map>

#include "../../include/Core/UEntityHandle.h"

namespace Pillar {

	/*
		to dos:
		- make a map like this
		- add a deregister function in the entity register to trigger when the containers are destructed
		- also implement tbis in the map
		- make a component wrapper (to avoid the copies generated from the sets and gets)
	
	*/

	class UEntityHandleContainer {
	public:
		UEntityHandleContainer() {}
		UEntityHandleContainer(UEntityHandleContainer& other);
		UEntityHandleContainer(const UEntityHandleContainer& other);
		UEntityHandleContainer(UEntityHandleContainer&& other)noexcept;
		virtual ~UEntityHandleContainer();

		UEntityHandleContainer& operator=(UEntityHandleContainer& other);
		UEntityHandleContainer& operator=(UEntityHandleContainer&& other)noexcept;
		UEntityHandleContainer& operator=(const UEntityHandleContainer& other);

		virtual void Reserve(size_t number);
		virtual const size_t NumOfEntities()const;
		virtual const UEntityHandle& ConstEntityAt(size_t index)const;
		virtual UEntityHandle EntityAt(size_t index);
		virtual const bool Set(UEntityHandle& entityHandle, size_t index);
		virtual const bool Contains(const UEntityHandle& entityHandle)const;
		virtual const bool Add(const UEntityHandle& entityHandle);
		virtual void SwapRemove(const UEntityHandle& entityHandle);
		virtual void Erase(const UEntityHandle& entityHandle);
		virtual void Erase(size_t index);
		virtual void Clear();

	protected:
		void RegisterAllEntityHandles();
		std::unordered_map<DHashId, size_t> m_EntityHashes_To_Index;
		std::vector<UEntityHandle> m_EntityHandles;

		UEntityHandleModifier m_EntityHandleModifier{};
		friend class UEntityHandleContainerModifier;

	};

	class UEntityHandleContainerModifier {
	public:
		void ReHash(UEntityHandleContainer* container, DHashId oldHashId, DHashId newHashId);
	};

	template<typename ComponentType>
	class UEntityHandleMap : public UEntityHandleContainer
	{
	public:
		UEntityHandleMap() {}
		UEntityHandleMap(UEntityHandleMap& other);
		UEntityHandleMap(const UEntityHandleMap& other);
		UEntityHandleMap(UEntityHandleMap&& other);
		~UEntityHandleMap() {}

		UEntityHandleMap<ComponentType>& operator=(UEntityHandleMap& other);
		UEntityHandleMap<ComponentType>& operator=(UEntityHandleMap&& other)noexcept;
		UEntityHandleMap<ComponentType>& operator=(const UEntityHandleMap& other);

		void Reserve(size_t number) override;
		const ComponentType& ConstValueAt(const UEntityHandle& handle)const;
		const ComponentType& ConstValueAt(size_t index)const;
		ComponentType& ValueAt(const UEntityHandle& handle);
		ComponentType& ValueAt(size_t index);
		const bool Set(size_t index, ComponentType& type);
		const bool Set(const UEntityHandle& entityHandle, const ComponentType& type);
		const bool Add(const UEntityHandle& entityHandle, const ComponentType& type);
		void AddOrSet(const UEntityHandle& entityHandle, const ComponentType& type);
		void SwapRemove(const UEntityHandle& entityHandle)override;
		void Erase(const UEntityHandle& entityHandle)override;
		void Erase(size_t index)override;
		void Clear()override;
		/*
			There are a couple functions here that have to be
			non const references because of the way I register
			the entity handle containers
		*/

	private:
		std::vector<ComponentType> m_Types;
	};

#pragma region Constructors

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>::UEntityHandleMap(UEntityHandleMap& other) :
		m_Types(other.m_Types)
	{
		m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
		m_EntityHandles = other.m_EntityHandles;

		RegisterAllEntityHandles();
	}

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>::UEntityHandleMap(const UEntityHandleMap& other) :
		m_Types(other.m_Types)
	{
		m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
		m_EntityHandles = other.m_EntityHandles;

		RegisterAllEntityHandles();
	}

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>::UEntityHandleMap(UEntityHandleMap&& other)
	{
		m_Types.swap(other.m_Types);
		m_EntityHashes_To_Index.swap(other.m_EntityHashes_To_Index);
		m_EntityHandles.swap(other.m_EntityHandles);

		RegisterAllEntityHandles();
	}

#pragma endregion

#pragma region Operator Overloading

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>& UEntityHandleMap<ComponentType>::operator=(UEntityHandleMap& other)
	{
		m_Types =other.m_Types;
		m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
		m_EntityHandles = other.m_EntityHandles;

		RegisterAllEntityHandles();

		return *this;
	}

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>& UEntityHandleMap<ComponentType>::operator=(const UEntityHandleMap& other)
	{
		m_Types = other.m_Types;
		m_EntityHashes_To_Index = other.m_EntityHashes_To_Index;
		m_EntityHandles = other.m_EntityHandles;

		RegisterAllEntityHandles();

		return *this;
	}

	template<typename ComponentType>
	inline UEntityHandleMap<ComponentType>& UEntityHandleMap<ComponentType>::operator=(UEntityHandleMap&& other) noexcept
	{
		m_Types.swap(other.m_Types);
		m_EntityHashes_To_Index.swap(other.m_EntityHashes_To_Index);
		m_EntityHandles.swap(other.m_EntityHandles);

		RegisterAllEntityHandles();

		return *this;
	}



#pragma endregion

#pragma region Overriden Functions

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::Reserve(size_t number)
	{
		m_EntityHashes_To_Index.reserve(number);
		m_EntityHandles.reserve(number);
		m_Types.reserve(number);
	}

	template<typename ComponentType>
	inline const ComponentType& UEntityHandleMap<ComponentType>::ConstValueAt(const UEntityHandle& handle) const
	{
		return m_Types.at(m_EntityHashes_To_Index.at(handle.GetHashId()));
	}

	template<typename ComponentType>
	inline const ComponentType& UEntityHandleMap<ComponentType>::ConstValueAt(size_t index) const
	{
		return m_Types.at(index);
	}

	template<typename ComponentType>
	inline ComponentType& UEntityHandleMap<ComponentType>::ValueAt(const UEntityHandle& handle)
	{
		return m_Types.at(m_EntityHashes_To_Index.at(handle.GetHashId()));
	}

	template<typename ComponentType>
	inline ComponentType& UEntityHandleMap<ComponentType>::ValueAt(size_t index)
	{
		return m_Types.at(index);
	}

	template<typename ComponentType>
	inline const bool UEntityHandleMap<ComponentType>::Set(size_t index, ComponentType& type)
	{
		if (index < m_Types.size())
		{
			m_Types.at(index) = type;
			return true;
		}
		return false;
	}

	template<typename ComponentType>
	inline const bool UEntityHandleMap<ComponentType>::Set(const UEntityHandle& entityHandle, const ComponentType& type)
	{
		if (entityHandle.IsValid() && m_EntityHashes_To_Index.contains(entityHandle.GetHashId()) )
		{
			size_t index = m_EntityHashes_To_Index.at(entityHandle.GetHashId());
			if (index < m_Types.size())
			{
				m_Types.at(index) = type;
				return true;
			}
		}
		return false;
	}

	template<typename ComponentType>
	inline const bool UEntityHandleMap<ComponentType>::Add(const UEntityHandle& entityHandle, const ComponentType& type)
	{
		if (UEntityHandleContainer::Add(entityHandle))
		{
			m_Types.push_back(type);
			return true;
		}

		return false;
	}

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::AddOrSet(const UEntityHandle& entityHandle, const ComponentType& type)
	{
		if (!Add(entityHandle, type))
		{
			Set(entityHandle, type);
		}
	}

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::SwapRemove(const UEntityHandle& entityHandle)
	{
		if (m_EntityHashes_To_Index.contains(entityHandle.GetHashId()))
		{
			size_t index{ m_EntityHashes_To_Index.at(entityHandle.GetHashId()) };

			std::swap(m_EntityHandles.at(index), m_EntityHandles.at(m_EntityHandles.size() - 1));
			//std::swap(m_Types[index], m_Types.at(m_Types.size() - 1)); <---- this gives an error
			m_Types.at(index) = m_Types.at(m_Types.size() - 1);

			m_EntityHashes_To_Index.at(m_EntityHandles.at(index).GetHashId()) = index;

			m_EntityHandles.pop_back();
			m_Types.pop_back();

			m_EntityHashes_To_Index.erase(entityHandle.GetHashId());
		}
	}

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::Erase(const UEntityHandle& entityHandle)
	{
		m_EntityHandles.erase(m_EntityHandles.begin() + m_EntityHashes_To_Index.at(entityHandle.GetHashId()));
		m_Types.erase(m_Types.begin() + m_EntityHashes_To_Index.at(entityHandle.GetHashId()));
		m_EntityHashes_To_Index.erase(entityHandle.GetHashId());
	}

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::Erase(size_t index)
	{
		m_EntityHashes_To_Index.erase(m_EntityHandles.at(index).GetHashId());
		m_EntityHandles.erase(m_EntityHandles.begin() + index);
		m_Types.erase(m_Types.begin() + index);
	}

	template<typename ComponentType>
	inline void UEntityHandleMap<ComponentType>::Clear()
	{
		UEntityHandleContainer::Clear();
		m_Types.clear();
	}

#pragma endregion
	
}