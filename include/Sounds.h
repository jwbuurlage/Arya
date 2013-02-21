#pragma once
#include "common/Singleton.h"
#include "Files.h"
#include <vector>
#include <string>
#include <map>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
using std::string;
using std::vector;
using std::map;
namespace Arya
{
   class SoundManager: public Singleton<SoundManager>

    {
        public:
            SoundManager();
            ~SoundManager();
            bool init();
            void cleanup();
            void cleanMemory();
            float play(string audioFileName);
            void stopMusic(string audioFile);
            void loop(string audioFile, int loopLength);
            void setPitch(string audioFile, float pitchLevel);
            void setVolume(string audioFile, float soundLevel);
            int getOffset(string audioFile);
        private:
            typedef map<string,sf::SoundBuffer*> BufferContainer;
            BufferContainer bufferCollection;
            vector<sf::Sound*> soundCollection;
            typedef map<string,sf::Music*> MusicContainer;
            MusicContainer musicCollection;
            bool getBufferFile(string audioFileName);
            int bindSoundFile(string audioFileName);
            bool getMusicFile(string musicFileName);
    };
}
