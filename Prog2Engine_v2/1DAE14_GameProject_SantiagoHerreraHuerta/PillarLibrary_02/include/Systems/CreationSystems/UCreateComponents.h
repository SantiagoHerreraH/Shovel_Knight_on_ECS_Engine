#pragma once

#include "../../../include/Core/URegistry.h"
#include "../../../include/Core/UEntityCreation.h"

namespace Pillar {

	struct SComponentNames {
		static const DComponentBlockName StateMachine;
		static const DComponentBlockName AI;
		static const DComponentBlockName Transformable;
		static const DComponentBlockName SceneNode;
		static const DComponentBlockName SceneObject;
		static const DComponentBlockName Shape;
		static const DComponentBlockName Physics;
		static const DComponentBlockName Collider;
		static const DComponentBlockName RectCollider;
		static const DComponentBlockName Homing;
		static const DComponentBlockName LookAt;
		static const DComponentBlockName Player;
		static const DComponentBlockName Camera;
		static const DComponentBlockName Grids;
		static const DComponentBlockName SpriteTransform;
		static const DComponentBlockName ParallaxObject;
		static const DComponentBlockName Health;
		static const DComponentBlockName Attack;
		static const DComponentBlockName Tag;
	};

	class UCreateComponents final{

	public:

		UCreateComponents(URegistry& registry);
		void Create();

	private:
		URegistry& m_Registry;
		UArchetypeCreator m_ArchetypeCreator;
	};
}