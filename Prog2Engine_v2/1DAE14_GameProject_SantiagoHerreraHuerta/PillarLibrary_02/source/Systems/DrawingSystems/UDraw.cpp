#define _USE_MATH_DEFINES 
#include <math.h>

#include "../../../include/Systems/DrawSystems/UDraw.h"
#include "../../../include/Systems/CreationSystems/ULoadTextures.h"
#include "../../../include/Systems/MathSystems/SAngles.h"
#include "../../../include/Systems/MathSystems/SVectors.h"
#include "../../../include/Structs/FPlayer.h"

namespace Pillar {

#pragma region Draw

	UDraw::UDraw(URegistry& registry) :
		m_DrawAnimations(registry),
		m_DrawSceneTextures(registry),
		m_DrawTextureMaps(registry),
		m_ApplyParallaxEffect(registry),
		m_SetScreenTransformsToTextureTransforms(registry),
		m_UpdateAnimations(registry),
		m_DrawPlayerHealth(registry),
		m_DrawPlayerWealth(registry),
		m_DrawAnimatedTextureMaps(registry){}


	void UDraw::Update(float deltaSeconds)
	{
		m_ApplyParallaxEffect.Update();
		m_SetScreenTransformsToTextureTransforms.Update();
		m_UpdateAnimations.Update(deltaSeconds);
		m_DrawAnimatedTextureMaps.Update(deltaSeconds);

	}

	void UDraw::Draw() const
	{
		m_DrawSceneTextures.Draw();
		m_DrawAnimatedTextureMaps.Draw();
		m_DrawTextureMaps.Draw();
		m_DrawAnimations.Draw();
		m_DrawPlayerHealth.Draw();
		//m_DrawPlayerWealth.Draw();
	}

#pragma endregion


	UApplyParallaxEffect::UApplyParallaxEffect(URegistry& registry)
	{

		FCamera camera{};
		URealTransform2D realTransform{};
		FZDepth zDepth{};

		UComponentFilter filter{};
		filter.SetIncludingFilter(camera, realTransform);

		registry.GetComponents(m_Camera, filter);
		registry.GetComponents(m_CameraTransform, filter);

		filter.SetIncludingFilter(zDepth, realTransform);

		registry.GetComponents(m_ObjectDepths, filter);
		registry.GetComponents(m_RealTransforms, filter);
	}

	void UApplyParallaxEffect::Update()
	{
		const URealTransform2D& cameraRealTransform = m_CameraTransform.At(0, 0);
		FCamera& camera = m_Camera.At(0, 0);

		double cameraApertureTangent = tan(SAngles::InRadians(camera.ApertureAngle / 2.f));
		const FPoint2f cameraPosition = cameraRealTransform.GetConstScreenTransform().Position;

		float currentZDistanceBasedOnDrawingOrder{};
		float depthDecreasePerEntity{};

		float maxHorizonWidth{};
		float deltaX{};
		float deltaY{};
		float currentAngleOnY{};
		float currentAngleOnX{};

		for (size_t archIdx = 0; archIdx < m_ObjectDepths.NumOfArchetypes(); archIdx++)
		{
			currentZDistanceBasedOnDrawingOrder = camera.MaxAutomaticDepth;
			depthDecreasePerEntity = 
				(camera.MaxAutomaticDepth - camera.MinAutomaticDepth) /
				m_ObjectDepths.NumOfActivatedComponents(archIdx);

			for (size_t compIdx = 0; compIdx < m_ObjectDepths.NumOfActivatedComponents(archIdx); compIdx++)
			{
				FTransform2D& transformToModify = m_RealTransformModifier.GetScreenTransform(&m_RealTransforms.At(archIdx, compIdx));

				if (m_ObjectDepths.At(archIdx, compIdx).Automatic)
				{
					maxHorizonWidth = currentZDistanceBasedOnDrawingOrder * cameraApertureTangent;

					currentZDistanceBasedOnDrawingOrder -= depthDecreasePerEntity;
				}
				else
				{
					maxHorizonWidth = m_ObjectDepths.At(archIdx, compIdx).ZDistance * cameraApertureTangent;
				}
				
				maxHorizonWidth = maxHorizonWidth == 0 ? 0.0001 : maxHorizonWidth;

				deltaX = transformToModify.Position.X - cameraPosition.X;
				deltaY = transformToModify.Position.Y - cameraPosition.Y;
				
				//transformToModify.Position.Y = (deltaY / maxHorizonDistance) + cameraPosition.Y;
				transformToModify.Position.X = (deltaX / maxHorizonWidth) + cameraPosition.X;
				
				if (m_ObjectDepths.At(archIdx, compIdx).Automatic)
				{
					float totalDepthDelta{ camera.MaxAutomaticDepth - camera.MinAutomaticDepth };

					float percentageOfDepth = totalDepthDelta <= 0 ? 0 :
						(currentZDistanceBasedOnDrawingOrder - camera.MinAutomaticDepth) / totalDepthDelta;

					transformToModify.Scale.Y = 
						camera.MaxAutomaticScale - ((camera.MaxAutomaticScale - camera.MinAutomaticScale) * percentageOfDepth);
					transformToModify.Scale.X = 
						camera.MaxAutomaticScale - ((camera.MaxAutomaticScale - camera.MinAutomaticScale) * percentageOfDepth);
				}

			}
		}
	}

