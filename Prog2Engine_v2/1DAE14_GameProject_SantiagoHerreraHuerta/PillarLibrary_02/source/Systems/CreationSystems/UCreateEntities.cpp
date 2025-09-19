#include "../../../include/Systems/CreationSystems/UCreateEntities.h"
#include "../../../include/Systems/CreationSystems/UCreateComponents.h"

#include "../../../include/Systems/MathSystems/SGeometry.h"
#include "../../../include/Systems/MathSystems/STools.h"

#include "../../../include/InternalIncludes.h"

namespace Pillar {

	UCreateEntities::UCreateEntities(URegistry& registry) :
		m_Registry(registry),
		m_CreatePlayer(registry),
		m_EntityCreator(registry),
		m_ArchetypeCreator(registry),
		m_CreateTreasures(registry),
		m_CreateMenu(registry){}

	void UCreateEntities::CreateAndSetParams()
	{
		CreateMainMenu();
		CreateLevelOne();
		m_CreateMenu.CreateWinningScreen(m_CurrentSceneIndex);
		//m_CreatePlayer.CreatePlayerWealthDrawer(m_CurrentSceneIndex, FPoint2f{ g_WINDOW.Width/2.f - 50, 200 }, FPoint2f{ 3,3 });
		CreateBackgroundMusic("Sound/ShovelKnightSong_2.mp3");
	}

	void UCreateEntities::CreateMainMenu()
	{
		m_CreateMenu.CreateMainMenuScreen(m_CurrentSceneIndex);
		m_CreateMenu.CreateMainMenuShovel(m_CurrentSceneIndex, FPoint2f{-220 * 1.3,-58 * 1.3 }, FPoint2f{400 * 1.3,50 * 1.3 }, FPoint2f{ 3 * 1.3,3 * 1.3 });
		CreateBackgroundMusic("Sound/ShovelKnightSong_1.mp3");
		++m_CurrentSceneIndex;
	}

	void UCreateEntities::CreateLevelOne()
	{
		CreateBackground();
		CreateBackgroundFortress();
		CreateLevel(FPoint2f{ 3.2,3.2 });
		CreateCamera(m_Player, 100, 100);
		CreateBackgroundMusic("Sound/ShovelKnightSong_0.mp3");
		m_CreatePlayer.CreatePlayerLifeDrawer(m_CurrentSceneIndex, FPoint2f{ 10,g_WINDOW.Height - 10 }, FPoint2f{5,5});

		++m_CurrentSceneIndex;
	}

	void UCreateEntities::CreateLevelChanger(const FPoint2f& bottomLeftPosition, const FPoint2f& size)
	{

		UEntityHandle entityHandle{};

		FRectCollider rectCollider{0, size.Y, size.X, size.Y};
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapAll};
		FStaticColliderFlag staticColliderFlag{};

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = bottomLeftPosition;
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		m_Registry.CreateEntity(entityHandle, 
			rectCollider, 
			entityResolveOverlapInfo,
			collisionBodyType,
			staticColliderFlag,
			
			localTransform,
			realTransform,
			matrix,

			sceneObject);

		const std::string changeScene{ "ChangeScene" };

		FSceneState sceneState{};
		sceneState.SceneDeltaFromCurrentState = 1;
		sceneState.StateRemovalType = EStateRemovalType::AllStateMachines;

		DStateController_SceneChanger stateController_SceneChanger{ m_Registry, entityHandle};
		stateController_SceneChanger.AddState(changeScene, sceneState);

		UStateMachine stateMachine{ m_Registry, entityHandle };
		stateMachine.AddStateController(stateController_SceneChanger);

		FEventTriggerCondition changeScene_ExecutionCondition{};
		changeScene_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		changeScene_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Player"),EEventState::Active );
		stateMachine.AddConditionsToExecute(changeScene, changeScene_ExecutionCondition);

		FEventTriggerCondition changeScene_RemovalCondition{};
		changeScene_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		changeScene_RemovalCondition.EventName_To_ActiveState.Push(SEventNames::OnCollisionWith("Player"), EEventState::Inactive);
		stateMachine.AddConditionsToRemove(changeScene, changeScene_RemovalCondition);

