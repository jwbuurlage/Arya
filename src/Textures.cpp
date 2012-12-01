#include "Textures.h"
#include "common/Logger.h"
#include "Files.h"
#include <sstream>
#include <GL/glfw.h>

namespace Arya
{
    template<> TextureManager* Singleton<TextureManager>::singleton = 0;

    TextureManager::TextureManager(){
    }

    TextureManager::~TextureManager(){
        cleanup();
    }

    int TextureManager::initialize(){
        loadDefaultTexture();
        return 1;
    }

    void TextureManager::cleanup(){

    }

    Texture* TextureManager::loadResource( const char* filename ){
        File* imagefile = FileSystem::shared().getFile(filename);
        if( imagefile == 0 ) return 0;

        //sf::Image image;
        //if( image.loadFromMemory(imagefile->data, imagefile->size) )

        Texture* texture = 0;

        GLFWimage glfwimage;
        if( glfwReadMemoryImage(imagefile->getData(), imagefile->getSize(), &glfwimage, 0) == GL_TRUE )
        {
            texture = new Texture;
            texture->width = glfwimage.Width; //image.getSize().x;
            texture->height = glfwimage.Height; //image.getSize().y;

            glGenTextures(1, &texture->handle);
            glBindTexture( GL_TEXTURE_2D, texture->handle );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

            glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, glfwimage.Format, GL_UNSIGNED_BYTE, glfwimage.Data);

            addResource(filename, texture);
            glfwFreeImage(&glfwimage);
        }
        else
        {
            LOG_ERROR("Unable to read image data of " << filename);
        }

        //OpenGL has the image data now
        FileSystem::shared().releaseFile(imagefile);
        return texture;
    }

    void TextureManager::loadDefaultTexture(){
        if( resourceLoaded("default") ) return;

        Texture* defaultTex = new Texture;
        defaultTex->handle = 0;
        defaultTex->width = 4;
        defaultTex->height = 4;

        unsigned int pixelCount = defaultTex->width * defaultTex->height;
        float* imageData = new float[pixelCount * 4];
        for(unsigned int i = 0; i < pixelCount; ++i){
            imageData[4*i+0] = 0.6f*i/pixelCount+0.4f; //red
            imageData[4*i+1] = 0.6f*1/pixelCount; //green
            imageData[4*i+2] = 0.8f*i/pixelCount+0.2f; //blue
            imageData[4*i+3] = 0.8f; //alpha
        }

        //glGetError();
        glGenTextures(1, &defaultTex->handle);
        glBindTexture( GL_TEXTURE_2D, defaultTex->handle );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, defaultTex->width, defaultTex->height, 0, GL_RGBA, GL_FLOAT, imageData );

        delete[] imageData;

        addResource("default", defaultTex);

        return;
    }
}

