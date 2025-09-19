
#include "../../../include/Systems/InteractionSystems/UInteractionSystem.h"

namespace Pillar {

	UApplyDamage::UApplyDamage(URegistry& registry) :
		m_Registry(registry)
	{
		FAttack attack{};
		FEntityResolveOverlapInfo overlapInfo{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(attack, overlapInfo);

		m_Registry.GetComponents(m_Attacks, filter);
		m_Registry.GetComponents(m_OverlapInfo, filter);
	}
	void UApplyDamage::Update()
	{
		FHealth* otherHealth{ nullptr };
		UEntityHandle currentEntityHandle{};
		bool firstLoop{ true };

		for (size_t archIdx = 0; archIdx < m_OverlapInfo.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_OverlapInfo.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_Attacks.At(archIdx, compIdx).AttackedThisTick = false;

				for (size_t i = 0; i < m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); i++)
				{
					otherHealth = m_Registry.GetComponent<FHealth>(m_OverlapInfo.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.EntityAt(i).GetConstEntity());

					if (otherHealth && !otherHealth->CurrentlyImmune) // here you have to check if nullptr because you are not sure the other entity has the health component
					{
						m_Registry.GetEntity<FEntityResolveOverlapInfo>(currentEntityHandle, m_OverlapInfo.GetArchetype(archIdx), compIdx);
						otherHealth->EntitiesBeingAttackedBy.Add(currentEntityHandle);

						if (!otherHealth->Wounded)
						{
							otherHealth->CurrentHealth -= m_Attacks.At(archIdx, compIdx).Damage;
							otherHealth->Wounded = true;
						}

						m_Attacks.At(archIdx, compIdx).AttackedThisTick = true;
					}
				}
			}
		}
	}

	UUpdateWoundDuration::UUpdateWoundDuration(URegistry& registry) :
		m_Health(FHealth(0))
	{
		registry.GetComponents(m_Health);
	}
	void UUpdateWoundDuration::Update(float deltaSeconds)
	{
		int currentDeltaHealth{0};

		for (size_t archIdx = 0; archIdx < m_Health.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_Health.NumOfActivatedComponents(archIdx); compIdx++)
			{

				if (m_Health.At(archIdx, compIdx).Wounded)
				{
					m_Health.At(archIdx, compIdx).CurrentWoundTime += deltaSeconds;

					if (m_Health.At(archIdx, compIdx).CurrentWoundTime >= m_Health.At(archIdx, compIdx).WoundDuration)
					{
						m_Health.At(archIdx, compIdx).CurrentWoundTime = 0;
						m_Health.At(archIdx, compIdx).Wounded = false;
					}
				}
				else
				{
					m_Health.At(archIdx, compIdx).EntitiesBeingAttackedBy.Clear();
				}
				if (m_Health.At(archIdx, compIdx).CurrentHealth <= 0)
				{
					//m_Health.At(archIdx, compIdx).CurrentDeathTime += deltaSeconds;
					//
					//if (m_Health.At(archIdx, compIdx).CurrentDeathTime >= m_Health.At(archIdx, compIdx).DeathDuration)
					//{
					//	m_Health.At(archIdx, compIdx).CurrentDeathTime = 0; 
					//}
					//

					m_Health.At(archIdx, compIdx).CurrentHealth = m_Health.At(archIdx, compIdx).MaxHealth;
					
				}
			}
		}
	}

	UInteractionSystem::UInteractionSystem(URegistry& registry) :
		m_ApplyDamage(registry),
		m_UpdateWoundDuration(registry)
	{
	}
	void UInteractionSystem::Update(float deltaSeconds)
	{
		m_UpdateWoundDuration.Update(deltaSeconds);
		m_ApplyDamage.Update();
	}

}