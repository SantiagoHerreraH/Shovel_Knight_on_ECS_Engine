#include "../../../include/Systems/StateSystems/UpdateStateSystems3.h"
#include "../../../include/Structs/FTag.h"

namespace Pillar {

	UUpdateState_SpawnManager::UUpdateState_SpawnManager(URegistry& registry) :
		m_Registry(registry),
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FState<FSpawnPointState>{}, FEntityResolveOverlapInfo{}, FWorldTransform2D{}, USpawnPointManager{}, UStateMachine{ registry, UEntityHandle{} });

		registry.GetComponents(m_SpawnPointStates, filter);
		registry.GetComponents(m_OverlapInfos, filter);
		registry.GetComponents(m_WorldTransforms, filter);
		registry.GetComponents(m_SpawnPointManagers, filter);
		registry.GetComponents(m_StateMachines, filter);
	}

	void UUpdateState_SpawnManager::Update()
	{
		m_Registry.Lock<URealTransform2D>();

		FTag* currentTag{ nullptr };
		const UEntityHandle* currentSpawnPoint{ nullptr };
		FWorldTransform2D* spawnPointWorldTransform{ nullptr };

		for (size_t archIdx = 0; archIdx < m_SpawnPointManagers.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SpawnPointManagers.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					switch (m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).SpawnPointsAction)
					{
					case ESpawnPointsAction::SpawnInLast:

						currentSpawnPoint = &m_SpawnPointManagers.At(archIdx, compIdx).GetSelectedSpawnPoint();
						spawnPointWorldTransform = m_Registry.GetComponent<FWorldTransform2D>(currentSpawnPoint->GetConstEntity());

						m_WorldTransforms.At(archIdx, compIdx).Transform.Position =
							spawnPointWorldTransform->Transform.Position;


						break;

					case ESpawnPointsAction::AddSpawnPoint:

						for (size_t enttIdx = 0; enttIdx < m_OverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.NumOfEntities(); enttIdx++)
						{
							currentSpawnPoint = &m_OverlapInfos.At(archIdx, compIdx).Entities_To_ResolveOverlapInfo.ConstEntityAt(enttIdx);

							currentTag = m_Registry.GetComponent<FTag>(currentSpawnPoint->GetConstEntity());

							if (currentTag && currentTag->Name ==
								m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).SpawnPointTagName &&
								!m_SpawnPointManagers.At(archIdx, compIdx).Contains(*currentSpawnPoint))
							{
								m_SpawnPointManagers.At(archIdx, compIdx).AddSpawnPoint(*currentSpawnPoint);
								m_SpawnPointManagers.At(archIdx, compIdx).SelectNextSpawnPoint();
								break;
							}
						}

						break;

					default:
						break;
					}

					switch (m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_SpawnManager>(m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_SpawnPointStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ForcedRemoval);
						break;
					}
				}
			}
		}
		m_Registry.Unlock<URealTransform2D>();
	}
	UUpdateState_SceneChanger::UUpdateState_SceneChanger(URegistry& registry) :
		m_StateMachines(UStateMachine{ registry , UEntityHandle{} }),
		m_SceneManagementSystem(USceneManagementSystem{ registry })
	{
		UComponentFilter filter{};

		filter.SetIncludingFilter(FState<FSceneState>{}, UStateMachine{ registry , UEntityHandle{} });

		registry.GetComponents(m_SceneStates, filter);
		registry.GetComponents(m_StateMachines, filter);
	}
	void UUpdateState_SceneChanger::Update()
	{
		int sceneDelta{0};

		for (size_t archIdx = 0; archIdx < m_SceneStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SceneStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_SceneStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					sceneDelta += m_SceneStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).SceneDeltaFromCurrentState;

					switch (m_SceneStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_SpawnManager>(m_SceneStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_SceneStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ForcedRemoval);
						break;
					}
				}
			}
		}

		if (sceneDelta != 0)
		{
			m_SceneManagementSystem.SkipToScene(sceneDelta);
		}
	}
	UUpdateState_SoundEffects::UUpdateState_SoundEffects(URegistry& registry) :
		m_StateMachines(UStateMachine(registry, UEntityHandle{}))
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(UStateMachine(registry, UEntityHandle{}), FState<FSoundEffectState>{});

		registry.GetComponents(m_StateMachines, filter);
		registry.GetComponents(m_SoundEffectStates, filter);
		
	}
	void UUpdateState_SoundEffects::Update()
	{
		for (size_t archIdx = 0; archIdx < m_SoundEffectStates.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SoundEffectStates.NumOfActivatedComponents(archIdx); compIdx++)
			{
				for (size_t stateIdx = 0; stateIdx < m_SoundEffectStates.At(archIdx, compIdx).EventName_To_State.ActivatedSize(); stateIdx++)
				{
					SoundEffect& soundEffect{ SSound::GetSoundEffect(m_SoundEffectStates.At(archIdx,compIdx).EventName_To_State.At(stateIdx).Name) };
					soundEffect.Play(m_SoundEffectStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).LoopNumber); //check if this is a loop or if it is a one time thing
					
					switch (m_SoundEffectStates.At(archIdx, compIdx).EventName_To_State.At(stateIdx).StateRemovalType)
					{
					case EStateRemovalType::Local:
						m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval
							<DStateController_SoundEffects>(m_SoundEffectStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx));
						break;
					case EStateRemovalType::AllStateMachines:
						m_StateMachines.At(archIdx, compIdx).TriggerState(m_SoundEffectStates.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(stateIdx), EEventStateToTrigger::ForcedRemoval);
						break;
					}
				}
			}
		}
	}
}