		m_Registry.AddComponent(entityHandle, stateMachine);

	}

	void UCreateEntities::CreateBackgroundMusic(const std::string& soundTrack)
	{
		FEntity entity{};

		FBackgroundMusic backgroundMusic{};
		backgroundMusic.Name = soundTrack;

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		m_Registry.CreateEntity(entity,
			backgroundMusic,
			sceneObject);
	}

	void UCreateEntities::CreateBackground()
	{
		FEntity entity{};


		FSceneTexture sceneTexture{ UTexture("Levels/Clouds.png") };
		sceneTexture.Texture.SetScreenPosition(Point2f{ 0,0 });
		sceneTexture.Texture.SetScale(Point2f{
			g_WINDOW.Width/ sceneTexture.Texture.GetTextureOriginalWidth(),
			g_WINDOW.Height/ sceneTexture.Texture.GetTextureOriginalHeight() });

		DTextureTransform textureTransform{};
		textureTransform.Scale = Vector2f{1,1};


		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		m_Registry.CreateEntity(entity, 
			textureTransform, 
			sceneTexture, 
			sceneObject);
	}

	void UCreateEntities::CreateTree(const FPoint2f& bottomLeftPosition, const FPoint2f& scale)
	{
		FEntity entity{};

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = bottomLeftPosition;

		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};
		DTextureTransform textureTransform{};

		FSceneTexture sceneTexture{ UTexture("Levels/Tree.png") };
		sceneTexture.Texture.SetScreenPosition(Point2f{ 0,0 });
		sceneTexture.Texture.SetScale(Point2f{ scale.X, scale.Y });

		FZDepth zDepth{};
		zDepth.Automatic = true;

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		FRectCollider collider{0, 1, 1, 1};//doesn't really matter
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapAll};
		FStaticColliderFlag staticColliderFlag{};


		m_Registry.CreateEntity(entity,

			localTransform,
			matrix,
			realTransform,

			textureTransform,
			sceneTexture,

			zDepth, 

			sceneObject,
			
			collider,
			entityResolveOverlapInfo,
			collisionBodyType,
			staticColliderFlag);
	}

	void UCreateEntities::CreateBackgroundFortress()
	{
		FEntity entity{};

		DTextureTransform textureTransform{};

		FSceneTexture sceneTexture{ UTexture("Levels/Castle.png") };
		sceneTexture.Texture.SetScreenPosition(Point2f{ 0,150 });
		sceneTexture.Texture.SetScale(Point2f{
			g_WINDOW.Width / sceneTexture.Texture.GetTextureOriginalWidth(),
			g_WINDOW.Width / sceneTexture.Texture.GetTextureOriginalWidth(), });

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		URealTransform2D realTransform{};
		FZDepth zDepth{};
		zDepth.ZDistance = 2;

		m_Registry.CreateEntity(entity,
			textureTransform,
			sceneTexture,
			sceneObject);
	}

	void UCreateEntities::CreateLevel(const FPoint2f& scale)
	{
		FEntity levelEntity{};
		UTextureMap levelOne(USpriteSheet(UTexture(STextureRegistry::GetTexture(STextureNames::Tiles)), 10, 14, 0, 0));

		/*
		
			DEFINE COLORS
		
		*/
		//	DEBUG -> 406, 124
		//GroundLayer
		const FColor4i groundColor{ 153, 229, 80, 255 };
		const FColor4i backgroundMountainColor{ 9, 30, 107, 255 };
		const FColor4i backgroundMountainTracksColor{ 98, 89, 170, 255 };

		const FColor4i waterColor{ 82, 246, 237, 255 };

		//Details Layer
		const FColor4i vegetationColor{ 0, 183, 0, 255 };
		const FColor4i wallIndicationColor{ 0, 0, 0, 255 };

		const FColor4i destroyableTileColor_Big_LeftBottomCorner{ 229, 190, 80, 255 };
		const FColor4i destroyableTileColor_Big{ 170, 164, 51, 255 };
		const FColor4i destroyableTileColor_Small{ 203, 142, 98, 255 };

		const FColor4i spikesColor{ 161, 37, 37, 255 };
		const FColor4i nonSpikesDamagingColor{255, 9 , 9, 255};

		const FColor4i stairsColor{ 229, 127, 80, 255 };
		const FColor4i spawnPointsColor{ 229, 221, 80, 255 };

		const FColor4i treasureColor{ 251, 4, 212, 255 };


		/*

			CREATE TILE RULES

		*/

		/*
			SET GROUND COLOR TILE BLOCKS
		*/
		levelOne.SetRulesForTileBlock(0,  0,  3,  2, groundColor, true, 1);
		levelOne.SetRulesForTileBlock(0,  3,  0,  5, groundColor);
		levelOne.SetRulesForTileBlock(1,  5,  4,  5, groundColor);
		levelOne.SetRulesForTileBlock(3,  8,  3,  9, groundColor);
		levelOne.SetRulesForTileBlock(2, 10, 4, 12, groundColor);
		levelOne.SetRulesForTileBlock(5,  4, 7,  5, groundColor);
		levelOne.SetRulesForTileBlock(5, 10, 6, 12, groundColor);
		
		UTextureMapRuleBook ruleFor_InvertedEdge_DownLeftTile{};
		ruleFor_InvertedEdge_DownLeftTile.SetCenterColor(groundColor);
		ruleFor_InvertedEdge_DownLeftTile.AddColorRule(groundColor, -1, -1, false);
		ruleFor_InvertedEdge_DownLeftTile.AddColorRule(groundColor, -1, 0, true);
		ruleFor_InvertedEdge_DownLeftTile.AddColorRule(groundColor, 0, -1, true);
		ruleFor_InvertedEdge_DownLeftTile.AddColorRule(groundColor, 1, 1, true);
		
		FPoint2f spriteCellFor_InvertedEdge_DownLeftTile{10,1};
		
		UTextureMapRuleBook ruleFor_InvertedEdge_DownRightTile{};
		ruleFor_InvertedEdge_DownRightTile.SetCenterColor(groundColor);
		ruleFor_InvertedEdge_DownRightTile.AddColorRule(groundColor, 1, -1, false);
		ruleFor_InvertedEdge_DownRightTile.AddColorRule(groundColor,  0, -1, true);
		ruleFor_InvertedEdge_DownRightTile.AddColorRule(groundColor, 1, 0, true);
		ruleFor_InvertedEdge_DownRightTile.AddColorRule(groundColor, -1, 1, true);
		FPoint2f spriteCellFor_InvertedEdge_DownRightTile{11,1};
		
		UTextureMapRuleBook ruleFor_InvertedEdge_TopLeftTile{};
		ruleFor_InvertedEdge_TopLeftTile.SetCenterColor(groundColor);
		ruleFor_InvertedEdge_TopLeftTile.AddColorRule(groundColor, -1, 1, false);
		ruleFor_InvertedEdge_TopLeftTile.AddColorRule(groundColor, -1, 0, true);
		ruleFor_InvertedEdge_TopLeftTile.AddColorRule(groundColor, 0, 1, true);
		ruleFor_InvertedEdge_TopLeftTile.AddColorRule(groundColor, 1, -1, true);
		FPoint2f spriteCellFor_InvertedEdge_TopLeftTile{10,0};
		
		UTextureMapRuleBook ruleFor_InvertedEdge_TopRightTile{};
		ruleFor_InvertedEdge_TopRightTile.SetCenterColor(groundColor);
		ruleFor_InvertedEdge_TopRightTile.AddColorRule(groundColor, 1, 1, false);
		ruleFor_InvertedEdge_TopRightTile.AddColorRule(groundColor, 0, 1, true);
		ruleFor_InvertedEdge_TopRightTile.AddColorRule(groundColor, 1, 0, true);
		ruleFor_InvertedEdge_TopRightTile.AddColorRule(groundColor, -1, -1, true);
		FPoint2f spriteCellFor_InvertedEdge_TopRightTile{11,0};

		UTextureMapRuleBook normalGroundTile_Rule{};
		normalGroundTile_Rule.SetCenterColor(groundColor);
		levelOne.SetRuleForTile(4, 5, normalGroundTile_Rule);

		UTextureMapRuleBook defaultUpGroundTile_Rule{};
		defaultUpGroundTile_Rule.SetCenterColor(groundColor);
		defaultUpGroundTile_Rule.AddColorRule(groundColor, 0, 1, false);
		levelOne.SetRuleForTile(0, 1, defaultUpGroundTile_Rule);

		UTextureMapRuleBook defaultDownGroundTile_Rule{};
		defaultDownGroundTile_Rule.SetCenterColor(groundColor);
		defaultDownGroundTile_Rule.AddColorRule(groundColor, 0, -1, false);
		levelOne.SetRuleForTile(3, 1, defaultDownGroundTile_Rule);

		levelOne.SetRuleForTile(
			spriteCellFor_InvertedEdge_DownLeftTile.Y,
			spriteCellFor_InvertedEdge_DownLeftTile.X,
			ruleFor_InvertedEdge_DownLeftTile);
		
		levelOne.SetRuleForTile(
			spriteCellFor_InvertedEdge_DownRightTile.Y,
			spriteCellFor_InvertedEdge_DownRightTile.X,
			ruleFor_InvertedEdge_DownRightTile);
		
		levelOne.SetRuleForTile(
			spriteCellFor_InvertedEdge_TopLeftTile.Y,
			spriteCellFor_InvertedEdge_TopLeftTile.X,
			ruleFor_InvertedEdge_TopLeftTile);
		
		levelOne.SetRuleForTile(
			spriteCellFor_InvertedEdge_TopRightTile.Y,
			spriteCellFor_InvertedEdge_TopRightTile.X,
			ruleFor_InvertedEdge_TopRightTile);

		/*
			SET MOUNTAIN BACKGROUND BLOCKS
		*/

		//NORMAL TILES

		std::vector<FColor4i> colorsToOmitBackground{ backgroundMountainTracksColor, groundColor };

		levelOne.SetBorderTile(	5,	0, backgroundMountainColor, false, true,  true,  false, colorsToOmitBackground);
		levelOne.SetBorderTile(	5,	1, backgroundMountainColor, false, false, true,  false, colorsToOmitBackground);
		levelOne.SetBorderTile(	5,	2, backgroundMountainColor, true,  false, true,  false, colorsToOmitBackground);
		levelOne.SetBorderTile(	6,	0, backgroundMountainColor, false, true,  false, false, colorsToOmitBackground);
		levelOne.SetBorderTile(	6,	1, backgroundMountainColor, false,  false, false, false);//center tile
		levelOne.SetBorderTile(	6,	2, backgroundMountainColor, true,  false, false, false, colorsToOmitBackground);
		levelOne.SetBorderTile(	7,	0, backgroundMountainColor, false, true,  false, true, colorsToOmitBackground);
		levelOne.SetBorderTile(	7,	1, backgroundMountainColor, false, false, false, true, colorsToOmitBackground);
		levelOne.SetBorderTile(	7,	2, backgroundMountainColor, true,  false, false, true, colorsToOmitBackground);

		//DIAGONAL TILES
		
		levelOne.SetDiagonalBorderTile(7, 9, backgroundMountainColor, true, false, false, true, colorsToOmitBackground);//right, bottom
		levelOne.SetDiagonalBorderTile(4, 9, backgroundMountainColor, true, false, true, false, colorsToOmitBackground);//right, top
		levelOne.SetDiagonalBorderTile(4,6, backgroundMountainColor, false, true, true, false, colorsToOmitBackground);//left, top
		levelOne.SetDiagonalBorderTile(7,6, backgroundMountainColor, false, true, false, true, colorsToOmitBackground);//left, bottom
		
		UTextureMapRuleBook topLeft__backgroundBlock_EdgeRule{};
		topLeft__backgroundBlock_EdgeRule.SetCenterColor(backgroundMountainColor);
		topLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, -1, 0, true);
		topLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 0, 1, true);
		topLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, -1, 1, false);
		topLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainTracksColor, -1, 1, false);
		topLeft__backgroundBlock_EdgeRule.AddColorRule(groundColor, -1, 1, false);

		levelOne.SetRuleForTile(8, 1, topLeft__backgroundBlock_EdgeRule);

		UTextureMapRuleBook topRight__backgroundBlock_EdgeRule{};
		topRight__backgroundBlock_EdgeRule.SetCenterColor(backgroundMountainColor);
		topRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 1, 0, true);
		topRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 0, 1, true);
		topRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 1, 1, false);
		topRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainTracksColor, 1, 1, false);
		topRight__backgroundBlock_EdgeRule.AddColorRule(groundColor, 1, 1, false);
		levelOne.SetRuleForTile(8, 0, topRight__backgroundBlock_EdgeRule);

		UTextureMapRuleBook bottomLeft__backgroundBlock_EdgeRule{};
		bottomLeft__backgroundBlock_EdgeRule.SetCenterColor(backgroundMountainColor);
		bottomLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, -1, 0, true);
		bottomLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 0, -1, true);
		bottomLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, -1, -1, false);
		bottomLeft__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainTracksColor, -1, -1, false);
		bottomLeft__backgroundBlock_EdgeRule.AddColorRule(groundColor, -1, -1, false);
		levelOne.SetRuleForTile(8, 3, bottomLeft__backgroundBlock_EdgeRule);

		UTextureMapRuleBook bottomRight__backgroundBlock_EdgeRule{};
		bottomRight__backgroundBlock_EdgeRule.SetCenterColor(backgroundMountainColor);
		bottomRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 1, 0, true);
		bottomRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 0, -1, true);
		bottomRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainColor, 1, -1, false);
		bottomRight__backgroundBlock_EdgeRule.AddColorRule(backgroundMountainTracksColor, 1, -1, false);
		bottomRight__backgroundBlock_EdgeRule.AddColorRule(groundColor, 1, -1, false);
		levelOne.SetRuleForTile(8, 2, bottomRight__backgroundBlock_EdgeRule);

		levelOne.SetRulesForTileBlock(4, 0, 4, 2, backgroundMountainTracksColor);
		levelOne.SetRulesForTileBlock(5, 3, 7, 3, backgroundMountainTracksColor);

		/*

			SET INTERACTIVES

		*/

		// --- SPAWN POINTS BLOCKS ---

		UTextureMapRuleBook spawnPointRule{};

		levelOne.SetBorderTile(9, 13, spawnPointsColor, false, false, false, false);

		UTextureMapRuleBook bottom_SpawnPointRule{};
		bottom_SpawnPointRule.SetCenterColor(spawnPointsColor);
		bottom_SpawnPointRule.AddColorRule(spawnPointsColor, 0, 1, true);
		levelOne.SetRuleForTile(8, 4, bottom_SpawnPointRule);


		UTextureMapRuleBook mid_SpawnPointRule{};
		mid_SpawnPointRule.SetCenterColor(spawnPointsColor);
		mid_SpawnPointRule.AddColorRule(spawnPointsColor, 0, 1, true);
		mid_SpawnPointRule.AddColorRule(spawnPointsColor, 0, -1, true);
		levelOne.SetRuleForTile(8, 5, mid_SpawnPointRule);

		
		// --- DESTROYABLE BLOCKS ---

		//OTHERS
		levelOne.SetBorderTile(2, 9, stairsColor, false, false, false, false);
		
		levelOne.SetBorderTile(1, 12, spikesColor, false, false, false, false);

		/*

			SET VEGETATION

		*/

		levelOne.SetRulesForTileBlock(1, 3, 2, 4, vegetationColor);
		levelOne.SetRulesForTileBlock(2, 6, 2, 8, vegetationColor);
		levelOne.SetRulesForTileBlock(3, 6, 3, 7, vegetationColor);
		levelOne.SetRulesForTileBlock(0, 6, 1, 9, vegetationColor);
		
		UTextureMapRuleBook hangingBush_FacingRight_TopPart_Rule{};
		hangingBush_FacingRight_TopPart_Rule.SetCenterColor(vegetationColor);
		hangingBush_FacingRight_TopPart_Rule.AddColorRule(vegetationColor, 0 ,-1, true);
		hangingBush_FacingRight_TopPart_Rule.AddColorRule(vegetationColor, 1, 0, false);
		hangingBush_FacingRight_TopPart_Rule.AddColorRule(wallIndicationColor, -1, 0, true);
		
		UTextureMapRuleBook hangingBush_FacingRight_BottomPart_Rule{};
		hangingBush_FacingRight_BottomPart_Rule.SetCenterColor(vegetationColor);
		hangingBush_FacingRight_BottomPart_Rule.AddColorRule(vegetationColor, 0, 1, true);
		hangingBush_FacingRight_BottomPart_Rule.AddColorRule(vegetationColor, 1, 0, false);
		hangingBush_FacingRight_BottomPart_Rule.AddColorRule(wallIndicationColor, -1, 0, true);
		
		levelOne.SetRuleForTile(3, 3, hangingBush_FacingRight_TopPart_Rule);
		levelOne.SetRuleForTile(4, 3, hangingBush_FacingRight_BottomPart_Rule);
		
		UTextureMapRuleBook hangingBush_FacingLeft_TopPart_Rule{};
		hangingBush_FacingLeft_TopPart_Rule.SetCenterColor(vegetationColor);
		hangingBush_FacingLeft_TopPart_Rule.AddColorRule(vegetationColor, 0, -1, true);
		hangingBush_FacingLeft_TopPart_Rule.AddColorRule(vegetationColor, -1, 0, false);
		hangingBush_FacingLeft_TopPart_Rule.AddColorRule(wallIndicationColor, 1, 0, true);
		
		UTextureMapRuleBook hangingBush_FacingLeft_BottomPart_Rule{};
		hangingBush_FacingLeft_BottomPart_Rule.SetCenterColor(vegetationColor);
		hangingBush_FacingLeft_BottomPart_Rule.AddColorRule(vegetationColor, 0, 1, true);
		hangingBush_FacingLeft_BottomPart_Rule.AddColorRule(vegetationColor, -1, 0, false);
		hangingBush_FacingLeft_BottomPart_Rule.AddColorRule(wallIndicationColor, 1, 0, true);
		
		levelOne.SetRuleForTile(3, 4, hangingBush_FacingLeft_TopPart_Rule);
		levelOne.SetRuleForTile(4, 4, hangingBush_FacingLeft_BottomPart_Rule);
		
		UTextureMapRuleBook hangingShortBush_FacingRight_Rule{};
		hangingShortBush_FacingRight_Rule.SetCenterColor(vegetationColor);
		hangingShortBush_FacingRight_Rule.AddColorRule(vegetationColor, 1, 0, false);
		hangingShortBush_FacingRight_Rule.AddColorRule(wallIndicationColor, -1, 0, true);
		
		UTextureMapRuleBook hangingShortBush_FacingLeft_Rule{};
		hangingShortBush_FacingLeft_Rule.SetCenterColor(vegetationColor);
		hangingShortBush_FacingLeft_Rule.AddColorRule(vegetationColor, -1, 0, false);
		hangingShortBush_FacingLeft_Rule.AddColorRule(wallIndicationColor, 1, 0, true);
		
		levelOne.SetRuleForTile(8, 7, hangingShortBush_FacingRight_Rule);
		levelOne.SetRuleForTile(8, 8, hangingShortBush_FacingLeft_Rule);
		
		levelOne.SetBorderTile(8,9, vegetationColor, false, false,  false, false);

		/*

			SET MAP

		*/

		levelOne.SetScale(Point2f{ scale.X, scale.Y });
		levelOne.SetMap(UTextureData(UTexture(STextureRegistry::GetTexture(STextureNames::MainLevel))));

		DTextureTransform textureTransform{};
		FMatrix4x4f matrix{};
		FLocalTransform2D localTransform{};
		FWorldTransform2D worldTransform{};
		URealTransform2D realTransform{};
		FStaticColliderFlag staticColliderFlag{};
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		m_Registry.CreateEntity(levelEntity, levelOne, sceneObject, textureTransform, matrix, localTransform, realTransform, worldTransform);

		// --- WATER ---

		UTextureMap waterOne(USpriteSheet(UTexture(STextureRegistry::GetTexture(STextureNames::Tiles)), 10, 14, 0, 0));
		waterOne.SetScale(Point2f{ scale.X, scale.Y });
		waterOne.SetBorderTile(6, 13, waterColor, false, false, false, false);
		waterOne.SetMap(UTextureData(UTexture(STextureRegistry::GetTexture(STextureNames::Water))));

		UTextureMap waterTwo(USpriteSheet(UTexture(STextureRegistry::GetTexture(STextureNames::Tiles)), 10, 14, 0, 0));
		waterTwo.SetScale(Point2f{ scale.X, scale.Y });
		waterTwo.SetBorderTile(7, 13, waterColor, false, false, false, false);
		waterTwo.SetMap(UTextureData(UTexture(STextureRegistry::GetTexture(STextureNames::Water))));

		UTextureMap waterThree(USpriteSheet(UTexture(STextureRegistry::GetTexture(STextureNames::Tiles)), 10, 14, 0, 0));
		waterThree.SetScale(Point2f{ scale.X, scale.Y });
		waterThree.SetBorderTile(8, 13, waterColor, false, false, false, false);
		waterThree.SetMap(UTextureData(UTexture(STextureRegistry::GetTexture(STextureNames::Water))));


		UAnimatedTextureMap animatedTextureMap{3};
		animatedTextureMap.AddTextureMap(waterOne);
		animatedTextureMap.AddTextureMap(waterTwo);
		animatedTextureMap.AddTextureMap(waterThree);

		FEntity animatedWaterEntity{};
		m_Registry.CreateEntity(animatedWaterEntity,
			animatedTextureMap, 
			sceneObject, 
			textureTransform, 
			matrix, 
			localTransform,
			realTransform, 
			worldTransform);


		//CREATE COLLISIONS

		FEntityResolveOverlapInfo entityResolveOverlapInfo{};

		ECollisionBodyType collisionBodyType{ ECollisionBodyType::Solid };


		std::vector<Rectf> levelOneCollisions{ SGeometry::Simplify(levelOne.GetRawMapBoundsData(groundColor)) };

		for (size_t i = 0; i < levelOneCollisions.size(); i++)
		{
			FRectCollider currentRect{ 
				levelOneCollisions.at(i).left ,
				levelOneCollisions.at(i).bottom + levelOneCollisions.at(i).height ,
				levelOneCollisions.at(i).width , 
				levelOneCollisions.at(i).height, true};
			
			m_Registry.CreateEntity(
				levelEntity, 
				currentRect, 
				staticColliderFlag,
				realTransform,
				matrix, 
				localTransform, 

				sceneObject,

				entityResolveOverlapInfo,
				collisionBodyType,
				worldTransform);
		}

		//MOVABLE PLATFORMS

		std::vector<Rectf> currentRelevantBlocks{};

		const FPoint2f currentBlockSize{
			levelOne.GetSpriteSheet().GetScaledCellWidth(),
			levelOne.GetSpriteSheet().GetScaledCellHeight() };
		FPoint2f currentPosition{};
		FPoint2f currentSize{};

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(backgroundMountainTracksColor));

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom;

			currentSize.X = currentRelevantBlocks.at(i).width;
			currentSize.Y = currentRelevantBlocks.at(i).height;

			FPoint2f rightMostPosition{};
			rightMostPosition.X = currentSize.X < currentSize.Y ? currentPosition.X : currentPosition.X + currentSize.X;
			rightMostPosition.Y = currentSize.X > currentSize.Y ? currentPosition.Y : currentPosition.Y + currentSize.Y;

			CreateMovingPlatform(
				currentPosition,
				FPoint2f{ currentBlockSize.X * 2,  currentBlockSize.Y },
				currentPosition,
				rightMostPosition);
		}

		//SET DETAILS MAP

		levelOne.SetMap(UTextureData(UTexture("Levels/Level_PlainsOfPassage_Details.png")));
		m_Registry.CreateEntity(levelEntity, levelOne, sceneObject, textureTransform, matrix, localTransform, realTransform);//Checate si neceas esto


		//PLAYER SPAWN POINTS

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(spawnPointsColor));
		std::vector<FPoint2f> playerSpawnPoints{};
		playerSpawnPoints.reserve(currentRelevantBlocks.size());

		FPoint2f currentLowestX{INFINITY, INFINITY}; 
		FPoint2f currentHighestX{ -INFINITY, -INFINITY };

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			if (currentRelevantBlocks.at(i).left < currentLowestX.X)
			{
				currentLowestX.X = currentRelevantBlocks.at(i).left;
				currentLowestX.Y = currentRelevantBlocks.at(i).bottom + currentRelevantBlocks.at(i).height;
			}
			if (currentRelevantBlocks.at(i).left > currentHighestX.X)
			{
				currentHighestX.X = currentRelevantBlocks.at(i).left;
				currentHighestX.Y = currentRelevantBlocks.at(i).bottom;
			}

			m_CreatePlayer.CreateSpawnPoint(FPoint2f{
					currentRelevantBlocks.at(i).left,
					currentRelevantBlocks.at(i).bottom + currentRelevantBlocks.at(i).height },
					FPoint2f{ 
					currentRelevantBlocks.at(i).width,
					currentRelevantBlocks.at(i).height });
			
		}

		CreateLevelChanger(currentHighestX,
			FPoint2f{
					currentRelevantBlocks.at(0).width,
					currentRelevantBlocks.at(0).height });

		// --- PLAYER ---
	
		CreatePlayer(currentLowestX);

		// --- TREASURES ---

		currentRelevantBlocks = levelOne.GetRawMapBoundsData(treasureColor);
		
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left + (currentRelevantBlocks.at(i).width/2.f);
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + (currentRelevantBlocks.at(i).height/2.f);
		
			m_CreateTreasures.CreateRandomTreasure(m_CurrentSceneIndex, currentPosition);
		}

		//DESTROYABLE BLOCKS

		currentRelevantBlocks = levelOne.GetRawMapBoundsData(destroyableTileColor_Big_LeftBottomCorner);
		
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom;
		
			currentSize.X = currentRelevantBlocks.at(i).width * 2.f;
			currentSize.Y = currentRelevantBlocks.at(i).height * 2.f;
		
			CreateDestroyableBlock(currentPosition, currentSize);
		}
		
		currentRelevantBlocks = levelOne.GetRawMapBoundsData(destroyableTileColor_Small);
		
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom;
		
			currentSize.X = currentRelevantBlocks.at(i).width;
			currentSize.Y = currentRelevantBlocks.at(i).height;
		
			CreateDestroyableBlock(currentPosition, currentSize);
		}

		//STAIRS

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(stairsColor)) ;

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom;

			currentSize.X = currentRelevantBlocks.at(i).width;
			currentSize.Y = currentRelevantBlocks.at(i).height - (currentBlockSize.Y/2.f);

			CreateStairs(currentPosition, currentSize);
		}

		//DAMAGING BLOCKS 

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(nonSpikesDamagingColor));

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left + (currentRelevantBlocks.at(i).width/2.f);
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + (currentRelevantBlocks.at(i).height/2.f);

			currentSize.X = currentRelevantBlocks.at(i).width;
			currentSize.Y = currentRelevantBlocks.at(i).height;

			CreateDamagingBlock(currentPosition, currentSize);
		}

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(spikesColor));

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left + (currentRelevantBlocks.at(i).width / 2.f);
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + (currentRelevantBlocks.at(i).height / 2.f);

			currentSize.X = currentRelevantBlocks.at(i).width;
			currentSize.Y = currentRelevantBlocks.at(i).height;

			CreateDamagingBlock(currentPosition, currentSize);
		}


		//ENEMIES

		const FColor4i beetleColor{0,27,255,255};
		const FColor4i blorbColor{ 0 , 255, 28, 255};
		const FColor4i diveDrakeColor{251, 153, 235, 255};


		FColor4i treeBlockColor{ 29, 143, 42, 255 };

		levelOne.SetMap(UTextureData(UTexture("Levels/Level_PlainsOfPassage_Enemies.png")));

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(blorbColor));
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
		
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + currentRelevantBlocks.at(i).height;
		
			CreateBlorb(currentPosition, FPoint2f{ currentPosition.X + currentRelevantBlocks.at(i).width, currentPosition.Y }, m_Player);
		}
		
		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(beetleColor));
		
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + currentRelevantBlocks.at(i).height;
		
			CreateBeetle(currentPosition, FPoint2f{ currentPosition.X + currentRelevantBlocks.at(i).width, currentPosition.Y},m_Player);
		}

		currentRelevantBlocks = SGeometry::Simplify(levelOne.GetRawMapBoundsData(diveDrakeColor));
		
		FPoint2f otherPosition{};

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom + currentRelevantBlocks.at(i).height;
		
			otherPosition.X =
				currentRelevantBlocks.at(i).height < currentRelevantBlocks.at(i).width ?
				currentPosition.X + currentRelevantBlocks.at(i).width : currentPosition.X;


			otherPosition.Y =
				currentRelevantBlocks.at(i).height > currentRelevantBlocks.at(i).width ?
				currentRelevantBlocks.at(i).bottom : currentPosition.Y;

			CreateDrake(currentPosition, otherPosition, m_Player);
		}

		FPoint2f treeScale{ 3,3 };
		
		currentRelevantBlocks = levelOne.GetRawMapBoundsData(treeBlockColor);

		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			currentPosition.X = currentRelevantBlocks.at(i).left;
			currentPosition.Y = currentRelevantBlocks.at(i).bottom;
		
			CreateTree(currentPosition, treeScale);
		}

		FColor4i optimizationBlockColor{255, 255, 0, 255};
		
		currentRelevantBlocks = levelOne.ExtractRectsOfColor(optimizationBlockColor);
		
		for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		{
			CreateOptimizationBox(currentRelevantBlocks.at(i));
		}



		//currentRelevantBlocks = levelOne.GetMapBoundsData(dozeDrakeColor);
		//
		//for (size_t i = 0; i < currentRelevantBlocks.size(); i++)
		//{
		//	currentPosition.X = currentRelevantBlocks.at(i).left;
		//	currentPosition.Y = currentRelevantBlocks.at(i).bottom;
		//
		//	CreateDozeDrake(currentPosition, FPoint2f{ currentPosition.X + currentRelevantBlocks.at(i).width, currentPosition.Y }, );
		//}
		
	}

	void UCreateEntities::CreateCamera(const UEntityHandle& player, float colliderHeight, float colliderWidth)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};

		/*

			CAMERA

		*/

		// COMPONENTS


		SetOfComponentBlocks.emplace(SComponentNames::Camera);
		SetOfComponentBlocks.emplace(SComponentNames::Homing);
		SetOfComponentBlocks.emplace(SComponentNames::Physics);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::SceneObject);

		params.EntityTypeName = "Camera";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer cameraEntityHandle{};
		m_EntityCreator.CreateEntities(cameraEntityHandle, params);

		size_t currentIndex{ cameraEntityHandle.NumOfEntities() - 1};

		//PARAMS
		/*

		- Set Camera Params
		- Set Homing

		*/

		FCamera camera{};
		camera.ApertureAngle = 90;
		camera.ScreenHeight = g_WINDOW.Height;
		camera.ScreenWidth = g_WINDOW.Width;
		camera.MaxAutomaticDepth = 3;
		camera.MinAutomaticDepth = 1;
		m_Registry.SetComponent<FCamera>(camera, cameraEntityHandle.EntityAt(currentIndex).GetConstEntity());

		FSpeedAtDistance speedAtDistance{};
		FHomingInformation cameraHomingInfo{};
		cameraHomingInfo.ApplyOnX = true;
		cameraHomingInfo.ApplyOnY = true;
		cameraHomingInfo.IsActive = true;
		cameraHomingInfo.MovementType = EMovementType::OverrideVelocity;
		cameraHomingInfo.MovementIsRelativeToDistance = true;

		speedAtDistance.MaxDistanceThreshold = 100;
		speedAtDistance.Speed = 5;
		cameraHomingInfo.MovementMagnitudePerDistance.Add(speedAtDistance);

		FTarget cameraTarget{};
		cameraTarget.CurrentTargetInfo = FTargetInformation{};
		cameraTarget.CurrentTarget = player;
		cameraTarget.CurrentPlayerTargetInfo = FTargetInformation{};
		cameraTarget.PlayerTarget = player;

		m_Registry.SetComponent<FHomingInformation>(cameraHomingInfo, cameraEntityHandle.EntityAt(currentIndex).GetConstEntity());
		m_Registry.SetComponent<FTarget>(cameraTarget, cameraEntityHandle.EntityAt(currentIndex).GetConstEntity());

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		m_Registry.SetComponent<FSceneObject>(sceneObject, cameraEntityHandle.EntityAt(currentIndex).GetConstEntity());

	}

	void UCreateEntities::CreateDestroyableBlock(const FPoint2f& bottomLeftPosition, const FPoint2f& size)
	{

		//----SET PARAMS----
		/*
		- Set Animation Graph
		- Set State Machine
		- Set Position
		- Set Shape (TriggerBox)
		- Set Gravity
		- Set physics collision

		*/



		//---- SET TRANSFORM ----

		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};
		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = bottomLeftPosition;

		/*
			SCENE OBJECT

		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		//---- SET COLLISION ----

		FRectCollider collider{ 0,size.Y, size.X , size.Y , true };
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};

		ECollisionBodyType collisionType{};
		collisionType = ECollisionBodyType::Solid;

		FStaticColliderFlag staticColliderflag{};

		//---- SET HEALTH
		FHealth health{ 1,0.5f };

		//--- SET TRANSFORM FOR TEXTURE

		DTextureTransform textureTransform{};

		//--- ENTITY 
		UEntityHandle blockEntity{};

		m_Registry.CreateEntity(blockEntity,
			health,

			collider,
			collisionType,
			staticColliderflag,
			entityResolveOverlapInfo,

			realTransform,
			localTransform,
			matrix,

			sceneObject,
			
			textureTransform);


		// ----- SET TEXTURE -----


		UTexture& dirtBlockTexture{ STextureRegistry::GetTexture(STextureNames::DirtBlock) };
		Point2f scaleInScreen{};
		scaleInScreen.y = size.Y / 31;
		scaleInScreen.x = size.X / 31;

		Point2f defaultTextureLocation{ -31 / 2.f, 31 / 2.f };


		//---- CREATE ANIMATION MONTAGE SETUP----

		FAnimationMontage idle_AnimationMontage{};
		idle_AnimationMontage.Animation.SetFramesPerSecond(1);
		idle_AnimationMontage.Animation.AddFrame(UTextureRef(dirtBlockTexture, Rectf(2, 33, 31, 31), Point2f{ 0,0 }, scaleInScreen), 1);

		FAnimationMontage death_AnimationMontage{};

		death_AnimationMontage.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		death_AnimationMontage.LoopNumber = 1;
		death_AnimationMontage.StateRemovalType = EStateRemovalType::AllStateMachines;
		death_AnimationMontage.ResetUponFinishing = true;

		scaleInScreen.x = size.Y / 41;
		scaleInScreen.y = size.X / 37;
		dirtBlockTexture.SetScreenPosition(Point2f{ 0,0 });
		dirtBlockTexture.SetScale(scaleInScreen);//Point2f{ -41 / 2.f, 37 / 2.f }

		death_AnimationMontage.Animation.SetFramesPerSecond(6);
		death_AnimationMontage.Animation.CanLoop(false);
		death_AnimationMontage.Animation.AddFrames(dirtBlockTexture, Rectf{ 2, 72, 44, 37 }, 3, 2);

		//---- SPRITE SETUP----

		DSprite sprite{ m_Registry, blockEntity };
		sprite.AddState(SActionNames::Idle, idle_AnimationMontage);
		sprite.AddState(SActionNames::Die, death_AnimationMontage);

		sprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);
		sprite.LinkWithAll(SActionNames::Idle, ELinkingAction::BothWays);

		/*

		----SET ACTIVATION STATE MACHINE----

		*/

		FActivationState despawn_ActivationState{};
		despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		despawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		FActivationState respawn_ActivationState{};
		respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation stateController_Activation{ m_Registry, blockEntity };
		stateController_Activation.AddState(SActionNames::Despawn, despawn_ActivationState);
		stateController_Activation.AddState(SActionNames::Spawn, respawn_ActivationState);

		/*

			STATE MACHINE

		*/


		UStateMachine stateMachine{ m_Registry, blockEntity };
		stateMachine.AddStateController<DSprite>(sprite);
		stateMachine.AddStateController<DStateController_Activation>(stateController_Activation);

		// LOGIC 

		//stateMachine.AddReactingEvent(SEventNames::OnZeroHealth, EEventStateToTrigger::Execution, SActionNames::Despawn, EEventStateToTrigger::Execution);
		stateMachine.AddReactingState(SEventNames::OnZeroHealth, EEventStateToTrigger::ConditionalExecution, SActionNames::Die, EEventStateToTrigger::ConditionalExecution);
		stateMachine.AddReactingState(SActionNames::Die, EEventStateToTrigger::ConditionalRemoval, SActionNames::Despawn, EEventStateToTrigger::ConditionalExecution);

		// ---- FINISH STATE MACHINE SETUP

		stateMachine.TriggerState(SActionNames::Idle, EEventStateToTrigger::ConditionalExecution);

		m_Registry.AddComponent(blockEntity, stateMachine);

		//CreateRandomTreasures(blockEntityHandles.EntityAt(currentSize), 2, 3); //THIS IS BROKEN, IDK WHY
	}

	void UCreateEntities::CreateStairs(const FPoint2f& bottomLeftPosition, const FPoint2f& size)
	{
		FEntity stairsEntity{};

		// ---- COLLISIONS

		FEntityResolveOverlapInfo entityResolveOverlapInfo{};

		FRectCollider currentRect{0,size.Y, size.X, size.Y };

		FStaticColliderFlag staticColliderFlag{};

		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapAll };

		// ---- TRANSFORMS
		
		FMatrix4x4f matrix{};
		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = bottomLeftPosition;
		URealTransform2D realTransform{};

		/*
			SCENE OBJECT

		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;



		//---- SET TAG
		FTag tag{};
		tag.Name = "Stairs";

		m_Registry.CreateEntity(
			stairsEntity,
			tag,
			realTransform,
			matrix,
			localTransform,

			sceneObject,

			entityResolveOverlapInfo,
			currentRect,
			staticColliderFlag,
			collisionBodyType);
	}


	void UCreateEntities::CreateDamagingBlock(const FPoint2f& position, const FPoint2f& size)
	{
		FEntity damagingBlockEntity{};

		//----Attack----

		FAttack obstacleAttack{};
		obstacleAttack.Damage = 10;

		//---- SET TRANSFORM ----

		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};
		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = position;

		/*
			SCENE OBJECT

		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;


		//---- SET COLLIDER ----

		FRectCollider collider{ -size.X / 2.f, size.Y / 2.f, size.X, size.Y};
		FStaticColliderFlag staticColliderFlag{};
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionType{ ECollisionBodyType::Solid };

		m_Registry.CreateEntity(damagingBlockEntity, 
			obstacleAttack,
			
			matrix,
			realTransform,
			localTransform,

			sceneObject,

			collider,
			staticColliderFlag,
			entityResolveOverlapInfo,
			collisionType);
	}

	void UCreateEntities::CreateMovingPlatform(const FPoint2f& position, const FPoint2f& size, const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition)
	{

		// --- ENTITY

		UEntityHandle platformEntity{};

		//---- SET SCENE OBJECT

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		//---- SET TRANSFORM ----

		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};
		FLocalTransform2D localTransform{};

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = position;
		FSceneNode sceneNode{};


		//---- SET COLLISION 

		FRectCollider collider{ 0, size.Y, size.X, size.Y };
		ECollisionBodyType collisionType{ ECollisionBodyType::Solid };
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		FStaticColliderFlag staticColliderFlag{};

		// --- PHYSICS PARAMS

		FStickyInformation physicsParamsToCopy{};
		physicsParamsToCopy.MultipleOnVelocity = 1;
		FSticky sticky{};
		sticky.Tag_To_StickyInformation.Push("Player", physicsParamsToCopy);

		FVelocity velocity{};
		FNetForce2D netForce{};

		// --- SPRITE TRANSFORM

		DTextureTransform textureTransform{};

		// ---- SET TAG

		FTag tag{};
		tag.Name = "MovingPlatform";

		// --- TARGET

		UEntityHandle patrolPoint_0{ CreateCheckPoint(leftMostPosition) };
		UEntityHandle patrolPoint_1{ CreateCheckPoint(rightMostPosition) };

		FTarget target{};
		target.CurrentTarget = patrolPoint_1;
		target.CurrentTargetInfo.TriggerDistance = FPoint2f{ 20,100 };
		target.PlayerTarget = patrolPoint_1; //doesn't matter
		target.CurrentPlayerTargetInfo.TriggerDistance = FPoint2f{ 200,200 };

		FHomingInformation homingInfo{};

		//---

		m_Registry.CreateEntity(platformEntity,
			collider,
			collisionType,
			entityResolveOverlapInfo,
			staticColliderFlag,
			
			matrix, 
			realTransform,
			localTransform,
			worldTransform,
			sceneNode,

			sceneObject,

			sticky,
			velocity,
			netForce,

			tag,

			target,
			homingInfo,

			textureTransform
			);



		// ----- SET TEXTURE -----


		UTexture& dirtBlockTexture{ STextureRegistry::GetTexture(STextureNames::DirtBlock) };
		Point2f scaleInScreen{};
		scaleInScreen.y = size.Y / 31;
		scaleInScreen.x = size.X / 31;

		Point2f defaultTextureLocation{ -31 / 2.f, 31 / 2.f };


		//---- CREATE ANIMATION MONTAGE SETUP----

		FAnimationMontage idle_AnimationMontage{};
		idle_AnimationMontage.Animation.SetFramesPerSecond(1);
		idle_AnimationMontage.Animation.AddFrame(UTextureRef(dirtBlockTexture, Rectf(2, 33, 31, 31), Point2f{ 0,0 }, scaleInScreen), 1);


		//---- SPRITE SETUP----

		DSprite sprite{ m_Registry, platformEntity };
		sprite.AddState(SActionNames::FollowTarget, idle_AnimationMontage);

		/*
			---- TARGET STATE CONTROLLER ----
		*/
		FTargetInformation targetInfo{};
		targetInfo.TriggerDistance = FPoint2f{ 20,20 };

		FTargetsOfInterest patrol_Targets_Of_Interest{};
		patrol_Targets_Of_Interest.StateRemovalType = EStateRemovalType::AllStateMachines;
		patrol_Targets_Of_Interest.CurrentIndexIncreases = true;
		patrol_Targets_Of_Interest.RangeBehaviourOfTargetIndex = EValueBehavior::Loop;

		patrol_Targets_Of_Interest.Targets.Add(patrolPoint_0, targetInfo);
		patrol_Targets_Of_Interest.Targets.Add(patrolPoint_1, targetInfo);

		DStateController_TargetChooser stateController_TargetChooser{ m_Registry, platformEntity };
		stateController_TargetChooser.AddState(SActionNames::ChooseNextPatrolTarget, patrol_Targets_Of_Interest);

		/*
			---- HOMING INFORMATION CONTROLLER
		*/
		FHomingInfoState patrol_HomingInfo{ };
		patrol_HomingInfo.HomingInfo.MovementIsRelativeToDistance = false;
		patrol_HomingInfo.StateRemovalType = EStateRemovalType::Local;
		patrol_HomingInfo.HomingInfo.ApplyOnX = true;
		patrol_HomingInfo.HomingInfo.ApplyOnY = true;
		patrol_HomingInfo.HomingInfo.IsActive = true;
		patrol_HomingInfo.HomingInfo.MovementType = EMovementType::OverrideVelocity;
		FSpeedAtDistance patrolSpeed{};
		patrolSpeed.MaxDistanceThreshold = 1000;// this doesn't really matter
		patrolSpeed.Speed = 100;
		patrol_HomingInfo.HomingInfo.MovementMagnitudePerDistance.Add(patrolSpeed);

		FHomingInfoState hit_HomingInfo{};
		hit_HomingInfo.HomingInfo.IsActive = false;
		hit_HomingInfo.StateRemovalType = EStateRemovalType::Local;

		DStateController_Homing stateController_Homing{ m_Registry, platformEntity };
		stateController_Homing.AddState(SActionNames::FollowTarget, patrol_HomingInfo);

		/*

			STATE MACHINE

		*/

		UStateMachine stateMachine{ m_Registry, platformEntity };
		stateMachine.AddStateController<DSprite>(sprite);
		stateMachine.AddStateController<DStateController_Homing>(stateController_Homing);
		stateMachine.AddStateController<DStateController_TargetChooser>(stateController_TargetChooser);


		// LOGIC 

		// ---- FINISH STATE MACHINE SETUP

		FEventTriggerCondition nextTarget_ExecutionCondition{};
		nextTarget_ExecutionCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextTarget_ExecutionCondition.EventName_To_ActiveState.Push(SEventNames::OnTargetInRange, EEventState::Active);
		stateMachine.AddConditionsToExecute(SActionNames::ChooseNextPatrolTarget, nextTarget_ExecutionCondition);

		FEventTriggerCondition nextTarget_RemovalCondition{};
		nextTarget_RemovalCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextTarget_RemovalCondition.EventName_To_ActiveState.Push(SEventNames::OnTargetInRange, EEventState::Inactive);
		stateMachine.AddConditionsToRemove(SActionNames::ChooseNextPatrolTarget, nextTarget_RemovalCondition);

		stateMachine.TriggerState(SActionNames::FollowTarget, EEventStateToTrigger::ConditionalExecution);

		// ---- SET STATE MACHINE ----

		m_Registry.AddComponent(platformEntity, stateMachine);
	}

	void UCreateEntities::CreateOrigin()
	{
		if (!m_Origin.IsValid())
		{
			m_Origin = CreateCheckPoint(FPoint2f{ 0,0 });
		}
	}

	UEntityHandle UCreateEntities::CreateCheckPoint(const FPoint2f& position)
	{
		UEntityHandle entity{};

		//---- SET TRANSFORM ----

		FMatrix4x4f matrix{};
		URealTransform2D realTransform{};
		FLocalTransform2D localTransform{};

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = position;
		FSceneNode sceneNode{};

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		m_Registry.CreateEntity(entity, 
			matrix,
			realTransform,
			localTransform,
			worldTransform,
			sceneNode,

			sceneObject);

		return entity;
	}

	void UCreateEntities::CreateOptimizationBox(const Rectf& bounds)
	{
		FRectCollider rectCollider{ bounds.left,bounds.bottom + bounds.height, bounds.width, bounds.height, false};
		FLocalTransform2D localTransform{};
		FWorldTransform2D worldTransform{};
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};
		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;

		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapAll};
		FOptimizationGridCellState optimizationGridCellState{ };

		FEntity entity{};
		m_Registry.CreateEntity(entity, 
			rectCollider,
			localTransform,
			realTransform,
			matrix,
			worldTransform,

			sceneObject,

			entityResolveOverlapInfo,
			collisionBodyType,
			optimizationGridCellState);
	}

	void UCreateEntities::CreatePlayer(const FPoint2f& position)
	{
		if (!m_Player.IsValid())
		{
			m_Player = m_CreatePlayer.CreatePlayer(position, 400, 2000, 700, m_CurrentSceneIndex);
		}
	}

	void UCreateEntities::CreateTestPlatform(const FPoint2f& topLeftPosition, const FPoint2f& scale)
	{
		FEntity entityTestPlatform{};

		FRectCollider rectCollider{};
		rectCollider.OriginalRect.Width = 1;
		rectCollider.OriginalRect.Height = 1;
		rectCollider.CurrentRect.Width = rectCollider.OriginalRect.Width;
		rectCollider.CurrentRect.Height = rectCollider.OriginalRect.Height;
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{ ECollisionBodyType::Solid};
		FStaticColliderFlag staticColliderFlag{};

		FLocalTransform2D localTransform{};
		localTransform.Transform.Position = topLeftPosition;
		localTransform.Transform.Scale = scale;
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};

		m_Registry.CreateEntity(entityTestPlatform, staticColliderFlag, localTransform, rectCollider, entityResolveOverlapInfo, collisionBodyType, realTransform, matrix);
	}

	void UCreateEntities::CreateBeetle(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};
		/*

			// --------------- BEETLE -------------------

		*/

		// COMPONENTS
		SetOfComponentBlocks.emplace(SComponentNames::LookAt);
		SetOfComponentBlocks.emplace(SComponentNames::Homing);
		SetOfComponentBlocks.emplace(SComponentNames::Health);
		SetOfComponentBlocks.emplace(SComponentNames::Attack);
		SetOfComponentBlocks.emplace(SComponentNames::StateMachine);
		SetOfComponentBlocks.emplace(SComponentNames::SpriteTransform);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::SceneObject);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::RectCollider);
		SetOfComponentBlocks.emplace(SComponentNames::Physics);

		params.EntityTypeName = "Beetle";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer beetlesHandleContainer{};
		m_EntityCreator.CreateEntities(beetlesHandleContainer, params);
		const size_t currentIndex{ beetlesHandleContainer.NumOfEntities() - 1};

		/*
		SETTINGS
		- Set State Machine
			- Set Animations
			- Set Movement State Machine
			- Set Activation State Machine
		- Set Target
		- Set Health
		- Set Attack
		- Set Shape
		- Set Z Depth Value
		- Set Transform
		- Set Collision Type
		- Set Gravity

		*/

		/*
		
			SCENE OBJECT
		
		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		m_Registry.SetComponent<FSceneObject>(sceneObject, beetlesHandleContainer.EntityAt(currentIndex).GetConstEntity());


		/*

			SET SENSES

		*/

		FTarget target{};
		target.PlayerTarget = playerHandle;
		target.CurrentPlayerTargetInfo.TriggerDistance = FPoint2f{ 200,200 };
		m_Registry.SetComponent<FTarget>(target, beetlesHandleContainer.EntityAt(currentIndex));

		FHomingInformation homingInfo{};
		homingInfo.ApplyOnX = true;
		homingInfo.ApplyOnY = false;
		homingInfo.IsActive = true;
		homingInfo.MovementIsRelativeToDistance = false;
		homingInfo.MovementType = EMovementType::OverrideVelocity;
		FSpeedAtDistance speedAtDistance{};
		speedAtDistance.MaxDistanceThreshold = 1000;//doesn't matter
		speedAtDistance.Speed = 100;
		homingInfo.MovementMagnitudePerDistance.Add(speedAtDistance);
		m_Registry.SetComponent<FHomingInformation>(homingInfo, beetlesHandleContainer.EntityAt(currentIndex));

		/*
			----SPRITE----
		*/

		UTexture& beetleSpriteSheet{ STextureRegistry::GetTexture(STextureNames::Beetle) };
		const Point2f beetleDefaultSize{ 25, 15 };
		const Point2f beetleDefaultTextureLocation{ -beetleDefaultSize.x * 1.5f, -beetleDefaultSize.y * 1.5f };
		const Point2f beetleDefaultScale{ 3, 3 };

		UTexture& beetleDeathEffectSpriteSheet{ STextureRegistry::GetTexture(STextureNames::ShovelKnight) };

		FAnimationMontage beetle_Walk{};
		beetle_Walk.Animation.SetFramesPerSecond(3);
		beetle_Walk.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(2, 17, beetleDefaultSize.x, beetleDefaultSize.y), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Walk.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(30, 17, beetleDefaultSize.x, beetleDefaultSize.y), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Walk.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(58, 17, beetleDefaultSize.x, beetleDefaultSize.y), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Walk.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(86, 17, beetleDefaultSize.x, beetleDefaultSize.y), beetleDefaultTextureLocation, beetleDefaultScale), 1);

		FAnimationMontage beetle_Die{};
		beetle_Die.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		beetle_Die.LoopNumber = 1;
		beetle_Die.StateRemovalType = EStateRemovalType::AllStateMachines;
		beetle_Die.ResetUponFinishing = true;
		beetle_Die.Animation.CanLoop(false);
		beetle_Die.Animation.SetFramesPerSecond(12);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(2, 36, 23, 16), beetleDefaultTextureLocation, beetleDefaultScale), 6);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleSpriteSheet, Rectf(28, 36, 23, 16), beetleDefaultTextureLocation, beetleDefaultScale), 6);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(136, 1102, 23, 15), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(162, 1102, 23, 15), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(188, 1102, 23, 15), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(214, 1102, 23, 15), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(240, 1102, 23, 15), beetleDefaultTextureLocation, beetleDefaultScale), 1);
		beetle_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(136, 1102, 2, 2), beetleDefaultTextureLocation, beetleDefaultScale), 1);

		FAnimationMontage beetle_Despawn{};
		beetle_Despawn.StateRemovalType = EStateRemovalType::Local;
		beetle_Despawn.ResetUponFinishing = true;
		beetle_Die.Animation.CanLoop(false);
		beetle_Despawn.Animation.SetFramesPerSecond(1);
		beetle_Despawn.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(136, 1102, 2, 2), beetleDefaultTextureLocation, beetleDefaultScale), 1);


		DSprite beetle_Sprite{ m_Registry,  beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_Sprite.AddState(SActionNames::FollowTarget, beetle_Walk);
		beetle_Sprite.AddState(SActionNames::Die, beetle_Die);
		beetle_Sprite.AddState(SActionNames::Despawn, beetle_Despawn);

		beetle_Sprite.LinkWithAll(SActionNames::FollowTarget, ELinkingAction::BothWays);
		beetle_Sprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);
		beetle_Sprite.LinkWithAll(SActionNames::Despawn, ELinkingAction::BothWays);

		/*
			----SOUND STATE CONTROLLER----
		*/

		FSoundEffectState die_SoundEffect{};
		die_SoundEffect.Name = "Sound/BeetleFlipSound.mp3";
		die_SoundEffect.ResetAfterPlay = true;
		die_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		die_SoundEffect.LoopNumber = 0;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		stateController_SoundEffects.AddState(SActionNames::Die, die_SoundEffect);

		/*
			----MOVEMENT STATE CONTROLLER----
		*/

		FVectorMovement beetleDie_VectorMovement{};
		beetleDie_VectorMovement.ApplyMovementOnXAxis = true;
		beetleDie_VectorMovement.MovementType = EMovementType::OverrideVelocity;
		beetleDie_VectorMovement.TerminateEachTick = true;

		DStateController_VectorMovement beetle_stateController_VectorMovement{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };

		beetle_stateController_VectorMovement.AddState(SActionNames::Die, beetleDie_VectorMovement);

		/*
			----MOVEMENT OVER TIME STATE CONTROLLER----
		*/

		FVectorMovementOverTime receiveDamage{};
		receiveDamage.MovementDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.EntityToGetMovementDirectionInfoFrom = beetlesHandleContainer.EntityAt(currentIndex);
		receiveDamage.GetXAxisFromMovementDirection = true;
		receiveDamage.GetYAxisFromMovementDirection = false;

		receiveDamage.BottomMagnitude = 800;
		receiveDamage.NormalMagnitude = 1000;
		receiveDamage.TopMagnitude = 1800;
		receiveDamage.TimeOfMovement = 0.2;
		receiveDamage.MovementType = EMovementType::OverrideForce;
		receiveDamage.ApplyMovementOnXAxis = true;
		receiveDamage.ApplyMovementOnYAxis = true;
		receiveDamage.MovementTerminationType = EMovementTerminationType::TerminateAfterTimeTranspired;
		receiveDamage.ResetMovementCounterOnTermination = true;
		receiveDamage.AddCustomToMovement = true;
		receiveDamage.MovementDirection = FPoint2f{0,2000};

		receiveDamage.OverrideMovementOnStart = true;
		receiveDamage.MovementTypeForStartOverride = EMovementType::OverrideVelocity;
		receiveDamage.MovementValueForOverrideOnStart = FPoint2f{ 0,0 };
		receiveDamage.ApplyStartMovementOnXAxis = true;
		receiveDamage.ApplyStartMovementOnYAxis = true;

		FCurvePoint beginCurvePoint{};
		beginCurvePoint.Point = { 0, 10 };
		beginCurvePoint.RightTangentPoint.AngleInDegrees = 0;// 120;
		beginCurvePoint.RightTangentPoint.Length = 2;

		FCurvePoint endCurvePoint{};
		endCurvePoint.Point = { 1,0 };
		endCurvePoint.LeftTangentPoint.AngleInDegrees = 60;
		endCurvePoint.LeftTangentPoint.Length = 2;
		receiveDamage.ChangeInMagnitude.Curve.push_back(beginCurvePoint);
		receiveDamage.ChangeInMagnitude.Curve.push_back(endCurvePoint);
		receiveDamage.ChangeInMagnitude.CurveBehavior = EValueBehavior::Clamp;

		DStateController_VectorMovementOverTime beetle_stateController_VectorMovementOverTime{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_stateController_VectorMovementOverTime.AddState(SActionNames::Wounded, receiveDamage);

		/*
			---- ROTATION STATE CONTROLLER ----
		*/
		FLookAtRotationInfoState patrolLookAt{};
		patrolLookAt.LookAtRotationInfo.ApplyOnY = true;
		patrolLookAt.LookAtRotationInfo.ApplyOnZ = false;
		patrolLookAt.LookAtRotationInfo.IsActive = true;
		patrolLookAt.StateRemovalType = EStateRemovalType::Local;

		DStateController_LookAt beetle_StateController_LookAt{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_LookAt.AddState(SActionNames::FollowTarget, patrolLookAt);

		/*
			---- ACTIVATION STATE CONTROLLER ----
		*/

		FActivationState beetle_Despawn_ActivationState{};
		beetle_Despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		beetle_Despawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		FActivationState beetle_Respawn_ActivationState{};
		beetle_Respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		beetle_Respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation beetle_StateController_Activation{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_Activation.AddState(SActionNames::Despawn, beetle_Despawn_ActivationState);
		beetle_StateController_Activation.AddState(SActionNames::Spawn, beetle_Respawn_ActivationState);

		/*
			---- TARGET STATE CONTROLLER ----
		*/
		FTargetInformation beetle_TargetInfo{};
		beetle_TargetInfo.TriggerDistance = FPoint2f{ 50,100 };

		FTargetsOfInterest beetle_Patrol_Targets_Of_Interest{};
		beetle_Patrol_Targets_Of_Interest.StateRemovalType = EStateRemovalType::AllStateMachines;
		beetle_Patrol_Targets_Of_Interest.RangeBehaviourOfTargetIndex = EValueBehavior::Loop;

		UEntityHandle patrolPoint_0{ CreateCheckPoint(rightMostPosition)  };
		UEntityHandle patrolPoint_1{ CreateCheckPoint(leftMostPosition) };
		beetle_Patrol_Targets_Of_Interest.Targets.Add(patrolPoint_0, beetle_TargetInfo);
		beetle_Patrol_Targets_Of_Interest.Targets.Add(patrolPoint_1, beetle_TargetInfo);

		DStateController_TargetChooser beetle_StateController_TargetChooser{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_TargetChooser.AddState(SActionNames::ChooseNextPatrolTarget, beetle_Patrol_Targets_Of_Interest);

		/*
			---- HOMING INFORMATION CONTROLLER
		*/
		FHomingInfoState patrol_HomingInfo{ };
		patrol_HomingInfo.HomingInfo.MovementIsRelativeToDistance = false;
		patrol_HomingInfo.StateRemovalType = EStateRemovalType::Local;
		patrol_HomingInfo.HomingInfo.ApplyOnX = true;
		patrol_HomingInfo.HomingInfo.ApplyOnY = false;
		patrol_HomingInfo.HomingInfo.IsActive = true;
		patrol_HomingInfo.HomingInfo.MovementType = EMovementType::OverrideVelocity;
		FSpeedAtDistance patrolSpeed{};
		patrolSpeed.MaxDistanceThreshold = 1000;// this doesn't really matter
		patrolSpeed.Speed = 100;
		patrol_HomingInfo.HomingInfo.MovementMagnitudePerDistance.Add(patrolSpeed);

		FHomingInfoState hit_HomingInfo{};
		hit_HomingInfo.HomingInfo.IsActive = false;
		hit_HomingInfo.StateRemovalType = EStateRemovalType::Local;

		DStateController_Homing beetle_StateController_Homing{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_Homing.AddState(SActionNames::FollowTarget, patrol_HomingInfo);
		beetle_StateController_Homing.AddState(SActionNames::Wounded, hit_HomingInfo);
		beetle_StateController_Homing.AddState(SActionNames::Die, hit_HomingInfo);

		/*

			COLLISION BODY TYPE CONTROLLER

		*/

		FCollisionBodyTypeState death_CollisionState{};
		death_CollisionState.CollisionBodyType = ECollisionBodyType::IgnoreOnlyMovable;
		FCollisionBodyTypeState life_CollisionState{};
		life_CollisionState.CollisionBodyType = ECollisionBodyType::OverlapOnlyMovable;

		DStateController_CollisionBodyType stateController_CollisionBodyType{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		stateController_CollisionBodyType.AddState(SActionNames::Revive, life_CollisionState);
		stateController_CollisionBodyType.AddState(SActionNames::Die, death_CollisionState);

		/*
			----STATE MACHINE----
		*/

		UStateMachine beetleStateBoundEventsSystem{ m_Registry, beetlesHandleContainer.EntityAt(currentIndex) };
		beetleStateBoundEventsSystem.AddStateController(beetle_Sprite);
		beetleStateBoundEventsSystem.AddStateController(beetle_stateController_VectorMovementOverTime);
		beetleStateBoundEventsSystem.AddStateController(beetle_StateController_Activation);
		beetleStateBoundEventsSystem.AddStateController(beetle_StateController_LookAt);
		beetleStateBoundEventsSystem.AddStateController(beetle_StateController_TargetChooser);
		beetleStateBoundEventsSystem.AddStateController(beetle_StateController_Homing);
		beetleStateBoundEventsSystem.AddStateController(beetle_stateController_VectorMovement);
		beetleStateBoundEventsSystem.AddStateController(stateController_CollisionBodyType);
		beetleStateBoundEventsSystem.AddStateController(stateController_SoundEffects);
		/*
			---- STATE MACHINE LOGIC ----
		*/


		FEventTriggerCondition die_Execution_Condition_1{};
		die_Execution_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		beetleStateBoundEventsSystem.AddConditionsToExecute(SActionNames::Die, die_Execution_Condition_1);

		FEventTriggerCondition despawnCondition{};
		despawnCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		despawnCondition.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		beetleStateBoundEventsSystem.AddConditionsToExecute(SActionNames::Despawn, despawnCondition);

		FEventTriggerCondition nextPatrolTargetCondition{};
		nextPatrolTargetCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextPatrolTargetCondition.EventName_To_ActiveState.Push(SEventNames::OnTargetInRange, EEventState::Active);
		beetleStateBoundEventsSystem.AddConditionsToExecute(SActionNames::ChooseNextPatrolTarget, nextPatrolTargetCondition);

		FEventTriggerCondition patrolCondition{};
		patrolCondition.EventTriggerConditionType = EEventTriggerConditionType::GateAndTrigger;
		patrolCondition.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth,	EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SActionNames::Die,			EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SActionNames::Despawn,		EEventState::Inactive);
		beetleStateBoundEventsSystem.AddConditionsToExecute(SActionNames::FollowTarget, patrolCondition);

		FEventTriggerCondition patrol_RemovalCondition_1{};
		patrol_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		beetleStateBoundEventsSystem.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_1);

		FEventTriggerCondition patrol_RemovalCondition_2{};
		patrol_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		beetleStateBoundEventsSystem.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_2);

		// ---- WOUNDED ----

		FEventTriggerCondition wounded_ExecutionCondition_1{};
		wounded_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		beetleStateBoundEventsSystem.AddConditionsToExecute(SActionNames::Wounded, wounded_ExecutionCondition_1);

		FEventTriggerCondition wounded_RemovalCondition_1{};
		wounded_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		beetleStateBoundEventsSystem.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_1);

		//FEventTriggerCondition wounded_RemovalCondition_2{};
		//wounded_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		//beetleStateBoundEventsSystem.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_2);

		// ---- SET PATROL AS DEFAULT ----

		beetleStateBoundEventsSystem.TriggerState(SActionNames::ChooseNextPatrolTarget, EEventStateToTrigger::ConditionalExecution);
		beetleStateBoundEventsSystem.TriggerState(SActionNames::FollowTarget, EEventStateToTrigger::ConditionalExecution);

		m_Registry.SetComponent<UStateMachine>(beetleStateBoundEventsSystem, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			HEALTH

		*/

		FHealth beetleHealth{ 1, 0.6f };
		m_Registry.SetComponent<FHealth>(beetleHealth, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			ATTACK

		*/

		FAttack beetle_Attack{ };
		beetle_Attack.Damage = 1;
		m_Registry.SetComponent<FAttack>(beetle_Attack, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			SHAPE

		*/

		FRectCollider rectCollider{};
		rectCollider.OriginalRect.Width = beetleDefaultSize.x * beetleDefaultScale.x;
		rectCollider.OriginalRect.Height = beetleDefaultSize.y * beetleDefaultScale.y;
		rectCollider.OriginalRect.Top = (rectCollider.OriginalRect.Height)/2.f;
		rectCollider.OriginalRect.Left = -(rectCollider.OriginalRect.Width)/2.f ;
		
		m_Registry.SetComponent<FRectCollider>(rectCollider, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			TRANSFORM

		*/

		FWorldTransform2D beetle_WorldTransform{};
		beetle_WorldTransform.Transform.Position = leftMostPosition;
		m_Registry.SetComponent<FWorldTransform2D>(beetle_WorldTransform, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			COLLISION TYPE

		*/

		ECollisionBodyType beetleCollisionBodyType{ ECollisionBodyType::OverlapOnlyMovable };
		m_Registry.SetComponent<ECollisionBodyType>(beetleCollisionBodyType, beetlesHandleContainer.EntityAt(currentIndex));

		/*

			GRAVITY

		*/

		FEnvironmentForces beetleForces{};
		FMovement stylizedGravity{};
		stylizedGravity.Movement = FPoint2f(0, -700);
		stylizedGravity.MovementType = EMovementType::AddForce;
		beetleForces.ForcesToApplyPerTick.Add(stylizedGravity);
		m_Registry.SetComponent<FEnvironmentForces>(beetleForces, beetlesHandleContainer.EntityAt(currentIndex));

		m_CreateTreasures.CreateRandomDropableTreasures(beetlesHandleContainer.EntityAt(currentIndex), 0, 4, m_CurrentSceneIndex);
	}

	void UCreateEntities::CreateDrake(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};
		/*

			// --------------- DRAKE -------------------

		*/

		// COMPONENTS
		SetOfComponentBlocks.emplace(SComponentNames::LookAt);
		SetOfComponentBlocks.emplace(SComponentNames::Homing);
		SetOfComponentBlocks.emplace(SComponentNames::Health);
		SetOfComponentBlocks.emplace(SComponentNames::Attack);
		SetOfComponentBlocks.emplace(SComponentNames::StateMachine);
		SetOfComponentBlocks.emplace(SComponentNames::SpriteTransform);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::SceneObject);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::RectCollider);
		SetOfComponentBlocks.emplace(SComponentNames::Physics);

		params.EntityTypeName = "DiveDrake";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer drakeHandleContainer{};
		m_EntityCreator.CreateEntities(drakeHandleContainer, params);
		const size_t currentIndex{ drakeHandleContainer.NumOfEntities() - 1 };

		/*
		SETTINGS
		- Set State Machine
			- Set Animations
			- Set Movement State Machine
			- Set Activation State Machine
		- Set Target
		- Set Health
		- Set Attack
		- Set Shape
		- Set Z Depth Value
		- Set Transform
		- Set Collision Type
		- Set Gravity

		*/

		/*

			SCENE OBJECT

		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		m_Registry.SetComponent<FSceneObject>(sceneObject, drakeHandleContainer.EntityAt(currentIndex).GetConstEntity());


		/*

			SET SENSES

		*/

		FTarget target{};
		target.PlayerTarget = playerHandle;
		target.CurrentPlayerTargetInfo.TriggerDistance = FPoint2f{ 200,200 };
		
		m_Registry.SetComponent<FTarget>(target, drakeHandleContainer.EntityAt(currentIndex));

		FHomingInformation homingInfo{};
		homingInfo.ApplyOnX = true;
		homingInfo.ApplyOnY = true;
		homingInfo.IsActive = true;
		homingInfo.MovementIsRelativeToDistance = false;
		homingInfo.MovementType = EMovementType::OverrideVelocity;
		FSpeedAtDistance speedAtDistance{};
		speedAtDistance.MaxDistanceThreshold = 1000;//doesn't matter
		speedAtDistance.Speed = 100;
		homingInfo.MovementMagnitudePerDistance.Add(speedAtDistance);
		m_Registry.SetComponent<FHomingInformation>(homingInfo, drakeHandleContainer.EntityAt(currentIndex));

		/*

		HEALTH

	*/

		FHealth health{ 3, 0.6f };
		m_Registry.SetComponent<FHealth>(health, drakeHandleContainer.EntityAt(currentIndex));

		/*

			ATTACK

		*/

		FAttack attack{ };
		attack.Damage = 1;
		m_Registry.SetComponent<FAttack>(attack, drakeHandleContainer.EntityAt(currentIndex));

		/*

			SHAPE

		*/

		USpriteSheet drakeSpriteSheet{ STextureRegistry::GetTexture(STextureNames::DiveDrake), 1, 3, 0, 0 };
		
		const Point2f drakeDefaultSize{ drakeSpriteSheet.GetScaledCellWidth(), drakeSpriteSheet.GetScaledCellWidth() };
		const Point2f drakeDefaultTextureLocation{ -drakeDefaultSize.x / 2.f,0 };
		const Point2f defaultScale{ 3, 3 };
		const Point2f defaultExplosionScale{ 1.5, 1.5 };

		drakeSpriteSheet.SetPositionInScreen(drakeDefaultTextureLocation);

		FRectCollider rectCollider{ -drakeDefaultSize.x / 2.f, drakeDefaultSize.y , drakeDefaultSize.x , drakeDefaultSize.y};

		m_Registry.SetComponent<FRectCollider>(rectCollider, drakeHandleContainer.EntityAt(currentIndex));

		/*

			TRANSFORM

		*/

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = leftMostPosition;
		worldTransform.Transform.Scale = FPoint2f{ defaultScale.x, defaultScale.y };
		m_Registry.SetComponent<FWorldTransform2D>(worldTransform, drakeHandleContainer.EntityAt(currentIndex));

		/*

			COLLISION TYPE

		*/

		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapOnlyMovable };
		m_Registry.SetComponent<ECollisionBodyType>(collisionBodyType, drakeHandleContainer.EntityAt(currentIndex));

		/*
			----SPRITE----
		*/

		UTexture& deathEffectSpriteSheet{ STextureRegistry::GetTexture(STextureNames::ShovelKnight) };
		deathEffectSpriteSheet.SetScreenPosition(Point2f{ 0,0 });

		FAnimationMontage anim_Fly{};
		anim_Fly.Animation.SetFramesPerSecond(6);
		anim_Fly.Animation.AddFrame(drakeSpriteSheet.GetFrameCopy(0, 0), 1);
		anim_Fly.Animation.AddFrame(drakeSpriteSheet.GetFrameCopy(0, 1), 1);
		anim_Fly.Animation.AddFrame(drakeSpriteSheet.GetFrameCopy(0, 2), 1);

		FAnimationMontage anim_Die{};
		anim_Die.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		anim_Die.LoopNumber = 1;
		anim_Die.StateRemovalType = EStateRemovalType::AllStateMachines;
		anim_Die.ResetUponFinishing = true;
		anim_Die.Animation.CanLoop(false);
		anim_Die.Animation.SetFramesPerSecond(12);
		anim_Die.Animation.AddFrame(drakeSpriteSheet.GetFrameCopy(0, 2), 3);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(136, 1102, 23, 15), drakeDefaultTextureLocation, defaultExplosionScale), 1);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(162, 1102, 23, 15), drakeDefaultTextureLocation, defaultExplosionScale), 1);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(188, 1102, 23, 15), drakeDefaultTextureLocation, defaultExplosionScale), 1);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(214, 1102, 23, 15), drakeDefaultTextureLocation, defaultExplosionScale), 1);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(240, 1102, 23, 15), drakeDefaultTextureLocation, defaultExplosionScale), 1);
		anim_Die.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(136, 1102, 2, 2), drakeDefaultTextureLocation, defaultExplosionScale), 1);

		FAnimationMontage anim_Despawn{};
		anim_Despawn.StateRemovalType = EStateRemovalType::Local;
		anim_Despawn.ResetUponFinishing = true;
		anim_Die.Animation.CanLoop(false);
		anim_Despawn.Animation.SetFramesPerSecond(1);
		anim_Despawn.Animation.AddFrame(UTextureRef(deathEffectSpriteSheet, Rectf(136, 1102, 2, 2), drakeDefaultTextureLocation, defaultScale), 1);


		DSprite beetle_Sprite{ m_Registry,  drakeHandleContainer.EntityAt(currentIndex) };
		beetle_Sprite.AddState(SActionNames::FollowTarget, anim_Fly);
		beetle_Sprite.AddState(SActionNames::Die, anim_Die);
		beetle_Sprite.AddState(SActionNames::Despawn, anim_Despawn);

		beetle_Sprite.LinkWithAll(SActionNames::FollowTarget, ELinkingAction::BothWays);
		beetle_Sprite.LinkWithAll(SActionNames::Die, ELinkingAction::BothWays);
		beetle_Sprite.LinkWithAll(SActionNames::Despawn, ELinkingAction::BothWays);

		/*
			----SOUND STATE CONTROLLER----
		*/

		FSoundEffectState die_SoundEffect{};
		die_SoundEffect.Name = "Sound/BeetleFlipSound.mp3";
		die_SoundEffect.ResetAfterPlay = true;
		die_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		die_SoundEffect.LoopNumber = 0;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_SoundEffects.AddState(SActionNames::Die, die_SoundEffect);

		/*
			----MOVEMENT STATE CONTROLLER----
		*/

		FVectorMovement drakeDie_VectorMovement{};
		drakeDie_VectorMovement.ApplyMovementOnXAxis = true;
		drakeDie_VectorMovement.MovementType = EMovementType::OverrideVelocity;
		drakeDie_VectorMovement.TerminateEachTick = true;

		DStateController_VectorMovement stateController_VectorMovement{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };

		stateController_VectorMovement.AddState(SActionNames::Die, drakeDie_VectorMovement);

		/*
			----MOVEMENT OVER TIME STATE CONTROLLER----
		*/

		FVectorMovementOverTime receiveDamage{};
		receiveDamage.MovementDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.EntityToGetMovementDirectionInfoFrom = drakeHandleContainer.EntityAt(currentIndex);
		receiveDamage.AddCustomToMovement = true;

		receiveDamage.MovementDirection = FPoint2f{ 0,1600 };

		receiveDamage.BottomMagnitude = 800;
		receiveDamage.NormalMagnitude = 1400;
		receiveDamage.TopMagnitude = 1400;
		receiveDamage.TimeOfMovement = 0.2;
		receiveDamage.MovementType = EMovementType::AddForce;
		receiveDamage.ApplyMovementOnXAxis = true;
		receiveDamage.ApplyMovementOnYAxis = true;
		receiveDamage.MovementTerminationType = EMovementTerminationType::TerminateAfterTimeTranspired;
		receiveDamage.ResetMovementCounterOnTermination = true;

		receiveDamage.OverrideMovementOnStart = true;
		receiveDamage.MovementTypeForStartOverride = EMovementType::OverrideVelocity;
		receiveDamage.ApplyStartMovementOnXAxis = true;
		receiveDamage.ApplyStartMovementOnYAxis = true;

		receiveDamage.MovementOnStartDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.StartMovementMultiple = 10.f;
		receiveDamage.AddCustomToStartMovement = true;
		receiveDamage.MovementValueForOverrideOnStart = FPoint2f{ 0,2 };
		receiveDamage.EntityToGetStartMovementDirectionInfoFrom = drakeHandleContainer.EntityAt(currentIndex);

		FCurvePoint beginCurvePoint{};
		beginCurvePoint.Point = { 0, 10 };
		beginCurvePoint.RightTangentPoint.AngleInDegrees = 0;// 120;
		beginCurvePoint.RightTangentPoint.Length = 2;

		FCurvePoint endCurvePoint{};
		endCurvePoint.Point = { 1,0 };
		endCurvePoint.LeftTangentPoint.AngleInDegrees = 60;
		endCurvePoint.LeftTangentPoint.Length = 2;
		receiveDamage.ChangeInMagnitude.Curve.push_back(beginCurvePoint);
		receiveDamage.ChangeInMagnitude.Curve.push_back(endCurvePoint);
		receiveDamage.ChangeInMagnitude.CurveBehavior = EValueBehavior::Clamp;

		DStateController_VectorMovementOverTime stateController_VectorMovementOverTime{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_VectorMovementOverTime.AddState(SActionNames::Wounded, receiveDamage);

		/*
			---- ROTATION STATE CONTROLLER ----
		*/
		FLookAtRotationInfoState patrolLookAt{};
		patrolLookAt.LookAtRotationInfo.ApplyOnY = true;
		patrolLookAt.LookAtRotationInfo.ApplyOnZ = false;
		patrolLookAt.LookAtRotationInfo.IsActive = true;
		patrolLookAt.StateRemovalType = EStateRemovalType::Local;

		DStateController_LookAt stateController_LookAt{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_LookAt.AddState(SActionNames::FollowTarget, patrolLookAt);

		/*
			---- ACTIVATION STATE CONTROLLER ----
		*/

		FActivationState despawn_ActivationState{};
		despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		despawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		FActivationState respawn_ActivationState{};
		respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation stateController_Activation{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_Activation.AddState(SActionNames::Despawn, despawn_ActivationState);
		stateController_Activation.AddState(SActionNames::Spawn, respawn_ActivationState);

		/*
			---- TARGET STATE CONTROLLER ----
		*/
		FTargetInformation targetInfo{};
		targetInfo.TriggerDistance = FPoint2f{ 50,100 };

		FTargetsOfInterest patrol_Targets_Of_Interest{};
		patrol_Targets_Of_Interest.StateRemovalType = EStateRemovalType::AllStateMachines;
		patrol_Targets_Of_Interest.RangeBehaviourOfTargetIndex = EValueBehavior::Loop;

		UEntityHandle patrolPoint_0{ CreateCheckPoint(rightMostPosition) };
		UEntityHandle patrolPoint_1{ CreateCheckPoint(leftMostPosition) };
		patrol_Targets_Of_Interest.Targets.Add(patrolPoint_0, targetInfo);
		patrol_Targets_Of_Interest.Targets.Add(patrolPoint_1, targetInfo);

		DStateController_TargetChooser stateController_TargetChooser{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_TargetChooser.AddState(SActionNames::ChooseNextPatrolTarget, patrol_Targets_Of_Interest);

		/*
			---- HOMING INFORMATION CONTROLLER
		*/
		FHomingInfoState patrol_HomingInfo{ };
		patrol_HomingInfo.HomingInfo.MovementIsRelativeToDistance = false;
		patrol_HomingInfo.StateRemovalType = EStateRemovalType::Local;
		patrol_HomingInfo.HomingInfo.ApplyOnX = true;
		patrol_HomingInfo.HomingInfo.ApplyOnY = true;
		patrol_HomingInfo.HomingInfo.IsActive = true;
		patrol_HomingInfo.HomingInfo.MovementType = EMovementType::OverrideVelocity;
		FSpeedAtDistance patrolSpeed{};
		patrolSpeed.MaxDistanceThreshold = 1000;// this doesn't really matter
		patrolSpeed.Speed = 100;
		patrol_HomingInfo.HomingInfo.MovementMagnitudePerDistance.Add(patrolSpeed);

		FHomingInfoState hit_HomingInfo{};
		hit_HomingInfo.HomingInfo.IsActive = false;
		hit_HomingInfo.StateRemovalType = EStateRemovalType::Local;

		DStateController_Homing stateController_Homing{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_Homing.AddState(SActionNames::FollowTarget, patrol_HomingInfo);
		stateController_Homing.AddState(SActionNames::Wounded, hit_HomingInfo);
		stateController_Homing.AddState(SActionNames::Die, hit_HomingInfo);

		/*

			COLLISION BODY TYPE CONTROLLER

		*/

		FCollisionBodyTypeState death_CollisionState{};
		death_CollisionState.CollisionBodyType = ECollisionBodyType::IgnoreOnlyMovable;
		FCollisionBodyTypeState life_CollisionState{};
		life_CollisionState.CollisionBodyType = ECollisionBodyType::OverlapOnlyMovable;

		DStateController_CollisionBodyType stateController_CollisionBodyType{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateController_CollisionBodyType.AddState(SActionNames::Revive, life_CollisionState);
		stateController_CollisionBodyType.AddState(SActionNames::Die, death_CollisionState);

		/*
			----STATE MACHINE----
		*/

		UStateMachine stateMachine{ m_Registry, drakeHandleContainer.EntityAt(currentIndex) };
		stateMachine.AddStateController(beetle_Sprite);
		stateMachine.AddStateController(stateController_VectorMovementOverTime);
		stateMachine.AddStateController(stateController_Activation);
		stateMachine.AddStateController(stateController_LookAt);
		stateMachine.AddStateController(stateController_TargetChooser);
		stateMachine.AddStateController(stateController_Homing);
		stateMachine.AddStateController(stateController_VectorMovement);
		stateMachine.AddStateController(stateController_CollisionBodyType);
		stateMachine.AddStateController(stateController_SoundEffects);
		/*
			---- STATE MACHINE LOGIC ----
		*/


		FEventTriggerCondition die_Execution_Condition_1{};
		die_Execution_Condition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		stateMachine.AddConditionsToExecute(SActionNames::Die, die_Execution_Condition_1);

		FEventTriggerCondition despawnCondition{};
		despawnCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		despawnCondition.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		stateMachine.AddConditionsToExecute(SActionNames::Despawn, despawnCondition);

		FEventTriggerCondition nextPatrolTargetCondition{};
		nextPatrolTargetCondition.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextPatrolTargetCondition.EventName_To_ActiveState.Push(SEventNames::OnTargetInRange, EEventState::Active);
		stateMachine.AddConditionsToExecute(SActionNames::ChooseNextPatrolTarget, nextPatrolTargetCondition);

		FEventTriggerCondition patrolCondition{};
		patrolCondition.EventTriggerConditionType = EEventTriggerConditionType::GateAndTrigger;
		patrolCondition.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		patrolCondition.EventName_To_ActiveState.Push(SActionNames::Despawn, EEventState::Inactive);
		stateMachine.AddConditionsToExecute(SActionNames::FollowTarget, patrolCondition);

		FEventTriggerCondition patrol_RemovalCondition_1{};
		patrol_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		stateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_1);

		FEventTriggerCondition patrol_RemovalCondition_2{};
		patrol_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		stateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_2);

		// ---- WOUNDED ----

		FEventTriggerCondition wounded_ExecutionCondition_1{};
		wounded_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		stateMachine.AddConditionsToExecute(SActionNames::Wounded, wounded_ExecutionCondition_1);

		FEventTriggerCondition wounded_RemovalCondition_1{};
		wounded_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		stateMachine.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_1);

		FEventTriggerCondition wounded_RemovalCondition_2{};
		wounded_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		stateMachine.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_2);

		// ---- SET PATROL AS DEFAULT ----

		stateMachine.TriggerState(SActionNames::ChooseNextPatrolTarget, EEventStateToTrigger::ConditionalExecution);
		stateMachine.TriggerState(SActionNames::FollowTarget, EEventStateToTrigger::ConditionalExecution);

		m_Registry.SetComponent<UStateMachine>(stateMachine, drakeHandleContainer.EntityAt(currentIndex));

		m_CreateTreasures.CreateRandomDropableTreasures(drakeHandleContainer.EntityAt(currentIndex), 0, 4, m_CurrentSceneIndex);
	}

	void UCreateEntities::CreateBlorb(const FPoint2f& leftMostPosition, const FPoint2f& rightMostPosition, const UEntityHandle& playerHandle)
	{
		std::unordered_set<std::string> SetOfComponentBlocks{};
		FParam_EntityCreation params{};
		/*

			// --------------- BEETLE -------------------

		*/

		// COMPONENTS
		SetOfComponentBlocks.emplace(SComponentNames::Homing);
		SetOfComponentBlocks.emplace(SComponentNames::Health);
		SetOfComponentBlocks.emplace(SComponentNames::Attack);
		SetOfComponentBlocks.emplace(SComponentNames::StateMachine);
		SetOfComponentBlocks.emplace(SComponentNames::SpriteTransform);
		SetOfComponentBlocks.emplace(SComponentNames::SceneNode);
		SetOfComponentBlocks.emplace(SComponentNames::SceneObject);
		SetOfComponentBlocks.emplace(SComponentNames::Transformable);
		SetOfComponentBlocks.emplace(SComponentNames::RectCollider);
		SetOfComponentBlocks.emplace(SComponentNames::Physics);

		params.EntityTypeName = "Blorb";
		params.NumberOfEntitiesToCreate = 1;
		m_ArchetypeCreator.CreateArchetype(params.EntityTypeName, SetOfComponentBlocks);
		SetOfComponentBlocks.clear();

		UEntityHandleContainer blorbsHandleContainer{};
		m_EntityCreator.CreateEntities(blorbsHandleContainer, params);

		size_t currentIndex{ blorbsHandleContainer.NumOfEntities() - 1};

		/*
		SETTINGS
		- Set State Machine
			- Set Animations
			- Set Movement State Machine
			- Set Activation State Machine
		- Set Target
		- Set Health
		- Set Attack
		- Set Shape
		- Set Z Depth Value
		- Set Transform
		- Set Collision Type
		- Set Gravity

		*/

		/*

			TEXTURE PARAMS

		*/

		FEntity blorbTextureEntity{};
		UTexture& blorbSpriteSheet{ STextureRegistry::GetTexture(STextureNames::Blorb) };
		const Point2f blorbDefaultSize{ 19, 11 };
		const Point2f blorbDefaultScale{ 3, 3 };
		const Point2f blorbDefaultTextureLocation{0, 0 };

		/*

			SET SENSES

		*/

		FTarget target{};
		target.PlayerTarget = playerHandle;
		target.CurrentPlayerTargetInfo.TriggerDistance = FPoint2f{ 200,200 };
		m_Registry.SetComponent<FTarget>(target, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			HEALTH

		*/

		FHealth blorbHealth{ 3, 0.6f };
		m_Registry.SetComponent<FHealth>(blorbHealth, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			ATTACK

		*/

		FAttack attack{ };
		attack.Damage = 1;
		m_Registry.SetComponent<FAttack>(attack, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			RECT COLLIDER

		*/

		FRectCollider rectCollider{ };
		rectCollider.OriginalRect.Left = 0;
		rectCollider.OriginalRect.Top = blorbDefaultSize.y * blorbDefaultScale.y;
		rectCollider.OriginalRect.Width = blorbDefaultSize.x * blorbDefaultScale.x;
		rectCollider.OriginalRect.Height = blorbDefaultSize.y * blorbDefaultScale.y;
		m_Registry.SetComponent<FRectCollider>(rectCollider, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			TRANSFORM

		*/

		FWorldTransform2D worldTransform{};
		worldTransform.Transform.Position = leftMostPosition;
		m_Registry.SetComponent<FWorldTransform2D>(worldTransform, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			COLLISION TYPE

		*/

		ECollisionBodyType collisionBodyType{ ECollisionBodyType::OverlapOnlyMovable };
		m_Registry.SetComponent<ECollisionBodyType>(collisionBodyType, blorbsHandleContainer.EntityAt(currentIndex));

		/*

			GRAVITY

		*/

		FEnvironmentForces blorbForces{};
		FMovement stylizedGravity{};
		stylizedGravity.Movement = FPoint2f(0, -700);
		stylizedGravity.MovementType = EMovementType::AddForce;
		blorbForces.ForcesToApplyPerTick.Add(stylizedGravity);
		m_Registry.SetComponent<FEnvironmentForces>(blorbForces, blorbsHandleContainer.EntityAt(currentIndex));

		/*
		
			SCENE OBJECT
		
		*/

		FSceneObject sceneObject{};
		sceneObject.SceneIndex = m_CurrentSceneIndex;
		m_Registry.SetComponent<FSceneObject>(sceneObject, blorbsHandleContainer.EntityAt(currentIndex).GetConstEntity());

		/*
			----SPRITE----


		*/

		FAnimationMontage blorb_blue_exist{};
		blorb_blue_exist.LoopNumber = 1;
		blorb_blue_exist.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		blorb_blue_exist.StateRemovalType = EStateRemovalType::AllStateMachines;
		blorb_blue_exist.Animation.SetFramesPerSecond(12);
		blorb_blue_exist.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(2, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_exist.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(24, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_exist.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(46, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_exist.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(68, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);

		FAnimationMontage blorb_blue_jump{};
		blorb_blue_jump.Animation.CanLoop(false);
		blorb_blue_jump.Animation.SetFramesPerSecond(8);
		blorb_blue_jump.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(2, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_jump.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(24, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_jump.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(46, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_blue_jump.Animation.AddFrame(UTextureRef(blorbSpriteSheet, Rectf(68, 13, 19, 11), blorbDefaultTextureLocation, blorbDefaultScale), 1);

		UTexture& beetleDeathEffectSpriteSheet{ STextureRegistry::GetTexture(STextureNames::ShovelKnight) };

		FAnimationMontage blorb_Die{};
		blorb_Die.AnimationDeactivationProcedure = EAnimationDeactivationProcedure::ForceDeactivationAfterLoopNumber;
		blorb_Die.LoopNumber = 1;
		blorb_Die.StateRemovalType = EStateRemovalType::AllStateMachines;
		blorb_Die.ResetUponFinishing = true;
		blorb_Die.Animation.CanLoop(false);
		blorb_Die.Animation.SetFramesPerSecond(12);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(136, 1102, 23, 15), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(162, 1102, 23, 15), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(188, 1102, 23, 15), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(214, 1102, 23, 15), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(240, 1102, 23, 15), blorbDefaultTextureLocation, blorbDefaultScale), 1);
		blorb_Die.Animation.AddFrame(UTextureRef(beetleDeathEffectSpriteSheet, Rectf(136, 1102, 2, 2),	 blorbDefaultTextureLocation, blorbDefaultScale), 1);

		DSprite blorb_Blue_Sprite{ m_Registry, blorbsHandleContainer.ConstEntityAt(currentIndex) };
		blorb_Blue_Sprite.AddState(SActionNames::Idle, blorb_blue_exist);
		blorb_Blue_Sprite.AddState(SActionNames::Jump, blorb_blue_jump);
		blorb_Blue_Sprite.AddState(SActionNames::Die, blorb_Die);

		blorb_Blue_Sprite.LinkWithAll(SActionNames::Idle, ELinkingAction::BothWays);
		blorb_Blue_Sprite.LinkWithAll(SActionNames::Jump, ELinkingAction::BothWays);
		blorb_Blue_Sprite.LinkWithAll(SActionNames::Die,  ELinkingAction::BothWays);

		/*
		
			SOUND

		*/

		FSoundEffectState wounded_SoundEffect{};
		wounded_SoundEffect.Name = "Sound/BlobHitSound.mp3";
		wounded_SoundEffect.ResetAfterPlay = true;
		wounded_SoundEffect.StateRemovalType = EStateRemovalType::Local;
		wounded_SoundEffect.LoopNumber = 0;

		DStateController_SoundEffects stateController_SoundEffects{ m_Registry, blorbsHandleContainer.ConstEntityAt(currentIndex) };
		stateController_SoundEffects.AddState(SActionNames::Wounded, wounded_SoundEffect);

		/*

			MOVEMENT

		*/

		FVectorMovement blorbJumpMovement{};
		blorbJumpMovement.ApplyMovementOnYAxis = true;
		blorbJumpMovement.ApplyMovementOnXAxis = false;
		blorbJumpMovement.Movement = FPoint2f{ 0, 200 };
		blorbJumpMovement.MovementType = EMovementType::OverrideVelocity;
		blorbJumpMovement.TerminateEachTick = true;

		FVectorMovement blorbIdleMovement{};
		blorbIdleMovement.ApplyMovementOnYAxis = false;
		blorbIdleMovement.ApplyMovementOnXAxis = true;
		blorbIdleMovement.Movement = FPoint2f{ 0, 0 };
		blorbIdleMovement.MovementType = EMovementType::OverrideVelocity;
		blorbIdleMovement.TerminateEachTick = true;

		DStateController_VectorMovement blorb_stateController_VectorMovement{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		blorb_stateController_VectorMovement.AddState(SActionNames::Jump, blorbJumpMovement);
		blorb_stateController_VectorMovement.AddState(SActionNames::Idle, blorbIdleMovement);

		/*
			----MOVEMENT OVER TIME STATE MACHINE----
		*/

		FVectorMovementOverTime receiveDamage{};
		receiveDamage.MovementDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.EntityToGetMovementDirectionInfoFrom = blorbsHandleContainer.EntityAt(currentIndex);
		receiveDamage.AddCustomToMovement = true;

		receiveDamage.MovementDirection = FPoint2f{ 0,1600 };

		receiveDamage.BottomMagnitude = 800;
		receiveDamage.NormalMagnitude = 1400;
		receiveDamage.TopMagnitude = 1400;
		receiveDamage.TimeOfMovement = 0.2;
		receiveDamage.MovementType = EMovementType::AddForce;
		receiveDamage.ApplyMovementOnXAxis = true;
		receiveDamage.ApplyMovementOnYAxis = true;
		receiveDamage.MovementTerminationType = EMovementTerminationType::TerminateAfterTimeTranspired;
		receiveDamage.ResetMovementCounterOnTermination = true;

		receiveDamage.OverrideMovementOnStart = true;
		receiveDamage.MovementTypeForStartOverride = EMovementType::OverrideVelocity;
		receiveDamage.ApplyStartMovementOnXAxis = true;
		receiveDamage.ApplyStartMovementOnYAxis = true;

		receiveDamage.MovementOnStartDirectionType = EMovementDirectionType::FromAttackingEntityDirection;
		receiveDamage.StartMovementMultiple = 10.f;
		receiveDamage.AddCustomToStartMovement = true;
		receiveDamage.MovementValueForOverrideOnStart = FPoint2f{ 0,2 };
		receiveDamage.EntityToGetStartMovementDirectionInfoFrom = blorbsHandleContainer.EntityAt(currentIndex);

		FCurvePoint beginCurvePoint{};
		beginCurvePoint.Point = { 0, 10 };
		beginCurvePoint.RightTangentPoint.AngleInDegrees = 0;// 120;
		beginCurvePoint.RightTangentPoint.Length = 2;

		FCurvePoint endCurvePoint{};
		endCurvePoint.Point = { 1,0 };
		endCurvePoint.LeftTangentPoint.AngleInDegrees = 60;
		endCurvePoint.LeftTangentPoint.Length = 2;
		receiveDamage.ChangeInMagnitude.Curve.push_back(beginCurvePoint);
		receiveDamage.ChangeInMagnitude.Curve.push_back(endCurvePoint);
		receiveDamage.ChangeInMagnitude.CurveBehavior = EValueBehavior::Clamp;

		DStateController_VectorMovementOverTime blorb_stateController_VectorMovementOverTime{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		blorb_stateController_VectorMovementOverTime.AddState(SActionNames::Wounded, receiveDamage);

		/*
			---- ACTIVATION STATE CONTROLLER ----
		*/

		FActivationState blorb_Despawn_ActivationState{};
		blorb_Despawn_ActivationState.EntityActivationAction = EEntityActivationAction::Deactivate;
		blorb_Despawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		FActivationState blorb_Respawn_ActivationState{};
		blorb_Respawn_ActivationState.EntityActivationAction = EEntityActivationAction::Activate;
		blorb_Respawn_ActivationState.StateRemovalType = EStateRemovalType::Local;

		DStateController_Activation beetle_StateController_Activation{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_Activation.AddState(SActionNames::Despawn, blorb_Despawn_ActivationState);
		beetle_StateController_Activation.AddState(SActionNames::Spawn, blorb_Respawn_ActivationState);

		/*
			---- TARGET STATE CONTROLLER ----
		*/

		//---- PATROL ----

		FTargetInformation blorb_TargetInfo{};
		blorb_TargetInfo.TriggerDistance = FPoint2f{ 50,50 };

		FTargetsOfInterest blorb_Patrol_Targets_Of_Interest{};
		blorb_Patrol_Targets_Of_Interest.StateRemovalType = EStateRemovalType::AllStateMachines;
		blorb_Patrol_Targets_Of_Interest.RangeBehaviourOfTargetIndex = EValueBehavior::Loop;

		UEntityHandle patrolPoint_0{ CreateCheckPoint(leftMostPosition)  };
		UEntityHandle patrolPoint_1{ CreateCheckPoint(rightMostPosition) };
		blorb_Patrol_Targets_Of_Interest.Targets.Add(patrolPoint_0, blorb_TargetInfo);
		blorb_Patrol_Targets_Of_Interest.Targets.Add(patrolPoint_1, blorb_TargetInfo);

		//---- CHASE ----

		FTargetsOfInterest beetle_Chase_Player{};
		beetle_Chase_Player.StateRemovalType = EStateRemovalType::AllStateMachines;
		beetle_Chase_Player.RangeBehaviourOfTargetIndex = EValueBehavior::Loop;

		UEntityHandle handle{ playerHandle };
		beetle_Chase_Player.Targets.Add(handle, target.CurrentPlayerTargetInfo);

		DStateController_TargetChooser beetle_StateController_TargetChooser{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_TargetChooser.AddState(SActionNames::ChooseNextPatrolTarget, blorb_Patrol_Targets_Of_Interest);
		beetle_StateController_TargetChooser.AddState(SActionNames::ChoosePlayerTarget, beetle_Chase_Player);

		/*
			---- HOMING INFORMATION CONTROLLER
		*/
		FHomingInfoState patrol_HomingInfo{ };
		patrol_HomingInfo.StateRemovalType = EStateRemovalType::Local;
		patrol_HomingInfo.HomingInfo.ApplyOnX = true;
		patrol_HomingInfo.HomingInfo.ApplyOnY = false;
		patrol_HomingInfo.HomingInfo.IsActive = true;
		patrol_HomingInfo.HomingInfo.MovementType = EMovementType::OverrideVelocity;
		patrol_HomingInfo.HomingInfo.MovementIsRelativeToDistance = false;
		FSpeedAtDistance patrolSpeed{};
		patrolSpeed.MaxDistanceThreshold = 1000;
		patrolSpeed.Speed = 400;
		patrol_HomingInfo.HomingInfo.MovementMagnitudePerDistance.Add(patrolSpeed);

		FHomingInfoState hit_HomingInfo{};
		hit_HomingInfo.HomingInfo.IsActive = false;
		hit_HomingInfo.StateRemovalType = EStateRemovalType::Local;

		FHomingInfoState idle_HomingInfo{};
		idle_HomingInfo.HomingInfo.IsActive = false;
		idle_HomingInfo.StateRemovalType = EStateRemovalType::Local;

		DStateController_Homing beetle_StateController_Homing{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		beetle_StateController_Homing.AddState(SActionNames::FollowTarget, patrol_HomingInfo);
		beetle_StateController_Homing.AddState(SActionNames::Wounded, hit_HomingInfo);
		beetle_StateController_Homing.AddState(SActionNames::Idle, idle_HomingInfo);

		/*
		
			COLLISION BODY TYPE CONTROLLER

		*/

		FCollisionBodyTypeState death_CollisionState{};
		death_CollisionState.CollisionBodyType = ECollisionBodyType::IgnoreOnlyMovable;
		FCollisionBodyTypeState life_CollisionState{};
		life_CollisionState.CollisionBodyType = ECollisionBodyType::OverlapOnlyMovable;

		DStateController_CollisionBodyType stateController_CollisionBodyType{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		stateController_CollisionBodyType.AddState(SActionNames::Revive, life_CollisionState);
		stateController_CollisionBodyType.AddState(SActionNames::Die, death_CollisionState);

		/*
			----STATE MACHINE----
		*/

		UStateMachine blorb_StateMachine{ m_Registry, blorbsHandleContainer.EntityAt(currentIndex) };
		blorb_StateMachine.AddStateController(blorb_stateController_VectorMovement);
		blorb_StateMachine.AddStateController(blorb_Blue_Sprite);
		blorb_StateMachine.AddStateController(blorb_stateController_VectorMovementOverTime);
		blorb_StateMachine.AddStateController(beetle_StateController_Activation);
		blorb_StateMachine.AddStateController(beetle_StateController_TargetChooser);
		blorb_StateMachine.AddStateController(beetle_StateController_Homing);
		blorb_StateMachine.AddStateController(stateController_CollisionBodyType);
		blorb_StateMachine.AddStateController(stateController_SoundEffects);

		/*
			---- STATE MACHINE LOGIC ----
		*/

		// ---	DIE ---

		blorb_StateMachine.AddReactingState(
			SEventNames::OnZeroHealth, EEventStateToTrigger::ForcedExecution,
			SActionNames::Die, EEventStateToTrigger::ForcedExecution);

		// ---	DESPAWN ---

		blorb_StateMachine.AddReactingState(
			SActionNames::Die, EEventStateToTrigger::ConditionalRemoval,
			SActionNames::Despawn, EEventStateToTrigger::ForcedExecution);

		// ---	CHOOSE NEXT PATROL TARGET---

		blorb_StateMachine.AddReactingState(
			SEventNames::OnPlayerInRange, EEventStateToTrigger::ConditionalRemoval,
			SActionNames::ChooseNextPatrolTarget, EEventStateToTrigger::ConditionalExecution);

		FEventTriggerCondition nextPatrolTarget_ExecutionCondition_1{};
		nextPatrolTarget_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextPatrolTarget_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnPlayerInRange, EEventState::Inactive);
		nextPatrolTarget_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnTargetInRange, EEventState::Active);//I think when this goes off, the one above is called again
		blorb_StateMachine.AddConditionsToExecute(SActionNames::ChooseNextPatrolTarget, nextPatrolTarget_ExecutionCondition_1);

		FEventTriggerCondition nextPatrolTarget_ExecutionCondition_2{};
		nextPatrolTarget_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		nextPatrolTarget_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::ChooseNextPatrolTarget, nextPatrolTarget_ExecutionCondition_2);

		FEventTriggerCondition nextPatrolTarget_RemovalCondition_1{};
		nextPatrolTarget_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextPatrolTarget_RemovalCondition_1.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::ChooseNextPatrolTarget, nextPatrolTarget_RemovalCondition_1);

		FEventTriggerCondition nextPatrolTarget_RemovalCondition_2{};
		nextPatrolTarget_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		nextPatrolTarget_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnPlayerInRange, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::ChooseNextPatrolTarget, nextPatrolTarget_RemovalCondition_2);

		// --- CHOOSE PLAYER TARGET

		FEventTriggerCondition choosePlayerTarget_ExecutionCondition_1{};
		choosePlayerTarget_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		choosePlayerTarget_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnPlayerInRange, EEventState::Active);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::ChoosePlayerTarget, choosePlayerTarget_ExecutionCondition_1);

		FEventTriggerCondition choosePlayerTarget_ExecutionCondition_2{};
		choosePlayerTarget_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		choosePlayerTarget_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::ChoosePlayerTarget, choosePlayerTarget_ExecutionCondition_2);

		FEventTriggerCondition choosePlayerTarget_RemovalCondition_1{};
		choosePlayerTarget_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		choosePlayerTarget_RemovalCondition_1.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::ChoosePlayerTarget, choosePlayerTarget_RemovalCondition_1);

		FEventTriggerCondition choosePlayerTarget_RemovalCondition_2{};
		choosePlayerTarget_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		choosePlayerTarget_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnPlayerInRange, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::ChoosePlayerTarget, choosePlayerTarget_RemovalCondition_2);

		// ---	FOLLOW TARGET ---

		FEventTriggerCondition patrol_ExecutionCondition_1{};
		patrol_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		patrol_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::FollowTarget, patrol_ExecutionCondition_1);

		FEventTriggerCondition patrol_ExecutionCondition_2{};
		patrol_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		patrol_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		patrol_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::FollowTarget, patrol_ExecutionCondition_2);


		FEventTriggerCondition patrol_RemovalCondition_1{};
		patrol_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_1);

		FEventTriggerCondition patrol_RemovalCondition_2{};
		patrol_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_2.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_2);

		FEventTriggerCondition patrol_RemovalCondition_3{};
		patrol_RemovalCondition_3.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_3.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_3);

		FEventTriggerCondition patrol_RemovalCondition_4{};
		patrol_RemovalCondition_4.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		patrol_RemovalCondition_4.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::FollowTarget, patrol_RemovalCondition_4);


		// ---	IDLE ---

		FEventTriggerCondition idle_ExecutionCondition_1{};
		idle_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::Idle, idle_ExecutionCondition_1);

		FEventTriggerCondition idle_ExecutionCondition_2{};
		idle_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		idle_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Jump, EEventState::Inactive);
		idle_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		idle_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		idle_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::Idle, idle_ExecutionCondition_2);

		FEventTriggerCondition idle_RemovalCondition_1{};
		idle_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::Idle, idle_RemovalCondition_1);

		FEventTriggerCondition idle_RemovalCondition_2{};
		idle_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		idle_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::Idle, idle_RemovalCondition_2);

		// ---	JUMP ---

		FEventTriggerCondition jump_ExecutionCondition_1{};
		jump_ExecutionCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		jump_ExecutionCondition_1.EventName_To_ActiveState.Push(SActionNames::Idle, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::Jump, jump_ExecutionCondition_1);

		FEventTriggerCondition jump_ExecutionCondition_2{};
		jump_ExecutionCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyGate;
		jump_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Idle, EEventState::Inactive);
		jump_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		jump_ExecutionCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Inactive);
		jump_ExecutionCondition_2.EventName_To_ActiveState.Push(SEventNames::OnZeroHealth, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::Jump, jump_ExecutionCondition_2);

		FEventTriggerCondition jump_RemovalCondition_1{};
		jump_RemovalCondition_1.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		jump_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnGroundCollision, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::Jump, jump_RemovalCondition_1);

		FEventTriggerCondition jump_RemovalCondition_2{};
		jump_RemovalCondition_2.EventTriggerConditionType = EEventTriggerConditionType::OnlyTrigger;
		jump_RemovalCondition_2.EventName_To_ActiveState.Push(SActionNames::Die, EEventState::Active);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::Jump, jump_RemovalCondition_2);

		// ---- WOUNDED ----

		FEventTriggerCondition wounded_ExecutionCondition_1{};
		wounded_ExecutionCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Active);
		blorb_StateMachine.AddConditionsToExecute(SActionNames::Wounded, wounded_ExecutionCondition_1);

		FEventTriggerCondition wounded_RemovalCondition_1{};
		wounded_RemovalCondition_1.EventName_To_ActiveState.Push(SEventNames::OnReceiveDamage, EEventState::Inactive);
		blorb_StateMachine.AddConditionsToRemove(SActionNames::Wounded, wounded_RemovalCondition_1);


		// ---- SET PATROL AS DEFAULT ----

		blorb_StateMachine.TriggerState(SActionNames::ChooseNextPatrolTarget, EEventStateToTrigger::ConditionalExecution);
		blorb_StateMachine.TriggerState(SActionNames::FollowTarget, EEventStateToTrigger::ConditionalExecution);

		m_Registry.SetComponent<UStateMachine>(blorb_StateMachine, blorbsHandleContainer.EntityAt(currentIndex));

		m_CreateTreasures.CreateRandomDropableTreasures(blorbsHandleContainer.EntityAt(currentIndex), 0, 6, m_CurrentSceneIndex);

	}
}