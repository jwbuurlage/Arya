#pragma once

namespace Arya
{
    class FileSystem;
    class Audio;

    class Locator
    {
        public:
            static FileSystem& getFileSystem() { return *fileSystem; }
            static Audio& getAudio() { return *audio; }

            static void provide(FileSystem* f) { fileSystem = f; }
            static void provide(Audio* a) { audio = a; }
        private:
            static FileSystem* fileSystem;
            static Audio* audio;
    };
}
