#pragma once
#include "Resources.h"
#include "../ext/stb_truetype.h"
#include "common/Singleton.h"
#include <GL/glew.h>

namespace Arya
{
  class File;

  class Font
  {
    public:
      stbtt_bakedchar* baked[100];
      Font(){textureHandle = 0;}
      ~Font(){if( textureHandle ) glDeleteTextures(1, &textureHandle);}
      GLuint textureHandle;
  };

  class FontManager : public Singleton<FontManager>, public ResourceManager<Font>
  {
    public: 
      FontManager();
      ~FontManager();

      int initialize();
      void cleanup();

      Font* getFont( const char* filename )
      {
        return getResource(filename); 
      }

    private:
      Font* loadResource( const char* filename);

      void loadDefaultFont();
      void makeImage(File* filename,Font* font);
  };
}
