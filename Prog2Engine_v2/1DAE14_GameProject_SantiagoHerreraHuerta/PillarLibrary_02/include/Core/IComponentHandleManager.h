#pragma once

#include "../../include/Core/UEntityHandle.h"

namespace Pillar {

	class IComponentHandle {

	public:
		virtual UEntityHandle& EntityHandle() = 0;
	};

	class IComponentHandleManager {

	public:
		virtual void RefreshAllComponentHandles() = 0;
		virtual void RefreshEntity(const FEntity& entity) = 0;
		virtual void ChangeEntity(const FEntity& oldEntity, const FEntity& newEntity) = 0;
		virtual void RefreshArchetype(DArchetypeId archetype) = 0;
		virtual void DeleteEntity(const FEntity& entity) = 0;
		virtual void DeregisterComponentHandle(IComponentHandle* componentHandle) = 0;
		virtual void Clear() = 0;
	};
}