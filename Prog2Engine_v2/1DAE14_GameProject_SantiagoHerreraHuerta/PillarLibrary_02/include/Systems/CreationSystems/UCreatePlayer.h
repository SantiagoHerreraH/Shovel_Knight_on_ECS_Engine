#pragma once

#include <string>
#include "../../../include/Structs/FVector.h"
#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UEntityCreation.h"
#include "../../../include/Core/UEntityCreation.h"

namespace Pillar {

	class UInputContainer;
	class UStateMachine;
	struct FAnimationMontage;

	class UCreatePlayer {

	public:
		UCreatePlayer(URegistry& registry);
		UEntityHandle CreatePlayer(const FPoint2f& startingPoint, float moveSpeed, float jumpSpeed, float gravity, size_t currentSceneIndex);
		void CreateSpawnPoint(const FPoint2f& topLeftPosition, const FPoint2f& size);
		void CreatePlayerLifeDrawer(int sceneIndex, const FPoint2f& topLeftPosition, const FPoint2f& scale);
		void CreatePlayerWealthDrawer(int sceneIndex, const FPoint2f& topLeftPosition, const FPoint2f& scale);
	private:

		UEntityHandle CreateDamageBox(const FPoint2f& position, float height, float width, const std::string& actionThatTriggersDamageBox, const FAnimationMontage* animMontage, const UEntityHandle& player);
		
		URegistry& m_Registry;
		UEntityCreator m_EntityCreator;
		UArchetypeCreator m_ArchetypeCreator;
	};
}