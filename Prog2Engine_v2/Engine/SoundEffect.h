#pragma once
#include <string>
class Mix_Chunk;
class SoundEffect final
{
public:
	explicit SoundEffect( const std::string& path );
	~SoundEffect( ); 
	SoundEffect(SoundEffect&& other)noexcept;
	SoundEffect& operator=(SoundEffect&& rhs)noexcept;

	SoundEffect(const SoundEffect& other) = delete;
	SoundEffect& operator=(const SoundEffect& rhs) = delete;
	

	bool IsLoaded( ) const;
	bool Play( const int loops ) const;
	void SetVolume( const int value ); 
	int GetVolume( ) const; 
	static void StopAll( );
	static void PauseAll( );
	static void ResumeAll( );

private:
	Mix_Chunk* m_pMixChunk;
};
