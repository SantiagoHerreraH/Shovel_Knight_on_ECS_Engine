#pragma once

#include <typeindex>
#include <vector>
#include <unordered_set>

namespace Pillar {

	using DEntityId = int;
	using DEntities = std::vector<DEntityId>;
	using DArchetype = std::unordered_set<std::type_index>;
	using DArchetypeKey = std::vector<bool>;
	using DArchetypeId = int;
	using DComponentId = int;
	using DHashId = long long;
	extern const DEntityId g_DEFAULT_ENTITY;
	extern const DArchetypeId g_DEFAULT_ARCHETYPE_ID;

	class UEntityHandleRegistry;
	class FEntityModifier;

	struct FEntity {
		DArchetypeId Archetype_Id;
		DEntityId Entity_Id;
	};

	

}