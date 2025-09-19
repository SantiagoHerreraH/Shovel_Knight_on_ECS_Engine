#pragma once 

#include <vector>
#include <string>
#include <memory>

#include "../../include/Core/FEntity.h"
#include "../../include/Core/URegistry.h"
#include "../../include/Core/URegistrySingleton.h"

namespace Pillar {

	using DComponentBlockName = std::string; 
	//later you can put the archetype here, but in general it will be a lot of work for little output at the moment

	using DArchetypeName = std::string;

	struct FParam_EntityCreation {
		DArchetypeName EntityTypeName;
		size_t NumberOfEntitiesToCreate;
	};

	struct FRegistry_ArchetypeCreation {
		std::unordered_map<DComponentBlockName, UArchetype> ComponentBlocks_To_Archetype;
	};

	struct FRegistry_WorldCreation {
		std::unordered_map<DArchetypeName, std::unordered_set<DComponentBlockName>> ArchetypeNames_To_ComponentNames;
	};

	class UArchetypeCreator{
	
	public:
		UArchetypeCreator(URegistry& registry);
		void CreateComponent(const DComponentBlockName& compName, const UArchetype& archetype);
		void CreateArchetype(const DArchetypeName& archetypeName, const std::unordered_set<DComponentBlockName>& componentNames);

	private:
		URegistry& m_Registry;

		URegistrySingleton<FRegistry_WorldCreation> m_EntityTypesCreated;
		URegistrySingleton<FRegistry_ArchetypeCreation> m_ComponentBlocksCreated;

	};

	class UEntityCreator {

	public:
		UEntityCreator(URegistry& registry);
		void CreateEntities(UEntityHandleContainer& outEntities, FParam_EntityCreation& creationParam);

	private:
		URegistry& m_Registry;

		URegistrySingleton<FRegistry_WorldCreation>		m_EntityTypesCreated;
		URegistrySingleton<FRegistry_ArchetypeCreation> m_ComponentBlocksCreated;

	};
}