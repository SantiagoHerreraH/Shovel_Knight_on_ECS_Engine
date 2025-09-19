#include "../../../include/Systems/CreationSystems/UCreatePlayer.h"
#include "../../../include/Systems/MathSystems/SGeometry.h"
#include "../../../include/Systems/CreationSystems/ULoadTextures.h"
#include "../../../include/InternalIncludes.h"

namespace Pillar {

	UCreatePlayer::UCreatePlayer(URegistry& registry) :
		m_Registry(registry),
		m_EntityCreator(registry),
		m_ArchetypeCreator(registry){}

	UEntityHandle UCreatePlayer::CreatePlayer( const FPoint2f& startingPoint,  float moveSpeed, float jumpSpeed, float gravity, size_t currentSceneIndex)
	{
		/*

			 --------------- SHOVEL KNIGHT -------------------

		*/

		// COMPONENTS


		UEntityHandle entityHandle{};

		FPlayer player{};

		//---- SET TAG
		FTag tag{};
		tag.Name = "Player";

		//---- SET TRANSFORM ----

		DTextureTransform textureTransform{};

		FLocalTransform localTransform{};
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};
		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = startingPoint; 
		FSceneNode sceneNode{};

		//---- SET COLLIDER ----

		FPoint2f shovelKnightCollisionBoxSize{ 20.f,25.f };
		Point2f shovelKnightScale{ 3.f,3.f };

		FRectCollider collider{ 1,1,1,1, false };
		collider.OriginalRect.Top = (shovelKnightCollisionBoxSize.Y * shovelKnightScale.y) / 2.f;
		collider.OriginalRect.Left = -(shovelKnightCollisionBoxSize.X * shovelKnightScale.x) / 4.f;
		collider.OriginalRect.Height = shovelKnightCollisionBoxSize.Y * shovelKnightScale.y;
		collider.OriginalRect.Width = shovelKnightCollisionBoxSize.X * (shovelKnightScale.x / 2.f);


		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionType{ ECollisionBodyType::Solid };


		//---- SET PHYSICS----


		FVelocity velocity{};
		FNetForce2D netForce{};
		FMass mass{};

		FEnvironmentForces forces{};
		FMovement stylizedGravity{};
		stylizedGravity.Movement = FPoint2f(0, -gravity);
		stylizedGravity.MovementType = EMovementType::AddForce;
		forces.ForcesToApplyPerTick.Add(stylizedGravity); //Gravity



		//---- SET STATS

		const float woundDuration{ 1.f };
		const float deathDuration{ 1.5f };
		FHealth health{ 3, woundDuration, deathDuration };

		FWealth wealth{0};

		//--- SET SCENE OBJECT
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = currentSceneIndex;

		USpawnPointManager spawnPointManager{};

		m_Registry.CreateEntity(entityHandle,
			spawnPointManager,

			sceneObject,
			
			wealth,
			health,

			forces,
			velocity,
			netForce,
			mass,

			collisionType,
			entityResolveOverlapInfo,
			collider,
			
			sceneNode,
			worldTransform,
			matrix,
			localTransform,
			realTransform,
			textureTransform,
			
			tag,
			player);

		// ----- SET ANIMATION GRAPH -----

		Point2f shovelKnightDefaultTextureLocation{ -(shovelKnightCollisionBoxSize.X - 5.f) * shovelKnightScale.x, -(shovelKnightCollisionBoxSize.Y / 2.f) * shovelKnightScale.y };

		UTexture& shovelKnightSpriteSheet{ STextureRegistry::GetTexture(STextureNames::ShovelKnight)};//559 x 3513

		//---- DECLARE INPUT ----
		UInputContainer playerInput{};
		playerInput.Create(SActionNames::MoveRight,		DKeys{ SDL_SCANCODE_D,		SDL_SCANCODE_RIGHT });
		playerInput.Create(SActionNames::MoveLeft,		DKeys{ SDL_SCANCODE_A,		SDL_SCANCODE_LEFT });
		playerInput.Create(SActionNames::Jump,			DKeys{ SDL_SCANCODE_SPACE});
		playerInput.Create(SActionNames::MoveUp,		DKeys{ SDL_SCANCODE_UP,		SDL_SCANCODE_W });
		playerInput.Create(SActionNames::MoveDown,		DKeys{ SDL_SCANCODE_DOWN,	SDL_SCANCODE_S });
		playerInput.Create(SActionNames::AttackForward, DKeys{ SDL_SCANCODE_J });
		playerInput.Create(SActionNames::AttackDown,	DKeys{ SDL_SCANCODE_S,		SDL_SCANCODE_DOWN }, EInputState::Down, EInputState::None);


		//---- CREATE ANIMATION MONTAGE SETUP----

