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
            Texture(){ handle = 0; width = 0; height = 0; }
            ~Texture(){ if( handle ) glDeleteTextures(1, &handle); }
            GLuint handle;
            GLuint width;
            GLuint height;
            //we could add more info about
            //bit depths and mipmap info and so on
    };

    class TextureManager : public Singleton<TextureManager>, public ResourceManager<Texture> {
        public:
            TextureManager();
            ~TextureManager();

            int initialize();
            void cleanup();

            Texture* getTexture( const char* filename ){ return getResource(filename); }

        private:
            Texture* loadResource( const char* filename );

            void loadDefaultTexture(); //Generates default texture
    };
}