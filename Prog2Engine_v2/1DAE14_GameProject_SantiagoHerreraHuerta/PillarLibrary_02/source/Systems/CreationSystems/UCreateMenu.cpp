#include "../../../include/Systems/CreationSystems/UCreateMenu.h"

#include "../../../include/InternalIncludes.h"

namespace Pillar {

	UCreateMenu::UCreateMenu(URegistry& registry) :
		m_Registry(registry)
	{
	}
	void UCreateMenu::CreateMainMenuScreen(size_t sceneIndex)
	{
		FEntity entity{};

		DTextureTransform textureTransform{};
		FSceneTexture sceneTexture{ UTexture("Menu/MainMenu.png") };

		sceneTexture.Texture.SetScale(Point2f{
			g_WINDOW.Width / sceneTexture.Texture.GetTextureOriginalWidth(),
			g_WINDOW.Height / sceneTexture.Texture.GetTextureOriginalHeight() });

		FLocalTransform2D localTransform{};
		URealTransform2D realTransform{};
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;

		m_Registry.CreateEntity(entity,
			textureTransform,
			sceneTexture,

			localTransform,
			realTransform,
			sceneObject);
	}
	void UCreateMenu::CreateMainMenuShovel(size_t sceneIndex, const FPoint2f& position, const FPoint2f& size, const FPoint2f& shovelScale)
	{
		UEntityHandle entity{};

		//--- SPRITE TRANSFORM

		DTextureTransform textureTransform{};

		// --- GENERAL TRANSFORMS

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = position;
		localTransform.Transform.Scale = shovelScale;
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;


		// --- COLLISION ---

		FRectCollider rectCollider{ 0, (size.Y / shovelScale.Y) / 1.5f, size.X / shovelScale.X, size.Y / shovelScale.Y };
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{};

		// --- MOUSE EVENT

		FMouseEvents mouseEvents{};
		mouseEvents.NextSceneOnRelease = true;

		// --- CREATION

		m_Registry.CreateEntity(entity,
			rectCollider,
			entityResolveOverlapInfo,
			collisionBodyType,

			mouseEvents,

			textureTransform,

			localTransform,
			realTransform,
			matrix,
			sceneObject);

		// --- SPRITE

		DSprite sprite{ m_Registry, entity };

		USpriteSheet spriteSheet{ STextureRegistry::GetConstTexture(STextureNames::Shovel), 1, 3, 0, 0 };

		spriteSheet.SetPositionInScreen(Point2f{ 0,0 });

		FAnimationMontage idleAnimation{};
		idleAnimation.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 0), 1);
		idleAnimation.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 1), 1);
		idleAnimation.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation;

		FAnimationMontage hoverAnimation{};
		hoverAnimation.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 2), 1);
		hoverAnimation.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation;


		sprite.AddState(SActionNames::Idle, idleAnimation);
		sprite.AddState(SActionNames::Selected, hoverAnimation);

		sprite.LinkWithAll(SActionNames::Idle, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::Selected, ELinkingAction::BothWays);

		// --- STATE MACHINE

		UStateMachine stateMachine{ m_Registry, entity };
		stateMachine.AddStateController(sprite);


		FEventTriggerCondition selected_ExecutionCondition{};
		selected_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		selected_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnHover, EEventState::Active);
		stateMachine.AddConditionsToExecute(SActionNames::Selected, selected_ExecutionCondition);

		FEventTriggerCondition selected_RemovalCondition{};
		selected_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		selected_RemovalCondition.EventName_To_ActiveState.Push(SEventNames::OnHover, EEventState::Inactive);
		stateMachine.AddConditionsToRemove(SActionNames::Selected, selected_RemovalCondition);

		FEventTriggerCondition idle_ExecutionCondition{};
		idle_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnHover, EEventState::Inactive);
		stateMachine.AddConditionsToExecute(SActionNames::Idle, idle_ExecutionCondition);

		FEventTriggerCondition idle_RemovalCondition{};
		idle_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_RemovalCondition.EventName_To_ActiveState.Push(SEventNames::OnHover, EEventState::Active);
		stateMachine.AddConditionsToRemove(SActionNames::Idle, idle_RemovalCondition);

		stateMachine.TriggerState(SActionNames::Idle, EEventStateToTrigger::ForcedExecution);

		m_Registry.AddComponent(entity, stateMachine);
	}
	void UCreateMenu::CreateWinningScreen(size_t sceneIndex)
	{
		FEntity entity{};

		DTextureTransform textureTransform{};
		FSceneTexture sceneTexture{ UTexture("Menu/WinScreen.png") };

		sceneTexture.Texture.SetScale(Point2f{
			g_WINDOW.Width / sceneTexture.Texture.GetTextureOriginalWidth(),
			g_WINDOW.Height / sceneTexture.Texture.GetTextureOriginalHeight() });

		FLocalTransform2D localTransform{};
		URealTransform2D realTransform{};
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;

		m_Registry.CreateEntity(entity,
			textureTransform,
			sceneTexture,

			localTransform,
			realTransform,
			sceneObject);
	}
}