		FAnimationMontage shovelKnight_Idle{};
		shovelKnight_Idle.Animation.SetFramesPerSecond(1);
		shovelKnight_Idle.Animation.AddFrame(
			UTextureRef(shovelKnightSpriteSheet, Rectf(2, 101, 33, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Idle.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(38, 101, 33, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Walk{};
		shovelKnight_Walk.Animation.SetFramesPerSecond(6);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(44, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(86, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(128, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(170, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Walk.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(212, 138, 39, 34), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Jump{};
		shovelKnight_Jump.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 208, 30, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Fall{};
		shovelKnight_Fall.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 244, 31, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Turn{};
		shovelKnight_Turn.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		shovelKnight_Turn.LoopNumber = 1;
		shovelKnight_Turn.Animation.SetFramesPerSecond(2);
		shovelKnight_Turn.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 172, 25, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Shovel{};
		shovelKnight_Shovel.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::PermitDeactivationAfterLoopNumber;
		shovelKnight_Shovel.LoopNumber = 1;
		shovelKnight_Shovel.Animation.SetFramesPerSecond(12);
		shovelKnight_Shovel.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 609, 53, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Shovel.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(58, 609, 53, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Shovel.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(114, 609, 53, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Shovel.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(170, 609, 53, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 2);
		
		FAnimationMontage shovelKnight_Shovel_Down{};
		shovelKnight_Shovel_Down.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 572, 23, 35), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_ClimbIdle{};
		shovelKnight_ClimbIdle.Animation.SetFramesPerSecond(1);
		shovelKnight_ClimbIdle.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 506, 24, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Climb{};
		shovelKnight_Climb.Animation.SetFramesPerSecond(6);
		shovelKnight_Climb.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 506, 24, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);
		shovelKnight_Climb.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(29, 506, 24, 31), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);

		FAnimationMontage shovelKnight_Wounded{};
		shovelKnight_Wounded.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		shovelKnight_Wounded.LoopNumber = 1;
		shovelKnight_Wounded.Animation.SetFramesPerSecond(int(1 / woundDuration));
		shovelKnight_Wounded.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 864, 34, 38), shovelKnightDefaultTextureLocation, shovelKnightScale), 1);


		FAnimationMontage shovelKnight_Death{};
		shovelKnight_Death.StateRemovalType = EStateRemovalType::AllStateMachines;
		shovelKnight_Death.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		shovelKnight_Death.LoopNumber = 1;
		shovelKnight_Death.Animation.SetFramesPerSecond(12);
		shovelKnight_Death.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(2, 900, 36, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 6);
		shovelKnight_Death.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(41, 900, 36, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 3);
		shovelKnight_Death.Animation.AddFrame(UTextureRef(shovelKnightSpriteSheet, Rectf(80, 900, 36, 33), shovelKnightDefaultTextureLocation, shovelKnightScale), 6);


		//---- SPRITE SETUP----

		DSprite shovelKnightSprite{ m_Registry, entityHandle };
		shovelKnightSprite.AddState(SActionNames::Idle,				shovelKnight_Idle);
		shovelKnightSprite.AddState(SActionNames::MoveLeft,			shovelKnight_Walk);
		shovelKnightSprite.AddState(SActionNames::MoveRight,		shovelKnight_Walk);
		shovelKnightSprite.AddState(SActionNames::Jump,				shovelKnight_Jump);
		shovelKnightSprite.AddState(SActionNames::AttackForward,	shovelKnight_Shovel);
		shovelKnightSprite.AddState(SActionNames::AttackDown,		shovelKnight_Shovel_Down);
		shovelKnightSprite.AddState(SActionNames::MoveUp,			shovelKnight_Climb);
		shovelKnightSprite.AddState(SActionNames::MoveDown,			shovelKnight_Climb);
		shovelKnightSprite.AddState(SActionNames::ClimbIdle,		shovelKnight_ClimbIdle);
		shovelKnightSprite.AddState(SActionNames::Fall,				shovelKnight_Fall);
		shovelKnightSprite.AddState(SActionNames::Wounded,			shovelKnight_Wounded);
		shovelKnightSprite.AddState(SActionNames::Die,				shovelKnight_Death);

		// ---- MOVE UP AND MOVE DOWN

		shovelKnightSprite.LinkWithAll(SActionNames::MoveUp, ELinkingAction::BothWays);
		shovelKnightSprite.LinkWithAll(SActionNames::MoveDown, ELinkingAction::BothWays);
		shovelKnightSprite.LinkWithAll(SActionNames::ClimbIdle, ELinkingAction::BothWays);

		//---- DEATH ANIMATION SETUP----

		shovelKnightSprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);

		FEventTriggerCondition death_Condition_1{};
		//death_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		death_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::Die, death_Condition_1);

		//---- WOUNDED ANIMATION SETUP----

		shovelKnightSprite.LinkWithAll(SActionNames::Wounded, ELinkingAction::BothWays);

		//---- IDLE ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::Idle, SActionNames::MoveLeft,		ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Idle, SActionNames::MoveRight,		ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Idle, SActionNames::Fall,			ELinkingAction::OneWay);
		shovelKnightSprite.LinkStates(SActionNames::Idle, SActionNames::AttackForward,	ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Idle, SActionNames::Jump,			ELinkingAction::BothWays);

		FEventTriggerCondition idle_Condition_1{};
		idle_Condition_1.EventTriggerConditionType = EEventTriggerConditionType::GateAndTrigger;
		idle_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision,	EEventState::Active);
		idle_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnMoveLeft,			EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnMoveRight,		EEventState::Inactive);
		//idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::MoveLeft,			EEventState::Inactive);
		//idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::MoveRight,			EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward,		EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown,		EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::MoveUp,			EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::MoveDown,			EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::ClimbIdle,			EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::Fall,				EEventState::Inactive);
		idle_Condition_1.EventName_To_ActiveState.Push(SActionNames::Wounded,			EEventState::Inactive);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::Idle, idle_Condition_1);


		//---- MOVE LEFT ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::MoveLeft, SActionNames::MoveRight,		ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::MoveLeft, SActionNames::TurnRight,		ELinkingAction::OneWay); // maybe can incorporate turn here
		shovelKnightSprite.LinkStates(SActionNames::MoveLeft, SActionNames::Jump,			ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::MoveLeft, SActionNames::AttackForward,	ELinkingAction::BothWays);

