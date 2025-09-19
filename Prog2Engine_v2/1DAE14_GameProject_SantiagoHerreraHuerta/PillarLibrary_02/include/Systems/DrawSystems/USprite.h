#pragma once

#include "../../../include/Systems/StateSystems/UStateMachine.h"
#include "../../../include/Systems/StateSystems/UStateControllerGraph.h"
#include "../../../include/Systems/DrawSystems/UAnimation.h"

namespace Pillar {

	using DSprite = UStateControllerGraph<FAnimationMontage, false, false>;

	struct FRegistry_Sprites {
		UEntityHandleMap<DSprite> EntityHandle_To_Sprites;
	};

	class UUpdateAnimations final {

	public:

		UUpdateAnimations(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		UComponentIterator<UStateMachine> m_StateMachines;
		UComponentIterator<FGatedState<FAnimationMontage>> m_AnimationMontages;
	};

	class UDrawAnimations final {

	public:
		UDrawAnimations(URegistry& registry);
		void Draw()const;

	private:

		UComponentIterator<FGatedState<FAnimationMontage>> m_AnimationMontages;
		UComponentIterator<DTextureTransform> m_TextureTransforms;
	};
}