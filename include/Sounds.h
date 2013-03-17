#pragma once
#include "common/Singleton.h"
#include "common/Listeners.h"
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
   class SoundManager : public Singleton<SoundManager>, public CommandListener
    {
        public:
            SoundManager();
            ~SoundManager();
            bool init(); //add the bufferFile of sounds you want to play here
            void cleanup();
            void cleanMemory();
            float play(string audioFileName);
            void stopMusic(string audioFile);
            void stopSound(string audioFileName, int id, float elapsedTime); //if soundfile is looping, you have to turn looping
            //off before you can stop it. If this is the case, elapsedTime is to be measured from the time you turned looping off.
            void pauseMusic(string audioFile);
            void setLoopMusic(string audioFile, bool isLoop);
            void setLoopSound(string audioFile, int id, float elapsedTime, bool isLoop);
            void setPitchMusic(string audioFile, float pitchLevel);
            void setPitchSound(string audioFile, int id, float elapsedTime, float pitchLevel);
            void setVolumeMusic(string audioFile, float soundLevel);
            void setVolumeSound(string audioFile, int id, float elapsedTime, float soundLevel);
            void setMinimumDistanceMusic(string audioFile, float distance);
            void setMinimumDistanceSound(string audioFile, int id, float elapsedTime, float distance);
            void set3DPositionMusic(string audioFile, float positionX, float positionY, float positionZ);
            void set3DPositionSound(string audioFile, int id, float elapsedTime, float positionX, float positionY, float positionZ);
            void setRelativeMusic(string audioFile, bool isRelative);
            void setRelativeSound(string audioFile, int id, float elapsedTime, bool isRelative);
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
			bool handleCommand(string command);
    };
}
