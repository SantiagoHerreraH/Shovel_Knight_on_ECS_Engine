#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Structs/FCollision.h"
#include "../../../include/Systems/TransformSystems/USpawnPointManager.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"
#include "../../../include/Systems/SceneSystems/USceneGraph.h"
#include "../../../include/Systems/SoundSystems/USound.h"

namespace Pillar {

	
	using DStateController_SpawnManager =		UStateController<FSpawnPointState,			false,	true>;
	using DStateController_SceneChanger =		UStateController<FSceneState,				false,	true>;
	using DStateController_SoundEffects =		UStateController<FSoundEffectState,				true,	true>;


	class UUpdateState_SpawnManager {
	public:
		UUpdateState_SpawnManager(URegistry& registry);
		void Update();

	private:
		URegistry& m_Registry;
		UComponentIterator<FState<FSpawnPointState>>	m_SpawnPointStates;
		UComponentIterator<FEntityResolveOverlapInfo>	m_OverlapInfos;
		UComponentIterator<FWorldTransform2D>			m_WorldTransforms;
		UComponentIterator<USpawnPointManager>			m_SpawnPointManagers;
		UComponentIterator<UStateMachine>				m_StateMachines;

		URealTransform2DModifier m_RealTransformModifier;
	};


	class UUpdateState_SceneChanger {
	public:
		UUpdateState_SceneChanger(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FSceneState>>			m_SceneStates;
		UComponentIterator<UStateMachine>				m_StateMachines;

		USceneManagementSystem m_SceneManagementSystem;
	};

	class UUpdateState_SoundEffects {

	public:
		UUpdateState_SoundEffects(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FState<FSoundEffectState>> m_SoundEffectStates;
		UComponentIterator<UStateMachine> m_StateMachines;
	};
}