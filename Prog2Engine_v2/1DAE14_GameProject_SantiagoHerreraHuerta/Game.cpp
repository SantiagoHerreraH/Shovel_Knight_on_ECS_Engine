#include "pch.h"
#include "Game.h"

Game::Game( const Window& window ) 
	:BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	m_InputSystem.Create();
	m_CreateWorld.Create();

	m_RectSceneGraph.InitializeSceneGraph();
	m_EventChecker.Initialize();
	m_OptimizationSystem.Initialize();
	m_RectSceneGraph.InitializeSceneObjects();
}

void Game::Cleanup( )
{
	Pillar::STextureRegistry::Delete();
}

void Game::Update( float elapsedSec )
{
	m_Sound.Update();
	m_OptimizationSystem.Update();
	m_InputSystem.Update(elapsedSec);
	m_InteractionSystem.Update(elapsedSec);
	m_TransformSystems.Update(elapsedSec);
	m_EventChecker.Check();
	m_Physics.Update(elapsedSec);
	m_RectSceneGraph.Update();
	m_RectCollision.Update(elapsedSec);
	m_RectSceneGraph.Update();
	m_Draw.Update(elapsedSec);
}

void Game::Draw( ) const
{
	ClearBackground();
	m_Draw.Draw();
	//m_Debug.Draw();
}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	//std::cout << "KEYUP event: " << e.keysym.sym << std::endl;
	//switch ( e.keysym.sym )
	//{
	//case SDLK_LEFT:
	//	//std::cout << "Left arrow key released\n";
	//	break;
	//case SDLK_RIGHT:
	//	//std::cout << "`Right arrow key released\n";
	//	break;
	//case SDLK_1:
	//case SDLK_KP_1:
	//	//std::cout << "Key 1 released\n";
	//	break;
	//}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;

	m_MouseInputSystem.CheckOnHover(e.x, e.y);
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}
	
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONUP event: ";
	//switch ( e.button )
	//{
	//case SDL_BUTTON_LEFT:
	//	std::cout << " left button " << std::endl;
	//	break;
	//case SDL_BUTTON_RIGHT:
	//	std::cout << " right button " << std::endl;
	//	break;
	//case SDL_BUTTON_MIDDLE:
	//	std::cout << " middle button " << std::endl;
	//	break;
	//}

	m_MouseInputSystem.CheckOnMouseUp(e.x, e.y);
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.0f, 0.0f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}
