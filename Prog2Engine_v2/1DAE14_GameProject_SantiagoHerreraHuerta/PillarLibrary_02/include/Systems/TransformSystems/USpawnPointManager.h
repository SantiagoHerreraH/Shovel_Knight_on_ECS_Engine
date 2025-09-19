#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UEntityHandleContainer.h"
#include "../../../include/Structs/FState.h"

namespace Pillar {

	enum class ESpawnPointsAction {
		AddSpawnPoint,
		SpawnInLast
	};

	struct FSpawnPointState {
		ESpawnPointsAction SpawnPointsAction{};
		EStateRemovalType StateRemovalType{};
		std::string SpawnPointTagName{"SpawnPoint"};
	};

	class USpawnPointManager {

	public:
		USpawnPointManager();

		bool Contains(const UEntityHandle& spawnPoint);
		void AddSpawnPoint(const UEntityHandle& spawnPoint);
		void RemoveSelectedSpawnPoint();
		const UEntityHandle& GetSelectedSpawnPoint()const;
		void SelectLastSpawnPoint();
		void SelectNextSpawnPoint();

	private:
		UEntityHandleContainer m_SpawnPoints;
		size_t m_CurrentSpawnPoint{0};
	};
}