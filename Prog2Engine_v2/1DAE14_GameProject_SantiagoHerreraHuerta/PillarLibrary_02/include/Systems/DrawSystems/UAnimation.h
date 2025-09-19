#pragma once

#include "../../../include/Systems/DrawSystems/UTexture.h"
#include "../../../include/Core/URegistry.h"
#include "../../../include/Structs/FState.h"

namespace Pillar {

	/*
		Each animation for now has:
		- a character
			- 
		- a state
		- 
		- The animation manager is just the interface 
		to change the animation state -> the actual change in state will be managed by the event system
		- There will be an event listener that will change the current animation corresponding its state
	*/

	struct FAnimationFrameInfo {
		UTextureRef Texture;
		unsigned int NumberOfFrames;
		float TimeToGoToTheNextFrame;
	};

	class UAnimation final{

	public:

		void Draw()const;
		void Update(float deltaSeconds);

		void CanLoop(bool canLoop); //default is true
		void SetFramesPerSecond(unsigned int numberOfFrames);
		void AddFrame(const UTextureRef& frame, unsigned int numberOfFrames);

		void AddFrames(UTexture& texture, const Rectf& startFrameBound, int numberOfFrames, int xSpaceBetweenFrames);
		//void AddFrames(USpriteSheet& spriteSheet, unsigned int startRowIndex, unsigned int startColumnIndex, unsigned int endRowIndex, unsigned int endColumnIndex );

		void SetFrameNumberInFrame(size_t frameIndex, int numberOfFrames);
		void ClearFrames();

		int LoopNumber();

		void Reset();
		void Play();
		void Pause();

		const UTextureRef& GetConstTextureRef(int frameNumber)const;

	private:
		int m_NumberOfLoops{ 0 };
		bool m_CanLoop{ true };
		bool m_IsPlaying{ true };
		double m_CurrentTime{0};
		size_t m_CurrentFrameIndex{ 0 };
		float m_SecondsPerFrame{ 1 };

		std::vector<FAnimationFrameInfo> m_Frames;
	};

	enum class EAnimationDeactivationProcedure {
		LoopNumberIsNotRelevantForDeactivation,
		ForceDeactivationAfterLoopNumber, //this one forces deactivation
		PermitDeactivationAfterLoopNumber //this one only deactivates if triggered externally
	};

	struct FAnimationMontage {
		UAnimation Animation;
		EAnimationDeactivationProcedure AnimationDeactivationProcedure{ EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation};
		int LoopNumber{ 1 };
		bool ResetUponFinishing{ true };
		EStateRemovalType StateRemovalType{ EStateRemovalType::Local };
		bool Activated{true};
	};
}