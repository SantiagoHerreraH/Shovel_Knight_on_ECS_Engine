#pragma once

#include "../../../../../Engine/Transform.h"

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/URegistrySingleton.h"
#include "../../../include/Structs/FDrawFrame.h"
#include "../../../include/Structs/FCamera.h"
#include "../../../include/Structs/FLife.h"
#include "../../../include/Systems/TransformSystems/URealTransform2D.h"
#include "../../../include/Systems/DrawSystems/USprite.h"
#include "../../../include/Systems/StateSystems/UpdateStateSystems.h"

namespace Pillar {


	class UApplyParallaxEffect {

	public:
		UApplyParallaxEffect(URegistry& registry);
		void Update(); //after setting screen transforms

	private:
		URealTransform2DModifier m_RealTransformModifier{};

		UComponentIterator<URealTransform2D>	m_RealTransforms;
		UComponentIterator<FParallaxObject>	m_ParallaxObjects;
		UComponentIterator<FZDepth>			m_ObjectDepths;


		UComponentIterator<FCamera>			m_Camera; //for now just one
		UComponentIterator<URealTransform2D>	m_CameraTransform;
	};
	
	class UDrawSceneTextures {
	public:
		UDrawSceneTextures(URegistry& registry);
		void Draw()const;

	private:
		UComponentIterator<FSceneTexture> m_SceneTextures;
		UComponentIterator<DTextureTransform> m_TextureTransforms;
	};

	class UDrawTextureMaps {
	public:
		UDrawTextureMaps(URegistry& registry);
		void Draw()const;

	private:
		UComponentIterator<UTextureMap> m_TextureMaps;//default textureMap here is going to give warning but it doesn't matter
		UComponentIterator<DTextureTransform> m_TextureTransforms;

		UComponentIterator<URealTransform2D> m_CameraRealTransform;
		UComponentIterator<FCamera> m_Camera;
	};

	class UDrawAnimatedTextureMaps {
	public:
		UDrawAnimatedTextureMaps(URegistry& registry);
		void Update(float deltaSeconds);
		void Draw()const;

	private:
		UComponentIterator<UAnimatedTextureMap> m_AnimatedTextureMaps;
		UComponentIterator<DTextureTransform> m_TextureTransforms;

		UComponentIterator<URealTransform2D> m_CameraRealTransform;
		UComponentIterator<FCamera> m_Camera;
	};

	class USetScreenTransformsToTextureTransforms {

	public:
		USetScreenTransformsToTextureTransforms(URegistry& registry);
		void Update();

	private:
		UComponentIterator<URealTransform2D> m_RealTransforms;
		UComponentIterator<DTextureTransform> m_TextureTransforms;
	};

	struct FPlayerUIHealth {
		UTexture UI_Texture;
	};

	struct FPlayerUIWealth {
		std::string FontPath{};
		int PointSize{};
		Color4f Color{};
	};

	class UDrawPlayerHealth {
	public:
		UDrawPlayerHealth(URegistry& registry);
		void Draw()const;
	private:

		UComponentIterator<FHealth> m_PlayerHealth;
		UComponentIterator<DTextureTransform> m_TextureTransforms;
		UComponentIterator<FPlayerUIHealth> m_PlayerUIHealth;
	};

	class UDrawPlayerWealth {
	public:
		UDrawPlayerWealth(URegistry& registry);
		void Draw()const;
	private:

		UComponentIterator<FWealth> m_PlayerWealth;
		UComponentIterator<DTextureTransform> m_TextureTransforms;
		UComponentIterator<FPlayerUIWealth> m_PlayerUIWealth;
	};

	class UDraw {

	public:
		UDraw(URegistry& registry);
		void Update(float deltaSeconds); //it has to be the last in the update function for now
		void Draw()const;

	private:

		/*
			DRAW
		*/
		UDrawAnimations								m_DrawAnimations;
		UDrawSceneTextures							m_DrawSceneTextures;
		UDrawAnimatedTextureMaps					m_DrawAnimatedTextureMaps;
		UDrawTextureMaps							m_DrawTextureMaps;
		UDrawPlayerHealth							m_DrawPlayerHealth;
		UDrawPlayerWealth							m_DrawPlayerWealth;

		/*
			UPDATE
		*/

		UApplyParallaxEffect						m_ApplyParallaxEffect;
		USetScreenTransformsToTextureTransforms		m_SetScreenTransformsToTextureTransforms;
		UUpdateAnimations							m_UpdateAnimations;

	};

}