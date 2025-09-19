#pragma once

#include "../../../include/Systems/CollisionSystems/UCollision.h"
#include "../../../include/Structs/FSceneGraphStructure.h"

namespace Pillar {

	class UCalculateStaticRectCollision {
	public:
		UCalculateStaticRectCollision(URegistry& registry);
		void Update();

	private:

		URegistry& m_Registry;

		UComponentIterator<FRectCollider>								m_MovableRectColliders;
		UComponentIterator<FEntityResolveOverlapInfo>					m_MovableShapeOverlapInfos;
		UComponentIterator<ECollisionBodyType>							m_MovableCollisionBodyTypes;

		UComponentIterator<FRectCollider>								m_StaticRectColliders;
		UComponentIterator<FEntityResolveOverlapInfo>					m_StaticShapeOverlapInfos;
	};


	class UCalculateMovableRectCollision {
	public:
		UCalculateMovableRectCollision(URegistry& registry);
		void Update();

	private:
		void ResetShapeOverlapInfo();

		URegistry& m_Registry;
		URegistrySingleton<FSceneRegistry>							m_SceneNodes;

		UComponentIterator<FRectCollider>							m_RectColliders;
		UComponentIterator<FEntityResolveOverlapInfo>				m_ShapeOverlapInfos;
		UComponentIterator<ECollisionBodyType>						m_CollisionBodyTypes;
	};

	class URectCollision final {

	public:
		URectCollision(URegistry& registry);
		void Update(float deltaSeconds);

	private:
		
		UCalculateMovableRectCollision m_CalculateMovableRectCollision;
		UCalculateStaticRectCollision m_CalculateStaticRectCollision;
		UExecuteSolidCollision m_ExecuteSolidCollision;
	};

	

	class UOptimization {

	};
}