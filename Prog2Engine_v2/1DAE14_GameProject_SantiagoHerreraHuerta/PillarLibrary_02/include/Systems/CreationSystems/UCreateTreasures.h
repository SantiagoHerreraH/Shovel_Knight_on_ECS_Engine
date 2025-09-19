#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UEntityCreation.h"
#include "../../../include/Structs/FVector.h"

#include "../../../../Engine/structs.h"

namespace Pillar {
	 
	class UCreateTreasures {

	public:
		UCreateTreasures(URegistry& registry);
		void CreateRandomDropableTreasures(const UEntityHandle& entityToDrop, int minNumberOfTreasures, int maxNumberOfTreasures, size_t sceneIndex);
		void CreateRandomTreasure(size_t sceneIndex, const FPoint2f& location);

	private:
		void CreateDropableTreasure(const UEntityHandle& entityToDrop, const FPoint2f& size, const Rectf& spriteSheetBounds);
		void CreateTreasure(size_t sceneIndex, const FPoint2f& location, const FPoint2f& size, const Rectf& spriteSheetBounds);


		void CreateGreyTreasure_Dropable(const UEntityHandle& entityToDrop);
		void CreateBlueTreasure_Dropable(const UEntityHandle& entityToDrop);
		void CreateGreenTreasure_Dropable(const UEntityHandle& entityToDrop);
		void CreateYellowTreasure_Dropable(const UEntityHandle& entityToDrop);
		void CreateRedTreasure_Dropable(const UEntityHandle& entityToDrop);
		void CreateMagentaTreasure_Dropable(const UEntityHandle& entityToDrop);

		void CreateGreyTreasure(size_t sceneIndex, const FPoint2f& location);
		void CreateBlueTreasure(size_t sceneIndex, const FPoint2f& location);
		void CreateGreenTreasure(size_t sceneIndex, const FPoint2f& location);
		void CreateYellowTreasure(size_t sceneIndex, const FPoint2f& location);
		void CreateRedTreasure(size_t sceneIndex, const FPoint2f& location);
		void CreateMagentaTreasure(size_t sceneIndex, const FPoint2f& location);

		const Rectf m_GreyRect{			Rectf(0, 24, 31, 24) };
		const Rectf m_BlueRect{			Rectf(32, 24, 31, 24) };
		const Rectf m_GreenRect{		Rectf((2 * 32), 24, 31, 24) };
		const Rectf m_YellowRect{		Rectf((3 * 32), 24, 31, 24) };
		const Rectf m_RedRect{			Rectf((4 * 32), 24, 31, 24) };
		const Rectf m_MagentaRect{		Rectf((5 * 32), 24, 31, 24) };

		const FPoint2f m_TreasureSize{ FPoint2f{ 50,50 } };

		void CreateOrigin();	
		UEntityHandle CreateCheckPoint(const FPoint2f& position);


		URegistry&			m_Registry;
		UEntityCreator		m_EntityCreator;
		UArchetypeCreator	m_ArchetypeCreator;
		UEntityHandle m_Origin;
		UEntityHandle m_Player;
		size_t m_CurrentSceneIndex{ 0 };
	};
}