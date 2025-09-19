
#pragma once

#include <unordered_map>
#include <memory>
#include <string>

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UComponentIterator.h"

#include "../../../include/Systems/StateSystems/UStateMachine.h"

#include "../../../../../Engine/SoundStream.h"
#include "../../../../../Engine/SoundEffect.h"

namespace Pillar {

	struct FBackgroundMusic {
		std::string Name;
		bool Playing{ false };
	};

	struct FSoundEffectState {
		std::string Name;
		int LoopNumber{1};
		bool ResetAfterPlay{true};
		EStateRemovalType StateRemovalType{};
	};

	class SSound {

	public:

		static SoundStream& GetBackgroundMusic(const std::string& fileName);
		static SoundEffect& GetSoundEffect(const std::string& fileName);

		static const SoundStream& GetConstBackgroundMusic(const std::string& fileName);
		static const SoundEffect& GetConstEffectMusic(const std::string& fileName);

	private:

		static std::unordered_map<std::string, std::unique_ptr<SoundStream>> s_Name_To_SoundStream;
		static std::unordered_map<std::string, std::unique_ptr<SoundEffect>> s_Name_To_SoundEffect;
	};

	class USound {
	public:
		USound(URegistry& registry);
		void Update();

	private:
		UComponentIterator<FBackgroundMusic> m_BackgroundTracks;
	};
}