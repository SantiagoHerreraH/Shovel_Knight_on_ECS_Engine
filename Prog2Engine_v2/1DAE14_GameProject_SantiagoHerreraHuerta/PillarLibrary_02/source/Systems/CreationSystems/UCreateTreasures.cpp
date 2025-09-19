
#include "../../../include/Systems/CreationSystems/UCreateTreasures.h"
#include "../../../include/Systems/MathSystems/SGeometry.h"
#include "../../../include/Systems/MathSystems/STools.h"
#include "../../../include/InternalIncludes.h"


namespace Pillar {
	UCreateTreasures::UCreateTreasures(URegistry& registry) : 
		m_Registry(registry),
		m_EntityCreator(registry),
		m_ArchetypeCreator(registry){}

	void UCreateTreasures::CreateRandomDropableTreasures(const UEntityHandle& entityToDrop, int minNumberOfTreasures, int maxNumberOfTreasures, size_t sceneIndex)
	{
		m_CurrentSceneIndex = sceneIndex;

		int rnd = STools::GetRandomBetweenRange(minNumberOfTreasures, maxNumberOfTreasures);

		for (size_t i = 0; i < rnd; i++)
		{
			const int numberOfTreasure{ STools::GetRandomBetweenRange(0, 5) };

			switch (numberOfTreasure)
			{
			case 0:
				CreateGreyTreasure_Dropable(entityToDrop);
				break;
			case 1:
				CreateBlueTreasure_Dropable(entityToDrop);
				break;
			case 2:
				CreateGreenTreasure_Dropable(entityToDrop);
				break;
			case 3:
				CreateYellowTreasure_Dropable(entityToDrop);
				break;
			case 4:
				CreateRedTreasure_Dropable(entityToDrop);
				break;
			case 5:
				CreateMagentaTreasure_Dropable(entityToDrop);
				break;
			}

		}

	}

	void UCreateTreasures::CreateRandomTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		m_CurrentSceneIndex = sceneIndex;

		const int numberOfTreasure{ STools::GetRandomBetweenRange(0, 5) };

