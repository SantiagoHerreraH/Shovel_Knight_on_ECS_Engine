#pragma once

#include "../../include/Core/URegistry.h"

namespace Pillar {

	template<typename ComponentType>
	class URegistrySingleton {
	public:
		URegistrySingleton(URegistry& registry, bool componentsCanBeDeactivated = false);
		URegistrySingleton(URegistry& registry, const ComponentType& component, bool componentsCanBeDeactivated = false);

		URegistrySingleton<ComponentType>& operator=(const URegistrySingleton& other);

		ComponentType& Get();
		const ComponentType& GetConst() const;

	private:
		URegistry* m_Registry;
		UComponentIterator<ComponentType> m_ComponentIterator;

	};
	template<typename ComponentType>
	inline URegistrySingleton<ComponentType>::URegistrySingleton(URegistry& registry, bool componentsCanBeDeactivated) :
		m_Registry(&registry)
	{
		registry.GetComponents(m_ComponentIterator);
		ComponentType component;
		FEntity entity;

		if (m_ComponentIterator.IsEmpty())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.CreateEntity(entity, component);
			registry.LockCreationAndDestructionOfComponentType<ComponentType>();
		}
		else if (!m_ComponentIterator.HasSingleComponent())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.ClearComponentsOfType<ComponentType>();
			registry.CreateEntity(entity, component);
			registry.LockCreationAndDestructionOfComponentType<ComponentType>();
		}

		if (!componentsCanBeDeactivated)
		{
			registry.LockActivationAndDeactivationOfComponentType<ComponentType>();
		}
	}
	template<typename ComponentType>
	inline URegistrySingleton<ComponentType>::URegistrySingleton(URegistry& registry, const ComponentType& component, bool componentsCanBeDeactivated) :
		m_Registry(registry)
	{
		registry.GetComponents(m_ComponentIterator);
		FEntity entity;

		if (m_ComponentIterator.IsEmpty())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.CreateEntity(entity, component);
			registry.LockCreationAndDestructionOfComponentType<ComponentType>();
		}
		else if (!m_ComponentIterator.HasSingleComponent())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.ClearComponentsOfType<ComponentType>();
			registry.CreateEntity(entity, component);
			registry.LockCreationAndDestructionOfComponentType<ComponentType>();
		}

		if (!componentsCanBeDeactivated)
		{
			registry.LockActivationAndDeactivationOfComponentType<ComponentType>();
		}
	}

	template<typename ComponentType>
	inline URegistrySingleton<ComponentType>& URegistrySingleton<ComponentType>::operator=(const URegistrySingleton& other)
	{
		m_Registry = other.m_Registry;
		m_Registry->GetComponents(m_ComponentIterator);

		return *this;
	}
	
	template<typename ComponentType>
	inline ComponentType& URegistrySingleton<ComponentType>::Get()
	{
		return m_ComponentIterator.At(0,0);
	}
	template<typename ComponentType>
	inline const ComponentType& URegistrySingleton<ComponentType>::GetConst() const
	{
		return m_ComponentIterator.ConstAt(0, 0);
	}
}