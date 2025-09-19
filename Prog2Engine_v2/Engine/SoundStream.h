#pragma once
#include <string>
class SoundStream final
{
public:
	explicit SoundStream( const std::string& path );
	~SoundStream( );
	SoundStream(SoundStream&& other)noexcept;
	SoundStream& operator=(SoundStream&& other)noexcept;

	SoundStream( const SoundStream& other ) = delete;
	SoundStream& operator=( const SoundStream& rhs ) = delete;


	bool IsLoaded( ) const;
	bool Play( bool repeat ) const;
	static void Stop( );
	static void Pause( );
	static void Resume( );
	static void SetVolume(int value );
	static int GetVolume( );
	static bool IsPlaying( );

private:
	Mix_Music *m_pMixMusic;
};