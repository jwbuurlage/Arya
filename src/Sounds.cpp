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

    bool SoundManager::init()
    {
        if(!getBufferFile("testSound.wav")) return false;
        return true;
    }
    void SoundManager::cleanup()
    {
    }

    void SoundManager::cleanMemory()
    {

    }
    float SoundManager::play(string audioFileName)
    {
        int count = -1;
        if(bufferCollection.find(audioFileName) == bufferCollection.end())
        {
            if(!getMusicFile(audioFileName))
            {
                LOG_WARNING("File not in any collection! Can't play this sound!");
                return -1000.;
            }
            musicCollection[audioFileName]->play();
            return -999.;
        }
        else
        {
            provideSoundID(audioFileName);
            count = bindSoundFile(audioFileName);
            if(count == -999) 
            {
                soundIdentity.pop_back();
                return -1000.;
            }
            else
            {
                soundIdentity[soundIdentity.size()-1].placeInSoundCollection = count;
                soundIdentity[soundIdentity.size()-1].length = soundCollection[count]->getBuffer()->getDuration().asSeconds();
                soundCollection[count]->play();
                return soundIdentity[soundIdentity.size()-1].soundID;
            }
        }
    }
    void SoundManager::provideSoundID(string audioFileName)
    {
        int count = 0;
        for(unsigned int i = 0; i < soundIdentity.size(); i++)
        {
            if(soundIdentity[i].nameOfFile == audioFileName) count += 1;
        }
        SoundIdentity idPushBack;
        idPushBack.nameOfFile = audioFileName;
        idPushBack.soundID = count;
        idPushBack.placeInSoundCollection = -1;
        idPushBack.length = -1.;
        soundIdentity.push_back(idPushBack);
    }

    void SoundManager::stopMusic(string audioFile)
    {
        if(musicCollection.find(audioFile) != musicCollection.end())
        {
            musicCollection[audioFile]->stop();
        }
        else LOG_WARNING("No such music file found to stop!");
    }
    void SoundManager::stopSound(string audioFileName, int id, float elapsedTime)
    {
        for(unsigned int i = 0; i < soundIdentity.size(); i++)
        {
            if(soundIdentity[i].nameOfFile == audioFileName && soundIdentity[i].soundID == id)
            {
                if(elapsedTime < soundIdentity[i].length)
                {
                    soundCollection[soundIdentity[i].placeInSoundCollection]->stop();
                    return;
                }
                else
                {
                    LOG_INFO("Sound " << audioFileName << ", " << id << "already stopped!");
                    return;
                }
            }
        }

    }

    void SoundManager::loopMusic(string audioFile, bool isLoop)
    {
        if(musicCollection.find(audioFile) != musicCollection.end())
        {
            musicCollection[audioFile]->setLoop(isLoop);
        }
        else LOG_WARNING("No music file " << audioFile << " found to loop");
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

    bool SoundManager::getBufferFile(string audioFileName)
    {
        if(bufferCollection.find(audioFileName) != bufferCollection.end())
        {
            LOG_WARNING("Bufferfile" << audioFileName << " already defined!");
            return true;
        }
        File* audioFile = FileSystem::shared().getFile(string("sounds/")+audioFileName);
        if(audioFile == 0) return false;
        sf::SoundBuffer *bufferPushBack = new sf::SoundBuffer;
        bufferPushBack->loadFromMemory(audioFile->getData(),audioFile->getSize());
        bufferCollection.insert(BufferContainer::value_type(audioFileName,bufferPushBack));
        return true;
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
    bool SoundManager::getMusicFile(string musicFileName)
    {
        if(musicCollection.find(musicFileName)!=musicCollection.end() && musicCollection[musicFileName]->getStatus() == sf::SoundSource::Stopped)
        {
            return true;
        }
        File* musicFile = FileSystem::shared().getFile(string("sounds/")+musicFileName);
        if(musicFile == 0) return false;
        sf::Music *musicPushBack = new sf::Music;
        musicPushBack->openFromMemory(musicFile->getData(),musicFile->getSize());
        musicCollection.insert(MusicContainer::value_type(musicFileName,musicPushBack));
        return true;
    }
}
