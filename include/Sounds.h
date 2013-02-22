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
   struct SoundIdentity
   {
        string nameOfFile;
        int soundID;
        int placeInSoundCollection;
        float length;
   };
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
            void stopSound(string audioFileName, int id, float elapsedTime);
            void loopmusic(string audioFile, bool isLoop);
            void setPitch(string audioFile, float pitchLevel);
            void setVolume(string audioFile, float soundLevel);
            int getOffset(string audioFile);
        private:
            typedef map<string,sf::SoundBuffer*> BufferContainer;
            BufferContainer bufferCollection;
            vector<sf::Sound*> soundCollection;
            typedef map<string,sf::Music*> MusicContainer;
            MusicContainer musicCollection;
            vector<SoundIdentity> soundIdentity;
            bool getBufferFile(string audioFileName);
            int bindSoundFile(string audioFileName);
            bool getMusicFile(string musicFileName);
            void provideSoundID(string audioFileName);
    };
}
