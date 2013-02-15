#include "Sounds.h"
#include "common/Logger.h"
#include <vector>
#include <SFML/Audio.hpp>
using std::vector;

namespace Arya
{
    template<> SoundManager* Singleton<SoundManager>::singleton = 0;

    SoundManager::SoundManager()
    {
        sf::SoundBuffer buffer;
        sf::Sound sound;
        sf::Music music;
    };

    SoundManager::~SoundManager()
    {
        cleanup();
    }

    void SoundManager::cleanup()
    {
    }

    void SoundManager::play(string audioFileName)
    {
        File* audioFile = FileSystem::shared().getFile(audioFileName);
        if(audioFile->getSize() > 2)
        {
            getMusicFile(audioFile);
            music.Play();
        }
        else
        {
            getSoundFile(audioFile);
            sound.SetBuffer(buffer);
            sound.Play();
        }
    }

    void SoundManager::stop(string audioFile)
    {

    }

    void SoundManager::loop(string audioFile, int loopLength)
    {

    }

    void SoundManager::setPitch(string audioFile, float pitchLevel)
    {

    }

    void SoundManager::setVolume(string audioFile, float soundLevel)
    {

    }

    int SoundManager::getOffset(string audioFile)
    {
        return 0;
    }

    void SoundManager::getSoundFile(File* bufferFile)
    {
        if(!buffer.LoadFromMemory(bufferFile->getData(),bufferFile->getSize()))
        {
            LOG_WARNING("The audiofile cannot be loaded into sound buffer!");
        }
    }
    void SoundManager::getMusicFile(File* musicFile)
    {
        if(!music.OpenFromMemory(musicFile->getData(),musicFile->getSize()))
        {
            LOG_WARNING("The audiofile cannot be loaded into music buffer!");
        }
    }
}
