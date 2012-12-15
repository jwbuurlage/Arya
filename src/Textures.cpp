#include "Textures.h"
#include "common/Logger.h"
#include "Files.h"
#include <sstream>
#include <GL/glfw.h>
#include "ext/stb_image.c"

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
		loadWhiteTexture();
		return 1;
  }

  void TextureManager::cleanup(){
    unloadAll();
  }

  Texture* TextureManager::loadResource( const char* filename ){
    File* imagefile = FileSystem::shared().getFile(filename);
    if( imagefile == 0 ) return 0;

    Texture* texture = 0;

    int width, height, channels;
    // NOTE: using STBI_default as last arg gives wrong pixel data
    unsigned char* ptr = stbi_load_from_memory((stbi_uc*)imagefile->getData(), imagefile->getSize(), &width, &height, &channels, STBI_rgb_alpha); 
    if(ptr)
    {
      texture = new Texture;
      texture->width = width;
      texture->height = height;

      glGenTextures(1, &texture->handle);
      glBindTexture(GL_TEXTURE_2D, texture->handle);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ptr);

      addResource(filename, texture);

      stbi_image_free(ptr);
    }
    else
    {
      LOG_ERROR("Unable to read image data of " << filename << ". Reason: " << stbi_failure_reason());
    }

    //OpenGL has the image data now
    FileSystem::shared().releaseFile(imagefile);
    return texture;
  }

  void TextureManager::loadDefaultTexture(){
    if( resourceLoaded("default") ) return;

    Texture* defaultTex = new Texture;
    defaultTex->handle = 0;
    defaultTex->width = 16;
    defaultTex->height = 16;

    unsigned int pixelCount = defaultTex->width * defaultTex->height;
    float* imageData = new float[pixelCount * 4];
    for(unsigned int i = 0; i < pixelCount; ++i){
		bool purple=((i%defaultTex->width)+(i/defaultTex->width)%2==0);
		imageData[4*i+0] = purple?1.0f:0.0f; //red
		imageData[4*i+1] = 0.0f; //green
		imageData[4*i+2] = purple?1.0f:0.0f; //blue
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
    defaultResource = defaultTex;

    return;
  }

  void TextureManager::loadWhiteTexture()
  {
    Texture* whiteTex= new Texture;
    whiteTex->handle = 0;
    whiteTex->width = 4;
    whiteTex->height = 4;
    unsigned int pixelCount = whiteTex->width * whiteTex->height;
    float* imageData = new float[pixelCount * 4];
    for(unsigned int i = 0; i < pixelCount; ++i)
    {
      imageData[4*i+0] = 1.0f; //red
      imageData[4*i+1] = 1.0f; //green
      imageData[4*i+2] = 1.0f; //blue
      imageData[4*i+3] = 1.0f; //alpha
    }
    glGenTextures(1, &whiteTex->handle);
    glBindTexture( GL_TEXTURE_2D, whiteTex->handle );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, whiteTex->width, whiteTex->height, 0, GL_RGBA, GL_FLOAT, imageData );

    delete[] imageData;

    addResource("white", whiteTex);
    return;

  }
}

