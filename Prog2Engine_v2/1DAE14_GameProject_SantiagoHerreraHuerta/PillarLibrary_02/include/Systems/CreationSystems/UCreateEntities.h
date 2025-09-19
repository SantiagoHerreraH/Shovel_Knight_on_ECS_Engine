#pragma once

#include "../../../include/Systems/CreationSystems/UCreatePlayer.h"
#include "../../../include/Systems/CreationSystems/UCreateTreasures.h"
#include "../../../include/Systems/CreationSystems/UCreateMenu.h"
#include "../../../include/Structs/FVector.h"
#include "../../../include/Structs/FCollision.h"

namespace Pillar {

	class UCreateEntities final{

	public:

		UCreateEntities(URegistry& registry);
		void CreateAndSetParams();

	private:

		void CreateMainMenu();
		void CreateLevelOne();

		void CreateLevelChanger(const FPoint2f& bottomLeftPosition, const FPoint2f& size);

		void CreateBackgroundMusic(const std::string& soundTrack);

		void CreateBackground();
		void CreateTree(const FPoint2f& bottomLeftPosition, const FPoint2f& scale);
		void CreateBackgroundFortress();

		void CreateTestPlatform(const FPoint2f& topLeftPosition, const FPoint2f& scale);
		void CreateLevel(const FPoint2f& scale);

		void CreateCamera(const UEntityHandle& player, float colliderHeight, float colliderWidth);

		//Misc
		void CreateDestroyableBlock(const FPoint2f& position, const FPoint2f& size);
		void CreateStairs(const FPoint2f& position, const FPoint2f& size);

		void CreateDamagingBlock(const FPoint2f& position, const FPoint2f& size);

		void CreateMovingPlatform(const FPoint2f& position, const FPoint2f& size, const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition);

		//Treasures
		void CreateOrigin();

		//Enemies
		void CreateBeetle(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle);
		void CreateDrake(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle);

		void CreateBlorb(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle);
		//
		UEntityHandle CreateCheckPoint(const FPoint2f& position);

		//Optimization
		void CreateOptimizationBox(const Rectf& bounds);

		//
		void CreatePlayer(const FPoint2f& position);
		
		//
		UEntityHandle m_Origin;
		UEntityHandle m_Player;
		size_t m_CurrentSceneIndex{0};

		//void CreateEntities();

		URegistry& m_Registry;
		UCreatePlayer m_CreatePlayer;
		UCreateTreasures m_CreateTreasures;
		UCreateMenu m_CreateMenu;

		UEntityCreator m_EntityCreator;
		UArchetypeCreator m_ArchetypeCreator;
	};

}