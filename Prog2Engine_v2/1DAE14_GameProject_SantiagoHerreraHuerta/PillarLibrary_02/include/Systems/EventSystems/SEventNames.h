#pragma once

#include <string>

namespace Pillar {

	/*
		Actions are defined by the state bound to them
	*/
	struct SActionNames {

		static const std::string Rebound;
		static const std::string CanJump;
		static const std::string Jump;
		static const std::string TurnLeft;
		static const std::string TurnRight;
		static const std::string AttackForward;
		static const std::string AttackDown;
		static const std::string AttackUp;

		static const std::string MoveLeft;
		static const std::string MoveRight;
		static const std::string MoveUp;
		static const std::string MoveDown;

		static const std::string Idle;
		static const std::string Fall;
		static const std::string Land;
		static const std::string Wounded;
		static const std::string Die;
		static const std::string Revive;
		static const std::string Despawn;
		static const std::string Spawn;
		static const std::string NewSpawn;
		static const std::string ClimbIdle;
		static const std::string Immune;
		static const std::string NotImmune;

		static const std::string Lit;
		static const std::string SparkedUp;
		static const std::string Unlit;
		
		static const std::string FollowTarget;
		static const std::string ChooseNextPatrolTarget;
		static const std::string ChoosePlayerTarget;

		static const std::string Selected;
	};

	/*

	EVENTS THAT ARE CONSEQUENCES OF ACTIONS AND CONTEXT

		Events are consequences of actions and context and check real data

	*/
	struct SEventNames {
		static const std::string OnPlayerInRange;
		static const std::string OnTargetInRange;
		static const std::string OnZeroHealth;
		static const std::string OnReceiveDamage;
		static const std::string OnAttackHit;

		static const std::string OnJump;
		static const std::string OnFall;
		static const std::string OnStill;
		static const std::string OnMoveLeft;
		static const std::string OnMoveRight;

		static const std::string OnCollision;
		static const std::string OnGroundCollision;

		static const std::string OnHover;

		static const std::string OnCollisionWith(const std::string& tagName);
	};
}