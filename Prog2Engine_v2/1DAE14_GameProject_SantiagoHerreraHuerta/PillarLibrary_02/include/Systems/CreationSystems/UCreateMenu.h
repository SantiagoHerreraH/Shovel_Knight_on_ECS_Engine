#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FVector.h"

namespace Pillar {

	class UCreateMenu {

	public:
		UCreateMenu(URegistry& registry);
		void CreateMainMenuScreen(size_t sceneIndex);
		void CreateMainMenuShovel(size_t sceneIndex, const FPoint2f& position, const FPoint2f& rectSize, const FPoint2f& shovelScale);
		void CreateWinningScreen(size_t sceneIndex);

	private:
		URegistry& m_Registry;
	};
}