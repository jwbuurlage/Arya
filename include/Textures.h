//Please see Resources.h for explanation
//for how a new texture is added to the resource list
#pragma once
#include "common/Singleton.h"
#include "Resources.h"
#include <GL/glew.h>

namespace Arya
{
    class Texture{
        public:
            GLuint handle;
            GLuint width;
            GLuint height;
            //we could add more info about
            //bit depths and mipmap info and so on
        private:
            //Only TextureManager can create Textures
            friend class TextureManager;
            friend class ResourceManager<Texture>;
            Texture(){ handle = 0; width = 0; height = 0; }
            ~Texture(){ if( handle ) glDeleteTextures(1, &handle); }
    };

    class TextureManager : public Singleton<TextureManager>, public ResourceManager<Texture> {
        public:
            TextureManager();
            ~TextureManager();

            int initialize();
            void cleanup();

            //If no texture found it will return 0
            Texture* getTexture( std::string filename ){ return getResource(filename); }

        private:
            Texture* loadResource( std::string filename );

            void loadDefaultTexture(); //Generates default texture
            void loadWhiteTexture(); //Generates white texture for overlay
    };
}
