#pragma once
#include "../Engine/Texture.h"
#include "../Engine/Transform.h"

#include "BaseGame.h"
#include "../1DAE14_GameProject_SantiagoHerreraHuerta/PillarLibrary_02/include/InternalIncludes.h"

class Game : public BaseGame
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	// http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Rh-override
	~Game();

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize();
	void Cleanup( );
	void ClearBackground( ) const;

	Pillar::URegistry		m_Registry;

	Pillar::UCreateWorld			m_CreateWorld				{ m_Registry };
	Pillar::UOptimizationSystem		m_OptimizationSystem		{ m_Registry };
	Pillar::UInteractionSystem		m_InteractionSystem			{ m_Registry };
	Pillar::USound					m_Sound						{ m_Registry };
	Pillar::UTransformState			m_TransformSystems			{ m_Registry };
	Pillar::UEventChecker			m_EventChecker				{ m_Registry };
	Pillar::UPhysics				m_Physics					{ m_Registry };
	Pillar::URectSceneGraph			m_RectSceneGraph			{ m_Registry };
	Pillar::URectCollision			m_RectCollision				{ m_Registry };
	Pillar::UDraw					m_Draw						{ m_Registry };
	Pillar::UDebug					m_Debug						{ m_Registry };
	Pillar::UInputSystem			m_InputSystem				{ m_Registry };
	Pillar::UMouseInputSystem		m_MouseInputSystem			{ m_Registry };
	
};