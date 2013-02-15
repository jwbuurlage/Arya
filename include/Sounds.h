#pragma once
#include "common/Singleton.h"
#include "Files.h"
#include <string>
#include <SFML/Audio.hpp>
using std::string;
namespace Arya
{
    class SoundManager: public Singleton<SoundManager>

    {
        public:
            SoundManager();
            ~SoundManager();
            void cleanup();
            void play(string audioFileName);
            void stop(string audioFile);
            void loop(string audioFile, int loopLength);
            void setPitch(string audioFile, float pitchLevel);
            void setVolume(string audioFile, float soundLevel);
            int getOffset(string audioFile);
        private:
            sf::SoundBuffer buffer;
            sf::Sound sound;
            sf::Music music;
            void getSoundFile(File* bufferFile);
            void getMusicFile(File* musicFile);
    };
}
