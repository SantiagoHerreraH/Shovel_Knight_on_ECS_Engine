#include "../../../include/Systems/DrawSystems/USprite.h"

namespace Pillar {

#pragma region Update Animations

	UUpdateAnimations::UUpdateAnimations(URegistry& registry) :
		m_StateMachines(UStateMachine{ registry, UEntityHandle{} })
	{
		UStateMachine stateSystem{ registry, UEntityHandle{} };
		FGatedState<FAnimationMontage> animMontage{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(stateSystem, animMontage);

		registry.GetComponents(m_AnimationMontages, filter);
		registry.GetComponents(m_StateMachines, filter);
	}
	void UUpdateAnimations::Update(float deltaSeconds)
	{
		FAnimationMontage* currentAnimationMontage{ nullptr };

		for (size_t archIdx = 0; archIdx < m_AnimationMontages.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_AnimationMontages.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentAnimationMontage = &m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).State;

				if (!currentAnimationMontage->Activated)
				{
					continue;
				}

				m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = 
					currentAnimationMontage->AnimationDeactivationProcedure == 
					EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation;

				currentAnimationMontage->Animation.Update(deltaSeconds);

				switch (currentAnimationMontage->AnimationDeactivationProcedure)
				{
				case EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation:
					m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = true;

					break;
				case EAnimationDeactivationProcedure::PermitDeactivationAfterLoopNumber:
					m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = false;

					if (currentAnimationMontage->Animation.LoopNumber() >= currentAnimationMontage->LoopNumber)
					{
						m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = true;

						if (m_StateMachines.At(archIdx, compIdx).
							EventState(m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(0)) == EEventState::Inactive)
						{
							switch (m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).State.StateRemovalType)
							{
							case EStateRemovalType::Local:
								m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval<DSprite>
									(m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(0));
								break;
							case EStateRemovalType::AllStateMachines:
								m_StateMachines.At(archIdx, compIdx).TriggerState
								(m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(0), EEventStateToTrigger::ConditionalRemoval);
								break;
							}
						}
					}
					
					break;
				case EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber:

					m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = false;

					if (currentAnimationMontage->Animation.LoopNumber() >= currentAnimationMontage->LoopNumber)
					{
						m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).CanBeRemoved = true;

						switch (m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(0).State.StateRemovalType)
						{
						case EStateRemovalType::Local:
							m_StateMachines.At(archIdx, compIdx).TriggerTypeSpecificEventRemoval<DSprite>
								(m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(0));
							break;
						case EStateRemovalType::AllStateMachines:
							m_StateMachines.At(archIdx, compIdx).TriggerState
							(m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.ConstKeyAt(0), EEventStateToTrigger::ForcedRemoval);
							break;
						}
						
					}
					break;
				}
				
				for (size_t animIdx = 1; animIdx < m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.Size(); animIdx++)
				{
					if (m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(animIdx).State.ResetUponFinishing)
					{
						m_AnimationMontages.At(archIdx, compIdx).EventName_To_State.At(animIdx).State.Animation.Reset();
					}
				}
			}
		}
	}

#pragma endregion

	UDrawAnimations::UDrawAnimations(URegistry& registry)
	{
		FGatedState<FAnimationMontage> animationMontage{};
		DTextureTransform textureTransform{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(animationMontage, textureTransform);

		registry.GetComponents(m_AnimationMontages, filter);
		registry.GetComponents(m_TextureTransforms, filter);

	}

	void UDrawAnimations::Draw() const
	{
		for (size_t archIdx = 0; archIdx < m_AnimationMontages.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_AnimationMontages.NumOfActivatedComponents(archIdx); compIdx++)
			{
				if (m_AnimationMontages.ConstAt(archIdx, compIdx).EventName_To_State.ConstAt(0).State.Activated)
				{
					m_TextureTransforms.ConstAt(archIdx, compIdx).ApplyTransformation();
					m_AnimationMontages.ConstAt(archIdx, compIdx).EventName_To_State.ConstAt(0).State.Animation.Draw();
					m_TextureTransforms.ConstAt(archIdx, compIdx).ResetTransformation();
				}
			}
		}
	}
}