		FEventTriggerCondition moveLeft_Condition_1{};
		moveLeft_Condition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		moveLeft_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision,	EEventState::Active);
		moveLeft_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward,		EEventState::Inactive);
		moveLeft_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown,		EEventState::Inactive);
		moveLeft_Condition_1.EventName_To_ActiveState.Push(SActionNames::ClimbIdle,			EEventState::Inactive);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::MoveLeft, moveLeft_Condition_1);

		FEventTriggerCondition moveLeft_Condition_2{};
		moveLeft_Condition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		moveLeft_Condition_2.EventName_To_ActiveState.Push(SEventNames::OnMoveLeft, EEventState::Active);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::MoveLeft, moveLeft_Condition_2);

		//---- MOVE RIGHT ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::MoveRight, SActionNames::Jump, ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::MoveRight, SActionNames::TurnLeft, ELinkingAction::OneWay);
		shovelKnightSprite.LinkStates(SActionNames::MoveRight, SActionNames::AttackForward, ELinkingAction::BothWays);

		FEventTriggerCondition moveRight_Condition_1{};
		moveRight_Condition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		moveRight_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		moveRight_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward,	EEventState::Inactive);
		moveRight_Condition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown,		EEventState::Inactive);
		moveRight_Condition_1.EventName_To_ActiveState.Push(SActionNames::ClimbIdle,		EEventState::Inactive);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::MoveRight, moveRight_Condition_1);

		FEventTriggerCondition moveRight_Condition_2{};
		moveRight_Condition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		moveRight_Condition_2.EventName_To_ActiveState.Push(SEventNames::OnMoveRight, EEventState::Active);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::MoveRight, moveRight_Condition_2);

		//---- JUMP ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::Jump, SActionNames::Fall,			ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Jump, SActionNames::AttackForward,	ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Jump, SActionNames::AttackDown,		ELinkingAction::OneWay);

		FEventTriggerCondition jumpAnimation_ExecutionCondition_1{};
		jumpAnimation_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision,	EEventState::Inactive);
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward,	EEventState::Inactive);
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown,		EEventState::Inactive);
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveUp,			EEventState::Inactive);
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveDown,			EEventState::Inactive);
		jumpAnimation_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Wounded,			EEventState::Inactive);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::Jump, jumpAnimation_ExecutionCondition_1);

		FEventTriggerCondition jump_ExecutionCondition_2{};
		jump_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		jump_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnJump, EEventState::Active);
		shovelKnightSprite.AddConditionsToExecute(SActionNames::Jump, jump_ExecutionCondition_2);

		FEventTriggerCondition jump_RemovalCondition_1{};
		jump_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		jump_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		shovelKnightSprite.AddConditionsToRemove(SActionNames::Jump, jump_RemovalCondition_1);

		//---- ATTACK DOWN ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::AttackDown, SActionNames::AttackForward,	ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::AttackDown, SActionNames::Idle,				ELinkingAction::OneWay);
		shovelKnightSprite.LinkStates(SActionNames::AttackDown, SActionNames::MoveLeft,			ELinkingAction::OneWay);
		shovelKnightSprite.LinkStates(SActionNames::AttackDown, SActionNames::MoveRight,		ELinkingAction::OneWay);

		//---- FALL ANIMATION SETUP----

		shovelKnightSprite.LinkStates(SActionNames::Fall, SActionNames::Idle,			ELinkingAction::OneWay);
		shovelKnightSprite.LinkStates(SActionNames::Fall, SActionNames::MoveRight,		ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Fall, SActionNames::MoveLeft,		ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Fall, SActionNames::AttackForward,	ELinkingAction::BothWays);
		shovelKnightSprite.LinkStates(SActionNames::Fall, SActionNames::AttackDown,		ELinkingAction::OneWay);

		FEventTriggerCondition fall_ExecutionCondition_1{};
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision,		EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnFall,				EEventState::Active);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward,		EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown,			EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveUp,				EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveDown,				EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::ClimbIdle,			EEventState::Inactive);
		fall_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Wounded,				EEventState::Inactive);

		shovelKnightSprite.AddConditionsToExecute(SActionNames::Fall, fall_ExecutionCondition_1);

		FEventTriggerCondition fall_RemovalCondition_1{};
		fall_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		fall_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		shovelKnightSprite.AddConditionsToRemove(SActionNames::Fall, fall_RemovalCondition_1);


		//---- FINISH ANIMATION SETUP----

		shovelKnightSprite.TriggerState(SActionNames::Idle, EEventStateToTrigger::ConditionalExecution);

		/*

		---- SET VECTOR MOVEMENT OVER TIME STATE MACHINE ----

		*/

		DStateController_VectorMovementOverTime player_VectorMovementOverTime_StateController{ m_Registry, entityHandle };
		player_VectorMovementOverTime_StateController.AddState(SActionNames::Rebound,	SMovementCreation::GetJumpMovement());
		player_VectorMovementOverTime_StateController.AddState(SActionNames::Jump,		SMovementCreation::GetJumpMovement());
		player_VectorMovementOverTime_StateController.AddState(SActionNames::Wounded,	SMovementCreation::GetHitMovement(entityHandle));

		//---- JUMP CONDITIONS ----

		FEventTriggerCondition jumpMovement_ExecutionCondition_1{};
		jumpMovement_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		jumpMovement_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		player_VectorMovementOverTime_StateController.AddConditionsToExecute(SActionNames::Jump, jumpMovement_ExecutionCondition_1);
		
		/*

		---- SET SOUND EFFECTS ----

		*/

		FSoundEffectState attack_SoundEffect{}; 
		attack_SoundEffect.Name = "Sound/SwooshSound.mp3";
		attack_SoundEffect.ResetAfterPlay = true;
		attack_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		attack_SoundEffect.LoopNumber = 0;

		FSoundEffectState jump_SoundEffect{}; 
		jump_SoundEffect.Name = "Sound/JumpSound.mp3";
		jump_SoundEffect.ResetAfterPlay = true;
		jump_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		jump_SoundEffect.LoopNumber = 0;

		FSoundEffectState hurt_SoundEffect{};
		hurt_SoundEffect.Name = "Sound/HurtSound.mp3";
		hurt_SoundEffect.ResetAfterPlay = true;
		hurt_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		hurt_SoundEffect.LoopNumber = 0;

		FSoundEffectState die_SoundEffect{};
		die_SoundEffect.Name = "Sound/DieSound.mp3";
		die_SoundEffect.ResetAfterPlay = true;
		die_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		die_SoundEffect.LoopNumber = 0;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, entityHandle };
		stateController_SoundEffects.AddState(SActionNames::AttackForward, attack_SoundEffect);
		stateController_SoundEffects.AddState(SActionNames::Jump, jump_SoundEffect);
		stateController_SoundEffects.AddState(SActionNames::Wounded, hurt_SoundEffect);
		stateController_SoundEffects.AddState(SActionNames::Die, die_SoundEffect);

		/*

		---- SET VECTOR MOVEMENT STATE MACHINE ----

		*/

		FVectorMovement playerIdleMove{};
		playerIdleMove.Movement = { 0, 0 };
		playerIdleMove.MovementType = EMovementType::OverrideVelocity;
		playerIdleMove.ApplyMovementOnXAxis = true;
		playerIdleMove.ApplyMovementOnYAxis = false;
		playerIdleMove.TerminateEachTick = false;

		FVectorMovement playerStillMove{};
		playerStillMove.Movement = { 0, 0 };
		playerStillMove.MovementType = EMovementType::OverrideVelocity;
		playerStillMove.ApplyMovementOnXAxis = true;
		playerStillMove.ApplyMovementOnYAxis = false;
		playerStillMove.TerminateEachTick = false;

		FVectorMovement playerLeftMove{};
		playerLeftMove.Movement = { -moveSpeed, 0 };
		playerLeftMove.MovementType = EMovementType::OverrideVelocity;
		playerLeftMove.ApplyMovementOnXAxis = true;
		playerLeftMove.ApplyMovementOnYAxis = false;

		FVectorMovement playerRightMove{};
		playerRightMove.Movement = { moveSpeed, 0 };
		playerRightMove.MovementType = EMovementType::OverrideVelocity;
		playerRightMove.ApplyMovementOnXAxis = true;
		playerRightMove.ApplyMovementOnYAxis = false;

		FVectorMovement playerUpMove{};
		playerUpMove.Movement = { 0, moveSpeed };
		playerUpMove.MovementType = EMovementType::OverrideVelocity;

		FVectorMovement playerDownMove{};
		playerDownMove.Movement = { 0, -moveSpeed };
		playerDownMove.MovementType = EMovementType::OverrideVelocity;

		FVectorMovement playerClimbIdle{};
		playerClimbIdle.Movement = { 0, 0 };
		playerClimbIdle.MovementType = EMovementType::OverrideVelocity;
		playerClimbIdle.ApplyMovementOnXAxis = false;
		playerClimbIdle.ApplyMovementOnYAxis = true;

		DStateController_VectorMovement player_VectorMovement_StateController{ m_Registry, entityHandle };
		player_VectorMovement_StateController.AddState(SActionNames::MoveLeft,		playerLeftMove);
		player_VectorMovement_StateController.AddState(SActionNames::MoveRight,		playerRightMove);
		player_VectorMovement_StateController.AddState(SActionNames::MoveUp,		playerUpMove);
		player_VectorMovement_StateController.AddState(SActionNames::MoveDown,		playerDownMove);
		player_VectorMovement_StateController.AddState(SActionNames::Idle,			playerIdleMove);
		player_VectorMovement_StateController.AddState(SActionNames::AttackForward, playerStillMove);
		player_VectorMovement_StateController.AddState(SActionNames::Die,			playerStillMove);
		player_VectorMovement_StateController.AddState(SActionNames::ClimbIdle,		playerClimbIdle);

		//----IDLE TRIGGERS----

		FEventTriggerCondition idle_ExecutionCondition_1{};
		idle_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::GateAndTrigger;
		idle_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveLeft, EEventState::Inactive);// when these get removed, these are not checking for idle
		idle_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveRight, EEventState::Inactive);
		idle_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		player_VectorMovement_StateController.AddConditionsToExecute(SActionNames::Idle, idle_ExecutionCondition_1);

		FEventTriggerCondition idle_RemovalCondition_1{};
		idle_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		player_VectorMovement_StateController.AddConditionsToRemove(SActionNames::Idle, idle_RemovalCondition_1);

		/*

		----SET HEALTH IMMUNITY STATE CONTROLLER----

		*/

		FHealthImmunityState immuneState{};
		immuneState.CurrentlyImmune = true;
		immuneState.StateRemovalType = EStateRemovalType::Local;

		FHealthImmunityState notImmuneState{};
		notImmuneState.CurrentlyImmune = false;
		notImmuneState.StateRemovalType = EStateRemovalType::Local;

		DStateController_HealthImmunity player_StateController_HealthImmunity{ m_Registry, entityHandle };
		player_StateController_HealthImmunity.AddState(SActionNames::Immune, immuneState);
		player_StateController_HealthImmunity.AddState(SActionNames::NotImmune, notImmuneState);

		/*

		----SET SPAWN POINT MANAGER CONTROLLER----

		*/

		FSpawnPointState spawnState{};
		spawnState.SpawnPointsAction = ESpawnPointsAction::SpawnInLast;
		spawnState.StateRemovalType = EStateRemovalType::AllStateMachines;

		FSpawnPointState newSpawnPointState{};
		newSpawnPointState.SpawnPointsAction = ESpawnPointsAction::AddSpawnPoint;
		newSpawnPointState.StateRemovalType = EStateRemovalType::AllStateMachines;

		DStateController_SpawnManager spawnManager_StateController{ m_Registry, entityHandle };
		spawnManager_StateController.AddState(SActionNames::Spawn,		spawnState);
		spawnManager_StateController.AddState(SActionNames::NewSpawn,	newSpawnPointState);

		/*

		----SET ROTATION TRANSFORMATION STATE CONTROLLER----

		*/

		FRotationTransformation playerTurnLeft{};
		playerTurnLeft.TerminateEachTick = true;
		playerTurnLeft.Rotation.Y = 180;
		playerTurnLeft.RotationType = EOperationType::Override;

		FRotationTransformation playerTurnRight{};
		playerTurnRight.TerminateEachTick = true;
		playerTurnRight.Rotation.Y = 0;
		playerTurnRight.RotationType = EOperationType::Override;

		DStateController_RotationTransformation player_RotationTransformation_StateController{ m_Registry, entityHandle };
		player_RotationTransformation_StateController.AddState(SActionNames::MoveLeft, playerTurnLeft);
		player_RotationTransformation_StateController.AddState(SActionNames::MoveRight, playerTurnRight);

		/*

			STATE MACHINE

		*/


		UStateMachine playerStateMachine{ m_Registry, entityHandle };
		playerStateMachine.AddStateController<DSprite>(shovelKnightSprite);
		playerStateMachine.AddStateController<DStateController_VectorMovement>(player_VectorMovement_StateController);
		playerStateMachine.AddStateController<DStateController_VectorMovementOverTime>(player_VectorMovementOverTime_StateController);
		playerStateMachine.AddStateController<DStateController_RotationTransformation>(player_RotationTransformation_StateController);
		playerStateMachine.AddStateController<DStateController_HealthImmunity>(player_StateController_HealthImmunity);
		playerStateMachine.AddStateController<DStateController_SpawnManager>(spawnManager_StateController);
		playerStateMachine.AddStateController<DStateController_SoundEffects>(stateController_SoundEffects);
		m_Registry.AddComponent(entityHandle, playerStateMachine);

		//---- INPUT ----

		for (auto It = playerInput.GetData().begin(); It != playerInput.GetData().end(); It++)
		{
			for (size_t keyIdx = 0; keyIdx < It->second.Keys.size(); keyIdx++)
			{
				playerStateMachine.BindEventToInput(It->first, EEventStateToTrigger::ConditionalExecution,	It->second.Keys.at(keyIdx), It->second.InputStateForExecution);
				playerStateMachine.BindEventToInput(It->first, EEventStateToTrigger::ConditionalRemoval,	It->second.Keys.at(keyIdx), It->second.InputStateForRemoval);
			}
		}

		// ---- CREATE DAMAGE BOXES ----

		USpriteSheet attackDownSpriteSheet{ STextureRegistry::GetTexture(STextureNames::AttackDown), 1, 4, 0, 0 };
		USpriteSheet attackForwardSpriteSheet{ STextureRegistry::GetTexture(STextureNames::AttackForward),1, 4, 0, 0  };
		
		attackDownSpriteSheet.SetPositionInScreen(Point2f{ attackDownSpriteSheet.GetOriginalCellWidth() * -1.8f,attackDownSpriteSheet.GetOriginalCellHeight() * -2.5f });
		attackDownSpriteSheet.SetScaleInScreen(Point2f{ 3,3 });

		attackForwardSpriteSheet.SetPositionInScreen(Point2f{0,0});
		attackForwardSpriteSheet.SetScaleInScreen(Point2f{ 3,3 });


		FAnimationMontage attackDownAnimationMontage{};
		FAnimationMontage attackForwardAnimationMontage{};

		attackDownAnimationMontage.Animation.SetFramesPerSecond(12);
		attackDownAnimationMontage.Animation.CanLoop(false);
		attackDownAnimationMontage.Animation.AddFrame(attackDownSpriteSheet.GetFrameCopy(0, 0), 1);
		attackDownAnimationMontage.Animation.AddFrame(attackDownSpriteSheet.GetFrameCopy(0, 1), 1);
		attackDownAnimationMontage.Animation.AddFrame(attackDownSpriteSheet.GetFrameCopy(0, 2), 1);
		attackDownAnimationMontage.Animation.AddFrame(attackDownSpriteSheet.GetFrameCopy(0, 3), 1);

		attackDownAnimationMontage.ResetUponFinishing = true;
		attackDownAnimationMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		attackDownAnimationMontage.LoopNumber = 1;
		attackDownAnimationMontage.StateRemovalType = EStateRemovalType::AllStateMachines;

		attackForwardAnimationMontage.Animation.SetFramesPerSecond(12);
		attackForwardAnimationMontage.Animation.CanLoop(false);
		attackForwardAnimationMontage.Animation.AddFrame(attackForwardSpriteSheet.GetFrameCopy(0, 0), 1);
		attackForwardAnimationMontage.Animation.AddFrame(attackForwardSpriteSheet.GetFrameCopy(0, 1), 1);
		attackForwardAnimationMontage.Animation.AddFrame(attackForwardSpriteSheet.GetFrameCopy(0, 2), 1);
		attackForwardAnimationMontage.Animation.AddFrame(attackForwardSpriteSheet.GetFrameCopy(0, 3), 1);
		
		attackForwardAnimationMontage.ResetUponFinishing = true;
		attackForwardAnimationMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		attackForwardAnimationMontage.LoopNumber = 1;
		attackForwardAnimationMontage.StateRemovalType = EStateRemovalType::AllStateMachines;

		CreateDamageBox(FPoint2f{ 80,0 }, 50, 80, SActionNames::AttackForward, &attackForwardAnimationMontage, entityHandle);
		UEntityHandle attackDownDamageBox{ CreateDamageBox(FPoint2f{ 0, -30 }, 50, 50, SActionNames::AttackDown, &attackDownAnimationMontage, entityHandle) };

		// LOGIC 


		// --- SPAWN LOGIC ---

		FEventTriggerCondition newSpawn_ExecutionCondition_1{};
		newSpawn_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		newSpawn_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("SpawnPoint"), EEventState::Active);
		playerStateMachine.AddConditionsToExecute(SActionNames::NewSpawn, newSpawn_ExecutionCondition_1);

		FEventTriggerCondition newSpawn_RemovalCondition_1{};
		newSpawn_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		newSpawn_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("SpawnPoint"), EEventState::Inactive);
		playerStateMachine.AddConditionsToRemove(SActionNames::NewSpawn, newSpawn_RemovalCondition_1);

		// ---- ATTACK DOWN LOGIC ----

		FEventTriggerCondition attackDown_TriggerCondition_1{};
		attackDown_TriggerCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		attackDown_TriggerCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Inactive);
		attackDown_TriggerCondition_1.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"), EEventState::Inactive);
		attackDown_TriggerCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		attackDown_TriggerCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		playerStateMachine.AddConditionsToExecute(SActionNames::AttackDown, attackDown_TriggerCondition_1);

		UStateCondition attackDown_RemovalCondition_1{m_Registry};
		attackDown_RemovalCondition_1.Set(SEventNames::OnGroundCollision,	EEventState::Active, entityHandle);
		attackDown_RemovalCondition_1.And(SEventNames::OnAttackHit,			EEventState::Inactive, attackDownDamageBox);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackDown, attackDown_RemovalCondition_1, EEventTriggerConditionType::GateAndTrigger);

		FEventTriggerCondition attackDown_RemovalCondition_2{};
		attackDown_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackDown_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::AttackForward, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackDown, attackDown_RemovalCondition_2);

		FEventTriggerCondition attackDown_RemovalCondition_3{};
		attackDown_RemovalCondition_3.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackDown_RemovalCondition_3.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"), EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackDown, attackDown_RemovalCondition_3);

		// ---- REBOUND LOGIC ----

		UStateCondition rebound_ExecutionCondition{ m_Registry };
		rebound_ExecutionCondition.Set(SEventNames::OnGroundCollision,	EEventState::Active, attackDownDamageBox);
		rebound_ExecutionCondition.And(SEventNames::OnAttackHit,		EEventState::Active, attackDownDamageBox);
		rebound_ExecutionCondition.And(SActionNames::AttackDown,		EEventState::Active, attackDownDamageBox);

		playerStateMachine.AddConditionsToExecute(SActionNames::Rebound, rebound_ExecutionCondition, EEventTriggerConditionType::OnlyTrigger);
		
		UStateCondition rebound_RemovalCondition{ m_Registry };
		rebound_RemovalCondition.Set(SEventNames::OnAttackHit,		 EEventState::Inactive, attackDownDamageBox);
		rebound_RemovalCondition.And(SEventNames::OnGroundCollision, EEventState::Inactive, attackDownDamageBox);
		rebound_RemovalCondition.And(SEventNames::OnFall,			 EEventState::Active, entityHandle);
		playerStateMachine.AddConditionsToRemove(SActionNames::Rebound, rebound_RemovalCondition, EEventTriggerConditionType::OnlyTrigger);

		// ---- ADD IMMUNITY LOGIC ----

		UStateCondition immunity_ExecutionCondition{ m_Registry };
		immunity_ExecutionCondition.Set(SActionNames::AttackDown, EEventState::Active, entityHandle);
		playerStateMachine.AddConditionsToExecute(SActionNames::Immune, immunity_ExecutionCondition, EEventTriggerConditionType::OnlyTrigger);

		UStateCondition immunity_RemovalCondition{ m_Registry };
		immunity_RemovalCondition.Set(SActionNames::AttackDown, EEventState::Inactive, entityHandle);
		playerStateMachine.AddConditionsToRemove(SActionNames::Immune, immunity_RemovalCondition, EEventTriggerConditionType::OnlyTrigger);

		UStateCondition immunity_RemovalCondition_2{ m_Registry };
		immunity_RemovalCondition_2.Set(SActionNames::Die, EEventState::Inactive, entityHandle);
		playerStateMachine.AddConditionsToRemove(SActionNames::Immune, immunity_RemovalCondition_2, EEventTriggerConditionType::OnlyGate);


		playerStateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalExecution, SActionNames::Immune, EEventStateToTrigger::ForcedExecution);
		playerStateMachine.AddReactingState(SActionNames::Spawn, EEventStateToTrigger::ForcedRemoval, SActionNames::Immune, EEventStateToTrigger::ForcedRemoval);

		
		// ---- ADD NON IMMUNITY LOGIC ----

		UStateCondition nonImmunity_ExecutionCondition{ m_Registry };
		nonImmunity_ExecutionCondition.Set(SActionNames::AttackDown, EEventState::Inactive, entityHandle);
		playerStateMachine.AddConditionsToExecute(SActionNames::NotImmune, nonImmunity_ExecutionCondition, EEventTriggerConditionType::OnlyTrigger);

		nonImmunity_ExecutionCondition.Set(SActionNames::Die, EEventState::Inactive, entityHandle);
		playerStateMachine.AddConditionsToExecute(SActionNames::NotImmune, nonImmunity_ExecutionCondition, EEventTriggerConditionType::OnlyGate);
		
		UStateCondition nonImmunity_RemovalCondition{ m_Registry };
		nonImmunity_RemovalCondition.Set(SActionNames::AttackDown, EEventState::Active, entityHandle);
		playerStateMachine.AddConditionsToRemove(SActionNames::NotImmune, nonImmunity_RemovalCondition, EEventTriggerConditionType::OnlyTrigger);

		playerStateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalExecution, SActionNames::NotImmune, EEventStateToTrigger::ForcedRemoval);
		playerStateMachine.AddReactingState(SActionNames::Spawn, EEventStateToTrigger::ForcedRemoval, SActionNames::NotImmune, EEventStateToTrigger::ForcedExecution);

		

		// ---- ATTACK FORWARD LOGIC ----

		FEventTriggerCondition attackForward_ExecutionCondition_1{};
		attackForward_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		attackForward_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"),	EEventState::Inactive);
		attackForward_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Wounded,						EEventState::Inactive);
		attackForward_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Die,							EEventState::Inactive);
		playerStateMachine.AddConditionsToExecute(SActionNames::AttackForward, attackForward_ExecutionCondition_1);

		FEventTriggerCondition attackForward_RemovalCondition_1{};
		attackForward_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackForward_RemovalCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackDown, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackForward, attackForward_RemovalCondition_1); 
		
		FEventTriggerCondition attackForward_RemovalCondition_2{};
		attackForward_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackForward_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::Wounded, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackForward, attackForward_RemovalCondition_2);

		FEventTriggerCondition attackForward_RemovalCondition_3{};
		attackForward_RemovalCondition_3.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackForward_RemovalCondition_3.EventName_To_ActiveState.Push(SActionNames::MoveLeft, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackForward, attackForward_RemovalCondition_3);

		FEventTriggerCondition attackForward_RemovalCondition_4{};
		attackForward_RemovalCondition_4.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackForward_RemovalCondition_4.EventName_To_ActiveState.Push(SActionNames::MoveRight, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackForward, attackForward_RemovalCondition_4);

		FEventTriggerCondition attackForward_RemovalCondition_5{};
		attackForward_RemovalCondition_5.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		attackForward_RemovalCondition_5.EventName_To_ActiveState.Push(SActionNames::ClimbIdle, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::AttackForward, attackForward_RemovalCondition_5);
		
		// ---- CLIMB LOGIC ----

		FEventTriggerCondition climb_ExecutionCondition{};
		climb_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		climb_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"), EEventState::Active);
		playerStateMachine.AddConditionsToExecute(SActionNames::MoveUp,		climb_ExecutionCondition);
		playerStateMachine.AddConditionsToExecute(SActionNames::MoveDown,	climb_ExecutionCondition);

		FEventTriggerCondition climb_RemovalCondition{};
		climb_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		climb_RemovalCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"), EEventState::Inactive);
		playerStateMachine.AddConditionsToRemove(SActionNames::MoveUp, climb_RemovalCondition);
		playerStateMachine.AddConditionsToRemove(SActionNames::MoveDown, climb_RemovalCondition);
		playerStateMachine.AddConditionsToRemove(SActionNames::ClimbIdle, climb_RemovalCondition);

		FEventTriggerCondition climbIdle_ExecutionCondition{};
		climbIdle_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		climbIdle_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Stairs"), EEventState::Active);
		climbIdle_ExecutionCondition.EventName_To_ActiveState.Push(SActionNames::MoveUp, EEventState::Inactive);
		climbIdle_ExecutionCondition.EventName_To_ActiveState.Push(SActionNames::MoveDown, EEventState::Inactive);
		playerStateMachine.AddConditionsToExecute(SActionNames::ClimbIdle, climbIdle_ExecutionCondition);
		
		FEventTriggerCondition climbIdle_RemovalCondition_1{};
		climbIdle_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		climbIdle_RemovalCondition_1.EventName_To_ActiveState.Push(SActionNames::MoveUp, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::ClimbIdle, climbIdle_RemovalCondition_1);
		
		FEventTriggerCondition climbIdle_RemovalCondition_2{};
		climbIdle_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		climbIdle_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::MoveDown, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::ClimbIdle, climbIdle_RemovalCondition_2);

		// ---- JUMP LOGIC ----

		FEventTriggerCondition jump_ExecutionCondition_1{};
		jump_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		jump_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::CanJump, EEventState::Active);
		playerStateMachine.AddConditionsToExecute(SActionNames::Jump, jump_ExecutionCondition_1);

		UStateCondition canJump_ExecutionCondition_1{ m_Registry };
		canJump_ExecutionCondition_1.Set(SActionNames::AttackDown, EEventState::Active, entityHandle);
		canJump_ExecutionCondition_1.And(SEventNames::OnAttackHit, EEventState::Active, attackDownDamageBox);
		playerStateMachine.AddConditionsToExecute(SActionNames::CanJump, canJump_ExecutionCondition_1, EEventTriggerConditionType::OnlyTrigger);

		UStateCondition canJump_ExecutionCondition_2{ m_Registry };
		canJump_ExecutionCondition_2.Set(SEventNames::OnGroundCollision, EEventState::Active, entityHandle);
		playerStateMachine.AddConditionsToExecute(SActionNames::CanJump, canJump_ExecutionCondition_2, EEventTriggerConditionType::OnlyTrigger);

		//FEventTriggerCondition canJump_RemovalCondition_1{};
		//canJump_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		//canJump_RemovalCondition_1.EventName_To_ActiveState.Push(SActionNames::Jump, EEventState::Active);
		//playerStateMachine.AddConditionsToRemove(SActionNames::CanJump, canJump_RemovalCondition_1);
		playerStateMachine.AddReactingState(SActionNames::Jump, EEventStateToTrigger::ConditionalRemoval,
			SActionNames::CanJump, EEventStateToTrigger::ForcedRemoval);

		// ---- MOVEMENT LOGIC ----


		FEventTriggerCondition move_ExecutionCondition_1{};//equivalent to if(x and x and x) -> permit action
		move_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		move_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::AttackForward, EEventState::Inactive);
		move_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		move_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		move_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Spawn, EEventState::Inactive);
		playerStateMachine.AddConditionsToExecute(SActionNames::MoveRight, move_ExecutionCondition_1);
		playerStateMachine.AddConditionsToExecute(SActionNames::MoveLeft, move_ExecutionCondition_1);

		FEventTriggerCondition move_RemovalCondition_1{};
		move_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		move_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		playerStateMachine.AddConditionsToRemove(SActionNames::MoveLeft, move_RemovalCondition_1);
		playerStateMachine.AddConditionsToRemove(SActionNames::MoveRight, move_RemovalCondition_1);

		//---- WOUNDED LOGIC ----

		//there is a bug that when you receive damage while attacking, and you stop attacking, you can't attack again

		FEventTriggerCondition wounded_ExecutionCondition_1{};
		wounded_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		wounded_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		playerStateMachine.AddConditionsToExecute(SActionNames::Wounded, wounded_ExecutionCondition_1);

		FEventTriggerCondition wounded_RemovalCondition_1{};
		wounded_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		wounded_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		playerStateMachine.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_1);

		//---- DIE TRIGGERS ----

		FEventTriggerCondition die_ExecutionCondition_1{};
		die_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		die_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		playerStateMachine.AddConditionsToExecute(SActionNames::Die, die_ExecutionCondition_1);


		// ---- SPAWN LOGIC ----
		playerStateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalRemoval, SActionNames::Spawn, EEventStateToTrigger::ForcedExecution);
		playerStateMachine.AddReactingState(SActionNames::Spawn, EEventStateToTrigger::ForcedExecution, SActionNames::Idle, EEventStateToTrigger::ForcedExecution);
		
		// ---- FINISH STATE MACHINE SETUP

		playerStateMachine.TriggerState(SActionNames::Idle, EEventStateToTrigger::ConditionalExecution);
		playerStateMachine.TriggerState(SActionNames::CanJump, EEventStateToTrigger::ConditionalExecution);
		// ---- SET STATE MACHINE ----

		m_Registry.SetComponent<UStateMachine>(playerStateMachine, entityHandle);

		return entityHandle;
	}

	void UCreatePlayer::CreateSpawnPoint(const FPoint2f& topLeftPosition, const FPoint2f& size)
	{
		UEntityHandle entityHandle{};

		FSceneNode sceneNode{};

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = topLeftPosition;
		FLocalTransform2D localTransform{};
		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};

		FRectCollider rectCollider{0, 0, size.X, size.Y};
		FStaticColliderFlag staticColliderFlag{};
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapAll};

		DTextureTransform textureTransform{};

		FTag tag{};
		tag.Name = "SpawnPoint";

		m_Registry.CreateEntity(entityHandle, 
			sceneNode,

			worldTransform,
			localTransform,
			matrix,
			realTransform,

			rectCollider,
			staticColliderFlag,
			entityResolveOverlapInfo,
			collisionBodyType,

			textureTransform,

			tag);

		// SPRITE

		USpriteSheet spriteSheet{STextureRegistry::GetConstTexture("Objects/Checkpoint.png"), 2, 6 ,0, 0};
		float scale{ size.X / spriteSheet.GetOriginalCellWidth()};
		spriteSheet.SetScaleInScreen(Point2f{ scale,scale });
		spriteSheet.SetPositionInScreen(Point2f{ 0,-size.Y/2.2f });

		FAnimationMontage lit_AnimMontage{};
		lit_AnimMontage.Animation.SetFramesPerSecond(12);
		lit_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(1, 0), 1);
		lit_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(1, 1), 1);
		lit_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(1, 2), 1);
		lit_AnimMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation;
		lit_AnimMontage.StateRemovalType = EStateRemovalType::AllStateMachines;

		FAnimationMontage unlit_AnimMontage{};
		unlit_AnimMontage.Animation.SetFramesPerSecond(12);
		unlit_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 0), 1);
		unlit_AnimMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::LoopNumberIsNotRelevantForDeactivation;
		unlit_AnimMontage.StateRemovalType = EStateRemovalType::Local;

		FAnimationMontage sparkedUp_AnimMontage{};
		sparkedUp_AnimMontage.Animation.SetFramesPerSecond(12);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 0), 1);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 1), 1);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 2), 1);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 3), 1);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 4), 1);
		sparkedUp_AnimMontage.Animation.AddFrame(spriteSheet.GetFrameCopy(0, 5), 1);
		sparkedUp_AnimMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		sparkedUp_AnimMontage.LoopNumber = 1;
		sparkedUp_AnimMontage.StateRemovalType = EStateRemovalType::AllStateMachines;

		DSprite sprite{ m_Registry, entityHandle };
		sprite.AddState(SActionNames::Lit, lit_AnimMontage);
		sprite.AddState(SActionNames::SparkedUp, sparkedUp_AnimMontage);
		sprite.AddState(SActionNames::Unlit, unlit_AnimMontage);

		sprite.LinkWithAll(SActionNames::Lit, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::SparkedUp, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::Unlit, ELinkingAction::BothWays);

		/*
		
			SOUND
		
		*/

		FSoundEffectState checkPointActivate{};
		checkPointActivate.LoopNumber = 0;
		checkPointActivate.Name = "Sound/CheckpointActivateSound.mp3";
		checkPointActivate.StateRemovalType = EStateRemovalType::Local;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, entityHandle };
		stateController_SoundEffects.AddState(SActionNames::SparkedUp, checkPointActivate);

		/*

			STATE MACHINE

		*/

		UStateMachine stateMachine{ m_Registry, entityHandle };
		stateMachine.AddStateController(stateController_SoundEffects);
		stateMachine.AddStateController(sprite);

		FEventTriggerCondition sparkUp_TriggerCondition{};
		sparkUp_TriggerCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		sparkUp_TriggerCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Player"), EEventState::Active);
		sparkUp_TriggerCondition.EventName_To_ActiveState.Push(SActionNames::Lit, EEventState::Inactive);
		stateMachine.AddConditionsToExecute(SActionNames::SparkedUp, sparkUp_TriggerCondition);

		stateMachine.AddReactingState(
			SActionNames::SparkedUp, EEventStateToTrigger::ConditionalRemoval, 
			SActionNames::Lit, EEventStateToTrigger::ForcedExecution);

		stateMachine.AddReactingState(
			SActionNames::Unlit, EEventStateToTrigger::ConditionalExecution,
			SActionNames::Lit, EEventStateToTrigger::ForcedRemoval);

		stateMachine.TriggerState(SActionNames::Unlit, EEventStateToTrigger::ForcedExecution);

		m_Registry.AddComponent(entityHandle, stateMachine);

	}

	void UCreatePlayer::CreatePlayerLifeDrawer(int sceneIndex, const FPoint2f& topLeftPosition, const FPoint2f& scale)
	{
		UEntityHandle entityHandle{};

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = topLeftPosition;
		localTransform.Transform.Scale = scale;

		URealTransform2D realTransform{};

		DTextureTransform textureTransform{};
		FPlayerUIHealth playerUIHealth{UTexture("Objects/HeartIcon.png")};
		playerUIHealth.UI_Texture.SetScale(Point2f{ scale.X,scale.Y });

		m_Registry.CreateEntity(entityHandle,
			sceneObject,
			
			localTransform,
			realTransform,
			
			textureTransform,
			playerUIHealth);

	}

	void UCreatePlayer::CreatePlayerWealthDrawer(int sceneIndex, const FPoint2f& topLeftPosition, const FPoint2f& scale)
	{
		UEntityHandle entityHandle{};

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = topLeftPosition;
		localTransform.Transform.Scale = scale;
		URealTransform2D realTransform{};

		DTextureTransform textureTransform{};
		FPlayerUIWealth playerUIWealth{};
		playerUIWealth.FontPath = "Fonts/Font.png";
		playerUIWealth.Color = Color4f{ 0,0,0,1 };
		playerUIWealth.PointSize = 12;

		m_Registry.CreateEntity(entityHandle,
			sceneObject,

			localTransform,
			realTransform,

			textureTransform,
			playerUIWealth);
	}

	UEntityHandle UCreatePlayer::CreateDamageBox(const FPoint2f& position, float height, float width, const std::string& actionThatTriggersHitBox, const FAnimationMontage* animMontage, const UEntityHandle& player)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};

		// COMPONENTS

		SetOfComponentBlocks.emplace(SComponentNames::StateMachine);
		SetOfComponentBlocks.emplace(SComponentNames::Attack);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::SpriteTransform);
		SetOfComponentBlocks.emplace(SComponentNames::RectCollider);

		params.EntityTypeName = "PlayerDamageBox";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer damageBox{};
		m_EntityCreator.CreateEntities(damageBox, params);
		size_t currentIndex{ damageBox.NumOfEntities() - 1};

		/*

		Things to set:
		- Set State Machine
		- SetParent
		- attack
		- shape and base shape
		- position
		- collision type

		*/


		//---- SET TAG
		FTag tag{};
		tag.Name = params.EntityTypeName;
		m_Registry.SetComponent<FTag>(tag, damageBox.EntityAt(currentIndex).GetConstEntity());

		// --- SPRITE STATE CONTROLLER ---

		FAnimationMontage emptyAnimMontage{};
		emptyAnimMontage.Activated = false;

		DSprite sprite{ m_Registry, damageBox.EntityAt(currentIndex) };
		sprite.AddState(SActionNames::Idle, emptyAnimMontage);
		sprite.AddState(actionThatTriggersHitBox, *animMontage);

		sprite.LinkWithAll(actionThatTriggersHitBox, ELinkingAction::BothWays);

		// --- SPAWN STATE CONTROLLER --- 

		FCollisionBodyTypeState spawnCollisionState{};
		spawnCollisionState.CollisionBodyType = ECollisionBodyType::OverlapAll;
		spawnCollisionState.StateRemovalType = EStateRemovalType::Local;

		FCollisionBodyTypeState despawnCollisionState{};
		despawnCollisionState.CollisionBodyType = ECollisionBodyType::IgnoreAll;
		despawnCollisionState.StateRemovalType = EStateRemovalType::Local;
		

		DStateController_CollisionBodyType collisionBodyType_StateController{ m_Registry, damageBox.EntityAt(currentIndex)};
		collisionBodyType_StateController.AddState(SActionNames::Spawn, spawnCollisionState);
		collisionBodyType_StateController.AddState(SActionNames::Despawn, despawnCollisionState);


		// --- SOUND STATE CONTROLLER --- 

		FSoundEffectState hit_SoundEffect{};
		hit_SoundEffect.Name = "Sound/DamageSound.mp3";
		hit_SoundEffect.ResetAfterPlay = true;
		hit_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		hit_SoundEffect.LoopNumber = 0;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, damageBox.EntityAt(currentIndex) };
		stateController_SoundEffects.AddState(SEventNames::OnAttackHit, hit_SoundEffect);

		//STATE MACHINE

		UStateMachine hitBoxStateMachine{ m_Registry, damageBox.EntityAt(currentIndex) };
		hitBoxStateMachine.AddStateController<DStateController_CollisionBodyType>(collisionBodyType_StateController);
		hitBoxStateMachine.AddStateController<DSprite>(sprite);
		hitBoxStateMachine.AddStateController<DStateController_SoundEffects>(stateController_SoundEffects);

		// LOGIC 

		UStateCondition spawn_ExecutionCondition{m_Registry};
		spawn_ExecutionCondition.Set(actionThatTriggersHitBox, EEventState::Active, player);
		hitBoxStateMachine.AddConditionsToExecute(SActionNames::Spawn, spawn_ExecutionCondition, EEventTriggerConditionType::OnlyTrigger);

		UStateCondition spawn_RemovalCondition{ m_Registry };
		spawn_RemovalCondition.Set(actionThatTriggersHitBox, EEventState::Inactive, player);
		//spawn_RemovalCondition.Set(actionThatTriggersHitBox, EEventState::Inactive, damageBox.EntityAt(currentIndex));
		hitBoxStateMachine.AddConditionsToRemove(SActionNames::Spawn, spawn_RemovalCondition, EEventTriggerConditionType::OnlyTrigger);
		
		hitBoxStateMachine.AddReactingState(SActionNames::Spawn, EEventStateToTrigger::ConditionalRemoval, SActionNames::Despawn, EEventStateToTrigger::ForcedExecution);
		hitBoxStateMachine.AddReactingState(SActionNames::Spawn, EEventStateToTrigger::ConditionalExecution, SActionNames::Despawn, EEventStateToTrigger::ForcedRemoval);

		FEventTriggerCondition animation_ExecutionCondition{};
		animation_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		animation_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnAttackHit, EEventState::Active);
		hitBoxStateMachine.AddConditionsToExecute(actionThatTriggersHitBox, animation_ExecutionCondition);

		//FEventTriggerCondition animation_ExecutionCondition_2{};
		//animation_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		//animation_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Spawn, EEventState::Active);
		//hitBoxStateMachine.AddConditionsToExecute(actionThatTriggersHitBox, animation_ExecutionCondition_2);

		//FEventTriggerCondition animation_RemovalCondition{};
		//animation_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		//animation_RemovalCondition.EventName_To_ActiveState.Push(SActionNames::Spawn, EEventState::Inactive);
		//hitBoxStateMachine.AddConditionsToRemove(actionThatTriggersHitBox, animation_RemovalCondition);
		//
		//FEventTriggerCondition animation_RemovalCondition_2{};
		//animation_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		//animation_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::Spawn, EEventState::Active);
		//hitBoxStateMachine.AddConditionsToRemove(actionThatTriggersHitBox, animation_RemovalCondition_2);
		//
		//FEventTriggerCondition animation_RemovalCondition_3{};
		//animation_RemovalCondition_3.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		//animation_RemovalCondition_3.EventName_To_ActiveState.Push(SEventNames::OnAttackHit, EEventState::Inactive);
		//hitBoxStateMachine.AddConditionsToRemove(actionThatTriggersHitBox, animation_RemovalCondition_3);


		hitBoxStateMachine.AddReactingState(
			SActionNames::Spawn, EEventStateToTrigger::ConditionalExecution,
			SActionNames::Idle, EEventStateToTrigger::ForcedExecution);

		hitBoxStateMachine.AddReactingState(
			actionThatTriggersHitBox, EEventStateToTrigger::ConditionalRemoval, 
			SActionNames::Idle, EEventStateToTrigger::ForcedExecution);

		// ---- FINISH STATE MACHINE SETUP

		hitBoxStateMachine.TriggerState(SActionNames::Despawn, EEventStateToTrigger::ConditionalExecution);

		m_Registry.SetComponent<UStateMachine>(hitBoxStateMachine, damageBox.EntityAt(currentIndex).GetConstEntity());

		//----SCENE OBJECT----

		FSceneNode hitBoxSceneObject{};
		hitBoxSceneObject.Parent = player;

		m_Registry.SetComponent<FSceneNode>(hitBoxSceneObject, damageBox.EntityAt(currentIndex).GetConstEntity());

		//----Attack----

		FAttack obstacleAttack{};
		obstacleAttack.Damage = 1;

		m_Registry.SetComponent<FAttack>(obstacleAttack, damageBox.EntityAt(currentIndex).GetConstEntity());

		//----Shape----

		FRectCollider rectCollider{};
		rectCollider.OriginalRect.Left = -width / 2.f;
		rectCollider.OriginalRect.Top = height / 2.f;
		rectCollider.OriginalRect.Width = width;
		rectCollider.OriginalRect.Height = height;

		m_Registry.SetComponent<FRectCollider>(rectCollider, damageBox.EntityAt(currentIndex).GetConstEntity());

		//----LocalTransform----

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = position;
		m_Registry.SetComponent<FLocalTransform2D>(localTransform, damageBox.EntityAt(currentIndex).GetConstEntity());

		//---- SET COLLISION TYPE

		ECollisionBodyType obstacleCollisionType{};
		obstacleCollisionType = ECollisionBodyType::OverlapAll;
		m_Registry.SetComponent<ECollisionBodyType>(obstacleCollisionType, damageBox.EntityAt(currentIndex).GetConstEntity());

		return damageBox.EntityAt(currentIndex);
	}
}