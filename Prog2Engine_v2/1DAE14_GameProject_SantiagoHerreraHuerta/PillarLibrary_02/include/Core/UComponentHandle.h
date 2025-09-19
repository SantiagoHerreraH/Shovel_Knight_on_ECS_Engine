#pragma once

#include "../../include/Core/IComponentHandleManager.h"
#include "../../include/Core/UEntityHandle.h"

namespace Pillar {


	template<typename ComponentType>
	class UComponentHandle : IComponentHandle {

	public:
		~UComponentHandle<ComponentType>();

		const bool IsValid() const { return m_ComponentPtr != nullptr && m_EntityHandle.IsValid(); }

		ComponentType* const Component() { return m_ComponentPtr; }
		const ComponentType* ConstComponent() const { return m_ComponentPtr; }
		UEntityHandle& EntityHandle() override { return m_EntityHandle; };
		const UEntityHandle& ConstEntityHandle() const { return m_EntityHandle; }

	private:
		ComponentType* m_ComponentPtr{nullptr};
		UEntityHandle m_EntityHandle;
		IComponentHandleManager* m_ComponentHandleManager{nullptr};
		friend class UComponentHandleModifier;
	};

	template<typename ComponentType>
	inline UComponentHandle<ComponentType>::~UComponentHandle()
	{
		if (m_ComponentHandleManager)
		{
			m_ComponentHandleManager->DeregisterComponentHandle(this);
		}
	}

	class UComponentHandleModifier {
		
	public:
		template<typename ComponentType>
		void SetComponentInfo(UComponentHandle<ComponentType>& componentHandle, ComponentType* component, const UEntityHandle& entityHandle, IComponentHandleManager* manager);
		template<typename ComponentType>
		void NullComponentHandle(UComponentHandle<ComponentType>& componentHandle);
	};
	template<typename ComponentType>
	inline void UComponentHandleModifier::SetComponentInfo(UComponentHandle<ComponentType>& componentHandle, ComponentType* component, const UEntityHandle& entityHandle, IComponentHandleManager* manager)
	{
		componentHandle.m_ComponentPtr = component;
		componentHandle.m_EntityHandle = entityHandle;
		componentHandle.m_ComponentHandleManager = manager;
	}

	template<typename ComponentType>
	inline void UComponentHandleModifier::NullComponentHandle(UComponentHandle<ComponentType>& componentHandle)
	{
		componentHandle.m_ComponentPtr = nullptr;
		componentHandle.m_ComponentHandleManager = nullptr;
	}
	
}