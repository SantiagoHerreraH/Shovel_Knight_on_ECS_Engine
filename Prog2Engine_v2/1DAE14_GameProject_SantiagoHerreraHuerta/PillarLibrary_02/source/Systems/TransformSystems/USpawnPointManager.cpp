
#include "../../../include/Systems/TransformSystems/USpawnPointManager.h"

namespace Pillar {
	USpawnPointManager::USpawnPointManager() {}

	bool USpawnPointManager::Contains(const UEntityHandle& spawnPoint)
	{
		return m_SpawnPoints.Contains(spawnPoint);
	}
	void USpawnPointManager::AddSpawnPoint(const UEntityHandle& spawnPoint)
	{
		m_SpawnPoints.Add(spawnPoint);
	}

	void USpawnPointManager::RemoveSelectedSpawnPoint()
	{
		m_SpawnPoints.Erase(m_CurrentSpawnPoint);
	}

	const UEntityHandle& USpawnPointManager::GetSelectedSpawnPoint() const
	{
		if (m_CurrentSpawnPoint >= m_SpawnPoints.NumOfEntities())
		{
			return UEntityHandle{};
		}

		return m_SpawnPoints.ConstEntityAt(m_CurrentSpawnPoint);
	}

	void USpawnPointManager::SelectLastSpawnPoint()
	{
		if (m_CurrentSpawnPoint > 0)
		{
			--m_CurrentSpawnPoint;
		}
	}

	void USpawnPointManager::SelectNextSpawnPoint()
	{
		if (m_CurrentSpawnPoint < (m_SpawnPoints.NumOfEntities() - 1))
		{
			++m_CurrentSpawnPoint;
		}
	}
	
}