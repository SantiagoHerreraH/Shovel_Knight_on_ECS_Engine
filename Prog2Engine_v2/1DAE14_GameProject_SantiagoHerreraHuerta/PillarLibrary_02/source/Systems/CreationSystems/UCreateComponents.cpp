#include "../../../include/Systems/CreationSystems/UCreateComponents.h"

#include "../../../include/InternalIncludes.h"

namespace Pillar {

	const DComponentBlockName SComponentNames::StateMachine{"StateMachine"};
	const DComponentBlockName SComponentNames::AI{"AI"};
	const DComponentBlockName SComponentNames::Transformable{ "Transformable" };
	const DComponentBlockName SComponentNames::SceneNode{ "SceneNode" };
	const DComponentBlockName SComponentNames::SceneObject{ "SceneObject" };
	const DComponentBlockName SComponentNames::Shape{ "Shape" };
	const DComponentBlockName SComponentNames::Physics{ "Physics" };
	const DComponentBlockName SComponentNames::Collider{ "Collider" };
	const DComponentBlockName SComponentNames::RectCollider{ "RectCollider" };
	const DComponentBlockName SComponentNames::Homing{ "Homing" };
	const DComponentBlockName SComponentNames::LookAt{"LookAt"};
	const DComponentBlockName SComponentNames::Player{ "Player" };
	const DComponentBlockName SComponentNames::Camera{ "Camera" };
	const DComponentBlockName SComponentNames::Grids{ "Grids" };
	const DComponentBlockName SComponentNames::SpriteTransform{ "SpriteTransform" };
	const DComponentBlockName SComponentNames::ParallaxObject{ "ParallaxObject" };
	const DComponentBlockName SComponentNames::Health{"Health"};
	const DComponentBlockName SComponentNames::Attack{"Attack"};
	const DComponentBlockName SComponentNames::Tag{"Tag"};

	UCreateComponents::UCreateComponents(URegistry& registry) :
		m_Registry(registry),
		m_ArchetypeCreator(registry)
	{
	}
	void UCreateComponents::Create()
	{
		UEntityHandle nullEntityHandle{};
		/*
		
			STATE MACHINE
		
		*/
		UStateMachine stateMachine{ m_Registry, nullEntityHandle};
		UArchetype StateMachineArchetype{ m_Registry, stateMachine };
		m_ArchetypeCreator.CreateComponent(SComponentNames::StateMachine, StateMachineArchetype);

		/*

			TAG

		*/
		FTag tag{};
		UArchetype TagArchetype{ m_Registry, tag };
		m_ArchetypeCreator.CreateComponent(SComponentNames::Tag, TagArchetype);


		/*
		
			TRANSFORMABLE

		*/

		FLocalTransform2D localTransform{};
		FWorldTransform2D worldTransform{};
		URealTransform2D realTransform{};
		FMatrix4x4f matrix{};

		UArchetype TransformableArchetype{ m_Registry, localTransform, worldTransform, realTransform, matrix };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Transformable, TransformableArchetype);

		/*
		
			SCENE NODE
		
		*/

		FSceneNode sceneNode{};

		UArchetype SceneNodeArchetype{m_Registry, sceneNode };

		m_ArchetypeCreator.CreateComponent(SComponentNames::SceneNode, SceneNodeArchetype);

		/*

			SCENE OBJECT

		*/

		FSceneObject sceneObject{};

		UArchetype SceneObjectArchetype{ m_Registry, sceneObject };

		m_ArchetypeCreator.CreateComponent(SComponentNames::SceneObject, SceneObjectArchetype);

		/*
		
			SHAPE

		*/

		FShape2Df shape{};
		FBaseShape2Df baseShape{};

		UArchetype ShapeArchetype{m_Registry, shape, baseShape};

		m_ArchetypeCreator.CreateComponent(SComponentNames::Shape, ShapeArchetype);

		/*
		
			PHYSICS
		
		*/

		FVelocity velocity{};
		FNetForce2D netForce{};
		FMass mass{};
		FEnvironmentForces environmentForce{};

		UArchetype PhysicsArchetype{ m_Registry, velocity, netForce, mass , environmentForce };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Physics, PhysicsArchetype);

		/*
		
			COLLIDER
		
		*/

		FObjectMaxCollisionRadiusPerGrid objectMaxCollisionRadiusPerGrid{};
		FEntityResolveOverlapInfo entityResolveOverlapInfo{};
		ECollisionBodyType collisionBodyType{};
		
		UArchetype CollisionArchetype{ m_Registry, objectMaxCollisionRadiusPerGrid, entityResolveOverlapInfo, collisionBodyType };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Collider, CollisionArchetype);

		/*

			RECT COLLIDER

		*/

		FRectCollider rectCollider{};

		UArchetype RectCollisionArchetype{ m_Registry, rectCollider, entityResolveOverlapInfo, collisionBodyType };

		m_ArchetypeCreator.CreateComponent(SComponentNames::RectCollider, RectCollisionArchetype);


		/*
		
			HOMING
		
		*/

		FHomingInformation homingInfo{};
		FTarget targetEntity{};

		UArchetype HomingArchetype{ m_Registry, homingInfo, targetEntity, };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Homing, HomingArchetype);

		/*
		
			LOOK AT
		
		*/

		FLookAtRotationInfo lookAtRotationInfo{};

		UArchetype LooAtArchetype{ m_Registry, lookAtRotationInfo, targetEntity, };

		m_ArchetypeCreator.CreateComponent(SComponentNames::LookAt, LooAtArchetype);

		/*
		
			PLAYER
		
		*/

		FPlayer player{};

		UArchetype PlayerArchetype{ m_Registry, player };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Player, PlayerArchetype);

		/*

			CAMERA

		*/

		FCamera camera{};

		UArchetype CameraArchetype{ m_Registry, camera };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Camera, CameraArchetype);

		/*
		
			GRIDS
		
		*/

		FParams_Grids grids{};

		UArchetype GridsArchetype{ m_Registry, grids };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Grids, GridsArchetype);

		/*

			SPRITE

		*/
	
		DTextureTransform textureTransform{};

		UArchetype SpriteArchetype{ m_Registry, textureTransform };

		m_ArchetypeCreator.CreateComponent(SComponentNames::SpriteTransform, SpriteArchetype);

		/*

			ParallaxObject

		*/

		FParallaxObject parallaxObject{};

		UArchetype ParallaxObjectArchetype{ m_Registry, parallaxObject };

		m_ArchetypeCreator.CreateComponent(SComponentNames::ParallaxObject, ParallaxObjectArchetype);

		/*

			Health

		*/

		FHealth health{0};

		UArchetype HealthArchetype{ m_Registry, health };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Health, HealthArchetype);

		/*

			Attack

		*/

		FAttack attack{};

		UArchetype AttackArchetype{ m_Registry, attack };

		m_ArchetypeCreator.CreateComponent(SComponentNames::Attack, AttackArchetype);

		
	}


}