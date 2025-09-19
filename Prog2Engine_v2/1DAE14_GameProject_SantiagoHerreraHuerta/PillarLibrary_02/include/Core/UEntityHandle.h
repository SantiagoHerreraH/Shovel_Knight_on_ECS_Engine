#pragma once

#include <unordered_map>

#include "../../include/Core/FEntity.h"

namespace Pillar {

	class UEntityHandleModifier;
	class UEntityHandleRegistry;

	class UEntityHandle {

	public:
		UEntityHandle() {}
		UEntityHandle(const UEntityHandle& other);
		UEntityHandle(UEntityHandle&& other)noexcept;
		~UEntityHandle();
		const FEntity& GetConstEntity() const { return m_Entity; }
		const DHashId GetHashId() const { return m_HashId; }
		const bool IsValid() const { return m_EntityRegistry != nullptr; }

		UEntityHandle& operator=(UEntityHandle& other);
		UEntityHandle& operator=(const UEntityHandle& other);

	private:
		FEntity m_Entity{};
		DHashId m_HashId{0};
		UEntityHandleRegistry* m_EntityRegistry{nullptr};
		friend class UEntityHandleModifier;
	};

	bool operator==(const UEntityHandle& lhs, const UEntityHandle& rhs);

	class UEntityHandleModifier {
	public:
		void SetEntityInformation(UEntityHandle& entityHandle, UEntityHandleRegistry* entityRegistry, FEntity newEntity, DHashId hashId);
		void DeleteEntity(UEntityHandle& entityHandle);
		UEntityHandleRegistry* const GetEntityRegistry(UEntityHandle& entityHandle);
	};
}