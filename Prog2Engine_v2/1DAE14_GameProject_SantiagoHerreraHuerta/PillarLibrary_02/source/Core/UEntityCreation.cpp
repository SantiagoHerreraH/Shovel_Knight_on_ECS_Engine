
#include "../../include/Core/UEntityCreation.h"

namespace Pillar {
	UArchetypeCreator::UArchetypeCreator(URegistry& registry) :
		m_Registry(registry),
		m_EntityTypesCreated(registry),
		m_ComponentBlocksCreated(registry)
	{
	}

	void UArchetypeCreator::CreateComponent(const DComponentBlockName& compName, const UArchetype& archetype)
	{
		if (!m_ComponentBlocksCreated.Get().ComponentBlocks_To_Archetype.contains(compName))
		{
			m_ComponentBlocksCreated.Get().ComponentBlocks_To_Archetype.emplace(std::pair(compName, archetype));
		}
	}
	void UArchetypeCreator::CreateArchetype(const DArchetypeName& archetypeName, const std::unordered_set<DComponentBlockName>& componentNames)
	{
		if (!m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.contains(archetypeName))
		{
			m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.emplace(std::pair(archetypeName, std::unordered_set<DComponentBlockName>{}));

			for (auto It = componentNames.begin(); It != componentNames.end(); It++)
			{
				if (m_ComponentBlocksCreated.Get().ComponentBlocks_To_Archetype.contains((*It)))
				{
					m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.at(archetypeName).emplace((*It));
				}
				
			}
		}
	}
	
	UEntityCreator::UEntityCreator(URegistry& registry):
		m_Registry(registry),
		m_EntityTypesCreated(registry),
		m_ComponentBlocksCreated(registry)
	{
	}

	void UEntityCreator::CreateEntities(UEntityHandleContainer& outEntities, FParam_EntityCreation& creationParam)
	{
		if (creationParam.NumberOfEntitiesToCreate > 0 && 
			m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.contains(creationParam.EntityTypeName))
		{
			auto begin = m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.at(creationParam.EntityTypeName).begin();
			auto end = m_EntityTypesCreated.Get().ArchetypeNames_To_ComponentNames.at(creationParam.EntityTypeName).end();

			const std::string& currentCompName =  (*begin);
			UArchetype& currentArchetype = m_ComponentBlocksCreated.Get().ComponentBlocks_To_Archetype.at(currentCompName);
			std::vector<FEntity> entities;
			m_Registry.CreateEntities(entities, creationParam.NumberOfEntitiesToCreate, currentArchetype);

			++begin;
			for (auto It = begin; It != end; It++)
			{
				for (size_t enttIdx = 0; enttIdx < entities.size(); enttIdx++)
				{
					m_Registry.
						AddComponents(
							entities.at(enttIdx),
							m_ComponentBlocksCreated.GetConst().ComponentBlocks_To_Archetype.at((*It)));
				}
			}

			m_Registry.GetEntities(outEntities , entities.at(0).Archetype_Id);
		}

		
		
	}

}