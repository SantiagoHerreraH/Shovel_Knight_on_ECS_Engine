#include "../../../include/Systems/EventSystems/SEventNames.h"

namespace Pillar{
	const std::string SActionNames::Rebound{ "Rebound" };
	const std::string SActionNames::CanJump{ "CanJump" };
	const std::string SActionNames::Jump{ "Jump" };
	const std::string SActionNames::Idle{ "Idle" };
	const std::string SActionNames::Fall{ "Fall" };
	const std::string SActionNames::Land{ "Land" };
	const std::string SActionNames::TurnLeft{ "TurnLeft" };
	const std::string SActionNames::TurnRight{ "TurnRight" };
	const std::string SActionNames::AttackForward{ "AttackForward" };
	const std::string SActionNames::AttackDown{ "AttackDown" };
	const std::string SActionNames::AttackUp{ "AttackUp" };
	const std::string SActionNames::ClimbIdle{ "ClimbIdle" };
	const std::string SActionNames::Immune{ "Immune" };
	const std::string SActionNames::NotImmune{ "NotImmune" };
	const std::string SActionNames::Wounded{"Wounded"};
	const std::string SActionNames::Die{ "Die" };
	const std::string SActionNames::Revive{"Revive"};
	const std::string SActionNames::Despawn{"Despawn"};
	const std::string SActionNames::Spawn{"Spawn"};
	const std::string SActionNames::NewSpawn{ "NewSpawn" };
	const std::string SActionNames::FollowTarget{"Patrol"};
	const std::string SActionNames::ChooseNextPatrolTarget{"ChooseNextPatrolTarget"};
	const std::string SActionNames::ChoosePlayerTarget{"ChoosePlayerTarget"};


	const std::string SActionNames::Lit{"Lit"};
	const std::string SActionNames::SparkedUp{"SparkedUp"};
	const std::string SActionNames::Unlit{"Unlit"};

	const std::string SActionNames::Selected{ "Selected" };

	const std::string SActionNames::MoveLeft{ "MoveLeft" };
	const std::string SActionNames::MoveRight{ "MoveRight" };
	const std::string SActionNames::MoveUp{ "MoveUp" };
	const std::string SActionNames::MoveDown{ "MoveDown" };

	const std::string SEventNames::OnPlayerInRange{"OnPlayerInRange"};
	const std::string SEventNames::OnTargetInRange{ "OnTargetInRange" };
	const std::string SEventNames::OnZeroHealth{ "OnZeroHealth" };
	const std::string SEventNames::OnReceiveDamage{"OnReceiveDamage"};
	const std::string SEventNames::OnAttackHit{"OnAttackHit"};
	const std::string SEventNames::OnCollision{ "OnCollision" };
	const std::string SEventNames::OnGroundCollision{ "OnGroundCollision" };
	const std::string SEventNames::OnJump{ "OnJump" };
	const std::string SEventNames::OnFall{ "OnFall" };
	const std::string SEventNames::OnStill{ "OnStill" };
	const std::string SEventNames::OnMoveLeft{ "OnMoveLeft" };
	const std::string SEventNames::OnMoveRight{ "OnMoveRight" };

	const std::string SEventNames::OnHover{"OnHover"};
	
	const std::string SEventNames::OnCollisionWith(const std::string& tagName)
	{
		return "OnCollisionWith" + tagName;
	}

}