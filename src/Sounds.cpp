#include "Sounds.h"
#include "common/Logger.h"
#include <vector>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
using std::vector;

namespace Arya
{
    template<> SoundManager* Singleton<SoundManager>::singleton = 0;

    SoundManager::SoundManager()
    {

    };

    SoundManager::~SoundManager()
    {
        cleanup();
    }

    void SoundManager::init()
    {
        getBufferFile("sounds/testSound.wav");
    }
    void SoundManager::cleanup()
    {
    }

    void SoundManager::cleanMemory()
    {

    }
    float SoundManager::play(string audioFileName)
    {
        bool flag = false;
        int count = -1;
        if(bufferCollection.find(audioFileName) == bufferCollection.end())
        {
            getMusicFile(audioFileName);
            musicCollection[audioFileName]->play();
            return -999.;
        }
        else
        {
            count = bindSoundFile(audioFileName);
            soundCollection[count]->play();
            return soundCollection[count]->getBuffer()->getDuration().asSeconds();
        }
    }

    void SoundManager::stopMusic(string audioFile)
    {
        if(musicCollection.find(audioFile) != musicCollection.end())
        {
            musicCollection[audioFile]->stop();
        }
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

    void SoundManager::getBufferFile(string audioFileName)
    {
        if(bufferCollection.find(audioFileName) != bufferCollection.end())
        {
            LOG_WARNING("Bufferfile" << audioFileName << " already defined!");
        }
        File* audioFile = FileSystem::shared().getFile(audioFileName);
        sf::SoundBuffer *bufferPushBack = new sf::SoundBuffer;
        bufferPushBack->loadFromMemory(audioFile->getData(),audioFile->getSize());
        bufferCollection.insert(BufferContainer::value_type(audioFileName,bufferPushBack));
    }
    int SoundManager::bindSoundFile(string audioFileName)
    {
        if(bufferCollection.find(audioFileName) == bufferCollection.end())
        {
            LOG_WARNING("Buffer not found:" << audioFileName);
            return -999;
        }
        for(unsigned int i = 0; i < soundCollection.size();i++)
        {
            if(soundCollection[i]->getStatus() == sf::SoundSource::Stopped)
            {
                return i;
            }
        }
        sf::Sound *soundPushBack = new sf::Sound;
        soundPushBack->setBuffer(*bufferCollection[audioFileName]);
        soundCollection.push_back(soundPushBack);
        return soundCollection.size() - 1;
    }
    void SoundManager::getMusicFile(string musicFileName)
    {
        if(musicCollection.find(musicFileName)!=musicCollection.end() && musicCollection[musicFileName]->getStatus() == sf::SoundSource::Stopped)
        {
            return;
        }
        File* musicFile = FileSystem::shared().getFile(musicFileName);
        sf::Music *musicPushBack = new sf::Music;
        musicPushBack->openFromMemory(musicFile->getData(),musicFile->getSize());
        musicCollection.insert(MusicContainer::value_type(musicFileName,musicPushBack));
        return;
    }
}
