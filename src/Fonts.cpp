#define STB_TRUETYPE_IMPLEMENTATION

#include "Fonts.h"
#include "Files.h"
#include "Resources.h"
#include "common/Logger.h"
#include "../ext/stb_truetype.h"
#include <GL/glfw.h>
#include <iostream>
using namespace std;

namespace Arya
{
    template<> FontManager* Singleton<FontManager>::singleton = 0;

    FontManager::FontManager(){}
    FontManager::~FontManager()
    {
        cleanup();
    }

    int FontManager::initialize()
    {
        loadDefaultFont();
        return 1;
    }
    void FontManager::cleanup(){}

    void FontManager::loadDefaultFont()
    {
        loadResource("courier.ttf");
    }

    Font* FontManager::loadResource(std::string filename)
    {
        File* fontfile = FileSystem::shared().getFile(string("fonts/") + filename);
        if( fontfile == 0 )
        {
            cout << "Font not found!" << endl;
            return 0;
        }
        Font* font = new Font;
        makeImage(fontfile,font);
        addResource(filename, font);
        return font;
    }

    void FontManager::makeImage(File* file,Font* font)
    {
        unsigned char pixeldata[512*512];
        int width = 512;
        int height = 512;
        stbtt_BakeFontBitmap((unsigned char*)file->getData(), 0, 20, pixeldata, width, height, 0, 150, font->baked);
        glGenTextures(1, &font->textureHandle);
        glBindTexture(GL_TEXTURE_2D, font->textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixeldata ); 
    }
}
