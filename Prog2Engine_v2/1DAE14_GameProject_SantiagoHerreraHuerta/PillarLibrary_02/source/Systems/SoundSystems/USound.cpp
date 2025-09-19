#include "../../../include/Systems/SoundSystems/USound.h"


namespace Pillar {

    std::unordered_map<std::string, std::unique_ptr<SoundStream>> SSound::s_Name_To_SoundStream{};
    std::unordered_map<std::string, std::unique_ptr<SoundEffect>> SSound::s_Name_To_SoundEffect{};

    SoundStream& SSound::GetBackgroundMusic(const std::string& fileName)
    {
        if (!s_Name_To_SoundStream.contains(fileName))
        {
            s_Name_To_SoundStream.emplace(std::pair(fileName, std::make_unique<SoundStream>(SoundStream(fileName))));
        }

        return *s_Name_To_SoundStream.at(fileName).get();
    }

    SoundEffect& SSound::GetSoundEffect(const std::string& fileName)
    {
        if (!s_Name_To_SoundEffect.contains(fileName))
        {
            s_Name_To_SoundEffect.emplace(std::pair(fileName, std::make_unique<SoundEffect>(SoundEffect(fileName))));
        }

        return *s_Name_To_SoundEffect.at(fileName).get();
    }

    const SoundStream& SSound::GetConstBackgroundMusic(const std::string& fileName)
    {
        return GetBackgroundMusic(fileName);
    }

    const SoundEffect& SSound::GetConstEffectMusic(const std::string& fileName)
    {
        return GetSoundEffect(fileName);
    }

    USound::USound(URegistry& registry)
    {
        registry.GetComponents(m_BackgroundTracks);
    }

    void USound::Update()
    {
        for (size_t archIdx = 0; archIdx < m_BackgroundTracks.NumOfArchetypes(); archIdx++)
        {
            for (size_t compIdx = 0; compIdx < m_BackgroundTracks.NumOfActivatedComponents(archIdx); compIdx++)
            {
                if (!m_BackgroundTracks.At(archIdx, compIdx).Playing)
                {
                    m_BackgroundTracks.At(archIdx, compIdx).Playing = true;
                    SSound::GetBackgroundMusic(m_BackgroundTracks.At(archIdx, compIdx).Name).Play(true);
                }
                
            }
        }
    }

}