		switch (numberOfTreasure)
		{
		case 0:
			CreateGreyTreasure(sceneIndex, location);
			break;
		case 1:
			CreateBlueTreasure(sceneIndex, location);
			break;
		case 2:
			CreateGreenTreasure(sceneIndex, location);
			break;
		case 3:
			CreateYellowTreasure(sceneIndex, location);
			break;
		case 4:
			CreateRedTreasure(sceneIndex, location);
			break;
		case 5:
			CreateMagentaTreasure(sceneIndex, location);
			break;
		}
	}

	void UCreateTreasures::CreateTreasure(size_t sceneIndex, const FPoint2f& location, const FPoint2f& size, const Rectf& spriteSheetBounds)
	{
	
		UEntityHandle entityHandle{};

		FTag tag{};
		tag.Name = "Treasure";

		DTextureTransform textureTransform{};

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = location;
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};
		
		FRectCollider rectCollider{0,size.Y, size.X, size.Y};
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapOnlyMovable };
		FStaticColliderFlag staticColliderFlag{};

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = sceneIndex;

		m_Registry.CreateEntity(entityHandle,
			tag,
			
			textureTransform,
			
			localTransform,
			realTransform,
			matrix,
			
			rectCollider,
			entityResolveOverlapInfo,
			collisionBodyType,
			staticColliderFlag,
			
			sceneObject);

		// ----- SET TEXTURE -----


		UTexture& texture{ STextureRegistry::GetTexture(STextureNames::Treasure) };
		Point2f scaleInScreen{};
		scaleInScreen.y = size.Y / 31.f;
		scaleInScreen.x = size.X / 31.f;

		Point2f defaultTextureLocation{ -31 / 2.f, 31 / 2.f };

		//---- CREATE ANIMATION MONTAGE SETUP----

		FAnimationMontage idle_AnimationMontage{};
		idle_AnimationMontage.Animation.SetFramesPerSecond(1);
		idle_AnimationMontage.Animation.AddFrame(UTextureRef(texture, spriteSheetBounds, Point2f{ 0, 0 }, scaleInScreen), 1);

		FAnimationMontage death_AnimationMontage{};

		death_AnimationMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		death_AnimationMontage.LoopNumber = 1;
		death_AnimationMontage.StateRemovalType = EStateRemovalType::AllStateMachines;
		death_AnimationMontage.ResetUponFinishing = true;

		UTexture& deathTexture{ STextureRegistry::GetTexture(STextureNames::MoneyPoof) };
		scaleInScreen.x = size.X / 43.f;
		scaleInScreen.y = size.Y / 35.f;
		deathTexture.SetScreenPosition(Point2f{ 0,0 });
		deathTexture.SetScale(scaleInScreen);

		death_AnimationMontage.Animation.SetFramesPerSecond(12);
		death_AnimationMontage.Animation.CanLoop(false);
		death_AnimationMontage.Animation.AddFrames(deathTexture, Rectf{ 2, 37, 43, 35 }, 7, 2);

		//---- SPRITE SETUP----

		DSprite sprite{ m_Registry, entityHandle };
		sprite.AddState(SActionNames::Spawn, idle_AnimationMontage);
		sprite.AddState(SActionNames::Die, death_AnimationMontage);

		sprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::Spawn, ELinkingAction::BothWays);



		/*

			---- SET SOUND EFFECT STATE CONTROLLER ----

		*/
		FSoundEffectState coin_SoundEffect{};
		coin_SoundEffect.Name = "Sound/GemSound_2.mp3";
		coin_SoundEffect.ResetAfterPlay = true;
		coin_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		coin_SoundEffect.LoopNumber = 0;



		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, entityHandle };
		stateController_SoundEffects.AddState(SActionNames::Die, coin_SoundEffect);

		/*

			----SET ACTIVATION STATE MACHINE----

		*/

		FActivationState despawn_ActivationState{};
		despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		despawn_ActivationState.StateRemovalType = EStateRemovalType::AllStateMachines;

		FActivationState respawn_ActivationState{};
		respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation stateController_Activation{ m_Registry, entityHandle };
		stateController_Activation.AddState(SActionNames::Despawn, despawn_ActivationState);
		stateController_Activation.AddState(SActionNames::Spawn, respawn_ActivationState);

		/*

			STATE MACHINE

		*/


		UStateMachine stateMachine{ m_Registry, entityHandle };
		stateMachine.AddStateController<DSprite>(sprite);
		stateMachine.AddStateController<DStateController_Activation>(stateController_Activation);
		stateMachine.AddStateController<DStateController_SoundEffects>(stateController_SoundEffects);

		// LOGIC 


		stateMachine.AddReactingState(SEventNames::OnCollisionWith("PlayerDamageBox"), EEventStateToTrigger::ConditionalExecution, SActionNames::Die, EEventStateToTrigger::ForcedExecution);
		stateMachine.AddReactingState(SEventNames::OnCollisionWith("Player"), EEventStateToTrigger::ConditionalExecution, SActionNames::Die, EEventStateToTrigger::ForcedExecution);
		stateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalRemoval, SActionNames::Despawn, EEventStateToTrigger::ForcedExecution);

		// ---- FINISH STATE MACHINE SETUP

		stateMachine.TriggerState(SActionNames::Spawn, EEventStateToTrigger::ForcedExecution);

		// ---- SET STATE MACHINE ----

		m_Registry.AddComponent(entityHandle, stateMachine);
	}

	void UCreateTreasures::CreateDropableTreasure(const UEntityHandle& entityToDrop, const FPoint2f& size, const Rectf& spriteSheetBounds)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};

		/*

		make the first treasure the parent and the rest the children so they don't collide
		*/

		// COMPONENTS
		SetOfComponentBlocks.emplace(SComponentNames::Tag);
		SetOfComponentBlocks.emplace(SComponentNames::StateMachine);
		SetOfComponentBlocks.emplace(SComponentNames::SpriteTransform);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::RectCollider);
		SetOfComponentBlocks.emplace(SComponentNames::Physics);

		params.EntityTypeName = "Treasure";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer entityHandles{};
		m_EntityCreator.CreateEntities(entityHandles, params);
		size_t currentIndex{ entityHandles.NumOfEntities() - 1 };

		//----SET PARAMS----
		/*
		- Set Animation Graph
		- Set State Machine
		- Set Position
		- Set Shape (TriggerBox)
		- Set Gravity
		- Set physics collision

		*/

		//---- SET TAG

		FTag tag{};
		tag.Name = params.EntityTypeName;
		m_Registry.SetComponent<FTag>(tag, entityHandles.EntityAt(currentIndex).GetConstEntity());

		// ----- SET TEXTURE -----


		UTexture& texture{ STextureRegistry::GetTexture(STextureNames::Treasure) };
		Point2f scaleInScreen{};
		scaleInScreen.y = size.Y / 31.f;
		scaleInScreen.x = size.X / 31.f;

		Point2f defaultTextureLocation{ -31 / 2.f, 31 / 2.f };

		//---- CREATE ANIMATION MONTAGE SETUP----

		FAnimationMontage idle_AnimationMontage{};
		idle_AnimationMontage.Animation.SetFramesPerSecond(1);
		idle_AnimationMontage.Animation.AddFrame(UTextureRef(texture, spriteSheetBounds, Point2f{ 0, 0 }, scaleInScreen), 1);

		FAnimationMontage death_AnimationMontage{};

		death_AnimationMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		death_AnimationMontage.LoopNumber = 1;
		death_AnimationMontage.StateRemovalType = EStateRemovalType::AllStateMachines;
		death_AnimationMontage.ResetUponFinishing = true;

		UTexture& deathTexture{ STextureRegistry::GetTexture(STextureNames::MoneyPoof) };
		scaleInScreen.x = size.X / 43.f;
		scaleInScreen.y = size.Y / 35.f;
		deathTexture.SetScreenPosition(Point2f{ 0,0 });
		deathTexture.SetScale(scaleInScreen);

		death_AnimationMontage.Animation.SetFramesPerSecond(12);
		death_AnimationMontage.Animation.CanLoop(false);
		death_AnimationMontage.Animation.AddFrames(deathTexture, Rectf{ 2, 37, 43, 35 }, 7, 2);

		//---- SPRITE SETUP----

		DSprite sprite{ m_Registry, entityHandles.EntityAt(currentIndex) };
		sprite.AddState(SActionNames::Spawn, idle_AnimationMontage);
		sprite.AddState(SActionNames::Die, death_AnimationMontage);

		sprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::Spawn, ELinkingAction::BothWays);

		/*

			---- SET VECTOR MOVEMENT STATE CONTROLLER ----

		*/

		FVectorMovement spawnVectorMovement{};
		spawnVectorMovement.ApplyMovementOnXAxis = true;
		spawnVectorMovement.ApplyMovementOnYAxis = true;
		spawnVectorMovement.Movement.Y = STools::GetRandomBetweenRange(200, 400);
		spawnVectorMovement.Movement.X = STools::GetRandomBetweenRange(-50, 50);
		spawnVectorMovement.MovementType = EMovementType::OverrideVelocity;
		spawnVectorMovement.TerminateEachTick = true;

		FVectorMovement dieVectorMovement{};
		dieVectorMovement.ApplyMovementOnXAxis = true;
		dieVectorMovement.ApplyMovementOnYAxis = true;
		dieVectorMovement.Movement.Y = 0;
		dieVectorMovement.Movement.X = 0;
		dieVectorMovement.MovementType = EMovementType::OverrideVelocity;
		dieVectorMovement.TerminateEachTick = true;

		DStateController_VectorMovement stateController_VectorMovement{ m_Registry, entityHandles.EntityAt(currentIndex) };
		stateController_VectorMovement.AddState(SActionNames::Spawn, spawnVectorMovement);
		stateController_VectorMovement.AddState(SActionNames::Die, dieVectorMovement);

		/*

			---- SET TRANSFORM STATE CONTROLLER ----

		*/
		DStateController_Transform stateController_Transform{ m_Registry, entityHandles.EntityAt(currentIndex) };
		FTransformState spawnTransformState{};
		spawnTransformState.StateRemovalType = EStateRemovalType::Local;
		spawnTransformState.ReferenceEntitiesFor_Transform.Add(entityToDrop);
		spawnTransformState.Current_Transform_ReferenceEntities_Index = 0;
		spawnTransformState.IncreaseIndexAfterUsingReference = false;
		spawnTransformState.ValueModificationTypeFor_Position = EValueModificationType::OverrideFromOtherEntity;
		spawnTransformState.ValueModificationTypeFor_Rotation = EValueModificationType::DontChange;
		spawnTransformState.ValueModificationTypeFor_Scale = EValueModificationType::DontChange;

		stateController_Transform.AddState(SActionNames::Spawn, spawnTransformState);



		/*

			---- SET SOUND EFFECT STATE CONTROLLER ----

		*/
		FSoundEffectState coin_SoundEffect{};
		coin_SoundEffect.Name = "Sound/GemSound_2.mp3";
		coin_SoundEffect.ResetAfterPlay = true;
		coin_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		coin_SoundEffect.LoopNumber = 0;

		

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, entityHandles.EntityAt(currentIndex) };
		stateController_SoundEffects.AddState(SActionNames::Die, coin_SoundEffect);

		/*

			---- SET ENVIORNMENT FORCES STATE CONTROLLER

		*/

		// ---- SPAWN ----
		FMovement gravity{};
		gravity.Movement = FPoint2f{ 0,-500 };
		gravity.MovementType = EMovementType::AddForce;

		FEnvironmentForcesState spawnEnvironmentForces{};
		spawnEnvironmentForces.EnvironmentForces.ForcesToApplyPerTick.Add(gravity);
		spawnEnvironmentForces.StateRemovalType = EStateRemovalType::Local;

		// ---- DIE ----
		FEnvironmentForcesState dieEnvironmentForces{};
		dieEnvironmentForces.StateRemovalType = EStateRemovalType::Local;

		DStateController_EnvironmentForces stateController_EnvironmentForces{ m_Registry, entityHandles.EntityAt(currentIndex) };
		stateController_EnvironmentForces.AddState(SActionNames::Spawn, spawnEnvironmentForces);
		stateController_EnvironmentForces.AddState(SActionNames::Die, dieEnvironmentForces);

		/*

			---- SET COLLISION BODY TYPE STATE CONTROLLER

		*/
		//FCollisionBodyTypeState spawnCollisionBodyType{};
		//spawnCollisionBodyType.CollisionBodyType = ECollisionBodyType::Solid;
		//spawnCollisionBodyType.StateRemovalType = EStateRemovalType::Local;
		//
		//FCollisionBodyTypeState dieCollisionBodyType{};
		//dieCollisionBodyType.CollisionBodyType = ECollisionBodyType::OverlapAll;
		//dieCollisionBodyType.StateRemovalType = EStateRemovalType::Local;
		//
		//DStateController_CollisionBodyType stateController_CollisionBodyType{ m_Registry, entityHandles.EntityAt(currentIndex) };
		//stateController_CollisionBodyType.AddState(SActionNames::Spawn, spawnCollisionBodyType);
		//stateController_CollisionBodyType.AddState(SActionNames::Die, dieCollisionBodyType);

		/*

			TO DOS
			- projectile like jump on spawning
			- when player touches it
				- no gravity
				- collision becomes overlap

			On Other Systems
			- FINISH SETTING UP LEVEL
			- Do main menu and death screen
			- setup UI
				- Life
				- Coins
			- Implement Sound

		*/



		/*

			----SET ACTIVATION STATE MACHINE----

		*/

		FActivationState despawn_ActivationState{};
		despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		despawn_ActivationState.StateRemovalType = EStateRemovalType::AllStateMachines;

		FActivationState respawn_ActivationState{};
		respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation stateController_Activation{ m_Registry, entityHandles.EntityAt(currentIndex) };
		stateController_Activation.AddState(SActionNames::Despawn, despawn_ActivationState);
		stateController_Activation.AddState(SActionNames::Spawn, respawn_ActivationState);

		/*

			STATE MACHINE

		*/


		UStateMachine stateMachine{ m_Registry, entityHandles.EntityAt(currentIndex) };
		stateMachine.AddStateController<DSprite>(sprite);
		stateMachine.AddStateController<DStateController_Activation>(stateController_Activation);
		stateMachine.AddStateController<DStateController_VectorMovement>(stateController_VectorMovement);
		stateMachine.AddStateController<DStateController_Transform>(stateController_Transform);
		stateMachine.AddStateController<DStateController_EnvironmentForces >(stateController_EnvironmentForces);
		stateMachine.AddStateController<DStateController_SoundEffects>(stateController_SoundEffects);

		// LOGIC 

		UStateCondition spawnCondition{ m_Registry };
		spawnCondition.Set(SActionNames::Despawn, EEventState::Active, entityToDrop);
		stateMachine.AddConditionsToExecute(SActionNames::Spawn, spawnCondition, EEventTriggerConditionType::OnlyTrigger);

		stateMachine.AddReactingState(SEventNames::OnCollisionWith("PlayerDamageBox"), EEventStateToTrigger::ConditionalExecution, SActionNames::Die, EEventStateToTrigger::ForcedExecution);
		stateMachine.AddReactingState(SEventNames::OnCollisionWith("Player"), EEventStateToTrigger::ConditionalExecution, SActionNames::Die, EEventStateToTrigger::ForcedExecution);
		stateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalRemoval, SActionNames::Despawn, EEventStateToTrigger::ForcedExecution);

		// ---- FINISH STATE MACHINE SETUP

		stateMachine.TriggerState(SActionNames::Despawn, EEventStateToTrigger::ConditionalExecution);

		// ---- SET STATE MACHINE ----

		m_Registry.SetComponent<UStateMachine>(stateMachine, entityHandles.EntityAt(currentIndex).GetConstEntity());

		//---- SET SHAPE ----

		FRectCollider playerCollider{};
		playerCollider.OriginalRect.Top = size.Y;
		playerCollider.OriginalRect.Left = 0;
		playerCollider.OriginalRect.Height = size.Y;
		playerCollider.OriginalRect.Width = size.X;

		m_Registry.SetComponent<FRectCollider>(playerCollider, entityHandles.EntityAt(currentIndex).GetConstEntity());

		//---- SET COLLISION TYPE

		ECollisionBodyType collisionType{};
		collisionType = ECollisionBodyType::OverlapOnlyMovable;
		m_Registry.SetComponent<ECollisionBodyType>(collisionType, entityHandles.EntityAt(currentIndex).GetConstEntity());


		//---- SET SCENE HIERARCHY

		CreateOrigin();
		FSceneNode sceneNode{};
		sceneNode.Parent = m_Origin;
		m_Registry.SetComponent<FSceneNode>(sceneNode, entityHandles.EntityAt(currentIndex).GetConstEntity());


		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		FEntity entity{ entityHandles.EntityAt(currentIndex).GetConstEntity()};
		m_Registry.AddComponent(entity, sceneObject);

		m_Registry.DeactivateEntity(entityHandles.EntityAt(currentIndex).GetConstEntity());
	}

	void UCreateTreasures::CreateGreyTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_GreyRect);
	}

	void UCreateTreasures::CreateBlueTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_BlueRect);
	}

	void UCreateTreasures::CreateGreenTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_GreenRect);
	}

	void UCreateTreasures::CreateYellowTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_YellowRect);
	}

	void UCreateTreasures::CreateRedTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_RedRect);
	}

	void UCreateTreasures::CreateMagentaTreasure_Dropable(const UEntityHandle& entityToDrop)
	{
		CreateDropableTreasure(entityToDrop, m_TreasureSize, m_MagentaRect);
	}

	void UCreateTreasures::CreateGreyTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_GreyRect);
	}

	void UCreateTreasures::CreateBlueTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_BlueRect);
	}

	void UCreateTreasures::CreateGreenTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_GreenRect);
	}

	void UCreateTreasures::CreateYellowTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_YellowRect);
	}

	void UCreateTreasures::CreateRedTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_RedRect);
	}

	void UCreateTreasures::CreateMagentaTreasure(size_t sceneIndex, const FPoint2f& location)
	{
		CreateTreasure(sceneIndex, location, m_TreasureSize, m_MagentaRect);
	}

	void UCreateTreasures::CreateOrigin()
	{
		if (!m_Origin.IsValid())
		{
			m_Origin = CreateCheckPoint(FPoint2f{ 0,0 });
		}
	}

	UEntityHandle UCreateTreasures::CreateCheckPoint(const FPoint2f& position)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};

		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);

		params.EntityTypeName = "CheckPoint";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer checkPoints{};
		m_EntityCreator.CreateEntities(checkPoints, params);//this gets all of the entities of the current archetype?
		size_t currentEntityIndex{ checkPoints.NumOfEntities() - 1 };

		//----WorldTransform----

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = position;
		m_Registry.SetComponent<FWorldTransform2D>(worldTransform, checkPoints.EntityAt(currentEntityIndex).GetConstEntity());

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		FEntity entity{ checkPoints.EntityAt(currentEntityIndex).GetConstEntity() };
		m_Registry.AddComponent(entity, sceneObject);

		return checkPoints.ConstEntityAt(currentEntityIndex);
	}

}