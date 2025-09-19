#pragma once

#include "../../include/Core/URegistry.h"

namespace Pillar {

	/*
		NOT FINISHED -> DO AFTERWARDS
	*/
	
	template<typename ComponentType>
	class URegistrySingletonArray {
	public:
		URegistrySingletonArray(URegistry& registry, size_t numberOfComponents, bool canCreateNewComponentsOfType, bool componentsCanBeDeactivated = false);
		std::vector<ComponentType>& Get();
		const std::vector<ComponentType>& GetConst() const;

	private:
		UComponentIterator<ComponentType> m_ComponentArray;

	};
	
	template<typename ComponentType>
	inline URegistrySingletonArray<ComponentType>::URegistrySingletonArray(URegistry& registry, size_t numberOfComponents, bool canCreateNewComponentsOfType, bool componentsCanBeDeactivated)
	{
		registry.GetActivatedComponents(m_ComponentArray);
		ComponentType component;
		std::vector<FEntity> entities;

		if (m_ComponentArray.IsEmpty())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.CreateEntities(entities, numberOfComponents, component);
			
		}
		if (m_ComponentArray.NumOfArchetypes() != 0)
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.ClearComponentsOfType<ComponentType>();
			registry.CreateEntities(entities, numberOfComponents, component);
		}
		else if (!m_ComponentArray.HasSingleComponent())
		{
			registry.UnlockCreationAndDestructionOfComponentType<ComponentType>();
			registry.ClearComponentsOfType<ComponentType>();
			registry.CreateEntities(entities, component);
			registry.LockCreationAndDestructionOfComponentType<ComponentType>();
		}

		if (!canCreateNewComponentsOfType)
		{

		}
		if (!componentsCanBeDeactivated)
		{
			registry.LockActivationAndDeactivationOfComponentType<ComponentType>();
		}
	}

	template<typename ComponentType>
	inline std::vector<ComponentType>& URegistrySingletonArray<ComponentType>::Get()
	{
		// TODO: insert return statement here
	}

}