	USetScreenTransformsToTextureTransforms::USetScreenTransformsToTextureTransforms(URegistry& registry)
	{
		URealTransform2D realTransform;
		DTextureTransform textureTransform;

		UComponentFilter filter{};
		filter.SetIncludingFilter(realTransform, textureTransform);

		registry.GetComponents(m_RealTransforms, filter);
		registry.GetComponents(m_TextureTransforms, filter);
	}

	void USetScreenTransformsToTextureTransforms::Update()
	{
		const FTransform2D* currentTransform{ nullptr };

		for (size_t archIdx = 0; archIdx < m_RealTransforms.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_RealTransforms.NumOfActivatedComponents(archIdx); compIdx++)
			{
				currentTransform = &m_RealTransforms.At(archIdx, compIdx).GetConstScreenTransform();

				m_TextureTransforms.At(archIdx, compIdx).Position.x = currentTransform->Position.X	;
				m_TextureTransforms.At(archIdx, compIdx).Position.y = currentTransform->Position.Y	;
				m_TextureTransforms.At(archIdx, compIdx).RotationX	= currentTransform->RotationX	;
				m_TextureTransforms.At(archIdx, compIdx).RotationY	= currentTransform->RotationY	;
				m_TextureTransforms.At(archIdx, compIdx).RotationZ	= currentTransform->RotationZ	;
				m_TextureTransforms.At(archIdx, compIdx).Scale.x	= currentTransform->Scale.X		;
				m_TextureTransforms.At(archIdx, compIdx).Scale.y	= currentTransform->Scale.Y		;
			}
		}
	}

	UDrawSceneTextures::UDrawSceneTextures(URegistry& registry) :
		m_SceneTextures(FSceneTexture{ UTexture{ "" } })
	{
		FSceneTexture sceneTexture{ UTexture{""}};
		DTextureTransform textureTransform;

		UComponentFilter filter{};
		filter.SetIncludingFilter(sceneTexture, textureTransform);

		registry.GetComponents(m_SceneTextures, filter);
		registry.GetComponents(m_TextureTransforms, filter);
	}

	void UDrawSceneTextures::Draw() const
	{
		for (size_t archIdx = 0; archIdx < m_SceneTextures.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_SceneTextures.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_TextureTransforms.ConstAt(archIdx, compIdx).ApplyTransformation();
				m_SceneTextures.ConstAt(archIdx, compIdx).Texture.Draw();
				m_TextureTransforms.ConstAt(archIdx, compIdx).ResetTransformation();
			}
		}
	}

	UDrawTextureMaps::UDrawTextureMaps(URegistry& registry) :
		m_TextureMaps(UTextureMap(USpriteSheet(UTexture(""),0,0,0,0)))
	{
		UTextureMap textureMap{ USpriteSheet(UTexture(""),0,0,0,0) };
		DTextureTransform textureTransform;

		UComponentFilter filter{};
		filter.SetIncludingFilter(textureMap, textureTransform);

		registry.GetComponents(m_TextureMaps, filter);
		registry.GetComponents(m_TextureTransforms, filter);

		filter.SetIncludingFilter(FCamera{}, URealTransform2D{});

		registry.GetComponents(m_CameraRealTransform, filter);
		registry.GetComponents(m_Camera, filter);
	}

	void UDrawTextureMaps::Draw() const
	{
		const float delta{ 200.f };

		Rectf cameraRect{};
		cameraRect.height = m_Camera.ConstAt(0, 0).ScreenHeight + delta;
		cameraRect.width = m_Camera.ConstAt(0,0).ScreenWidth + delta;
		cameraRect.bottom = m_CameraRealTransform.ConstAt(0, 0).GetConstRealWorldTransform().Position.Y - (cameraRect.height/2.f);
		cameraRect.left = m_CameraRealTransform.ConstAt(0, 0).GetConstRealWorldTransform().Position.X - (cameraRect.width / 2.f);

		for (size_t archIdx = 0; archIdx < m_TextureMaps.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_TextureMaps.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_TextureTransforms.ConstAt(archIdx, compIdx).ApplyTransformation();
				m_TextureMaps.ConstAt(archIdx, compIdx).Draw(cameraRect);
				m_TextureTransforms.ConstAt(archIdx, compIdx).ResetTransformation();
			}
		}
	}

	UDrawAnimatedTextureMaps::UDrawAnimatedTextureMaps(URegistry& registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(UAnimatedTextureMap{}, DTextureTransform{});

		registry.GetComponents(m_AnimatedTextureMaps, filter);
		registry.GetComponents(m_TextureTransforms, filter);

		filter.SetIncludingFilter(FCamera{}, URealTransform2D{});

		registry.GetComponents(m_CameraRealTransform, filter);
		registry.GetComponents(m_Camera, filter);
	}

	void UDrawAnimatedTextureMaps::Update(float deltaSeconds)
	{
		for (size_t archIdx = 0; archIdx < m_AnimatedTextureMaps.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_AnimatedTextureMaps.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_AnimatedTextureMaps.At(archIdx, compIdx).Update(deltaSeconds);
			}
		}
	}

	void UDrawAnimatedTextureMaps::Draw() const
	{
		const float delta{ 200.f };

		Rectf cameraRect{};
		cameraRect.height = m_Camera.ConstAt(0, 0).ScreenHeight + delta;
		cameraRect.width = m_Camera.ConstAt(0, 0).ScreenWidth + delta;
		cameraRect.bottom = m_CameraRealTransform.ConstAt(0, 0).GetConstRealWorldTransform().Position.Y - (cameraRect.height / 2.f);
		cameraRect.left = m_CameraRealTransform.ConstAt(0, 0).GetConstRealWorldTransform().Position.X - (cameraRect.width / 2.f);

		for (size_t archIdx = 0; archIdx < m_AnimatedTextureMaps.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_AnimatedTextureMaps.NumOfActivatedComponents(archIdx); compIdx++)
			{
				m_TextureTransforms.ConstAt(archIdx, compIdx).ApplyTransformation();
				m_AnimatedTextureMaps.ConstAt(archIdx, compIdx).Draw(cameraRect);
				m_TextureTransforms.ConstAt(archIdx, compIdx).ResetTransformation();
			}
		}
	}

	UDrawPlayerHealth::UDrawPlayerHealth(URegistry& registry) :
		m_PlayerHealth(FHealth{0}),
		m_PlayerUIHealth(FPlayerUIHealth{ UTexture{""}})
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FHealth{ 0 }, FPlayer{});
		registry.GetComponents(m_PlayerHealth);

		filter.SetIncludingFilter(DTextureTransform{}, FPlayerUIHealth{ UTexture{""} });
		registry.GetComponents(m_TextureTransforms);
		registry.GetComponents(m_PlayerUIHealth);
	}

	void UDrawPlayerHealth::Draw() const
	{
		int currentHealth = m_PlayerHealth.ConstAt(0, 0).CurrentHealth;
		float xDelta{};
		Transform currentTransform{};

		for (size_t archIdx = 0; archIdx < m_PlayerUIHealth.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_PlayerUIHealth.NumOfActivatedComponents(archIdx); compIdx++)
			{
				xDelta = m_PlayerUIHealth.ConstAt(archIdx, compIdx).UI_Texture.GetTextureScaledWidth();
				currentTransform = m_TextureTransforms.ConstAt(archIdx, compIdx);

				for (int i = 0; i < currentHealth; i++)//currentHealth can be negative, so always keep an int
				{
					currentTransform.ApplyTransformation();
					m_PlayerUIHealth.ConstAt(archIdx, compIdx).UI_Texture.Draw();
					currentTransform.ResetTransformation();
					currentTransform.Position.x += xDelta;
				}
			}
		}
	}

	UDrawPlayerWealth::UDrawPlayerWealth(URegistry& registry)
	{
		UComponentFilter filter{};
		filter.SetIncludingFilter(FWealth{ }, FPlayer{});

		registry.GetComponents(m_PlayerWealth, filter);

		filter.SetIncludingFilter(DTextureTransform{}, FPlayerUIWealth{});

		registry.GetComponents(m_TextureTransforms, filter);
		registry.GetComponents(m_PlayerUIWealth, filter);
	}

	void UDrawPlayerWealth::Draw() const
	{
		for (size_t archIdx = 0; archIdx < m_PlayerUIWealth.NumOfArchetypes(); archIdx++)
		{
			for (size_t compIdx = 0; compIdx < m_PlayerUIWealth.NumOfActivatedComponents(archIdx); compIdx++)
			{
				const FPlayerUIWealth& playerUI{ m_PlayerUIWealth.ConstAt(archIdx, compIdx) };
				const int wealth{ m_PlayerWealth.ConstAt(0,0).Amount };

				m_TextureTransforms.ConstAt(archIdx, compIdx).ApplyTransformation();
				STextureRegistry::GetFont("Coins: " + std::to_string(wealth), playerUI.FontPath, playerUI.PointSize, playerUI.Color).Draw();
				m_TextureTransforms.ConstAt(archIdx, compIdx).ResetTransformation();
			}
		}
	}

}

