#pragma once
#include "common/Listeners.h"
#include "common/Singleton.h"
#include <vector>
#include <string>
#include "Overlay.h"
#include "Fonts.h"
using std::vector;
using std::string;

namespace Arya
{
  class Console : public Singleton<Console>, public FrameListener
  {
    public:
    Console();
    ~Console();

    void cleanup();
    void onFrame(float elapsedTime);
    bool init();
    bool visibility //visibility of the kernel
    string currentLine;

    void toggleVisibilityConsole();
    void enterInput();

    private:
    void consoleInfo();
    int nrLines; //visible number of lines of console 
    int searchNrLines; //number of lines in which you can search, if more the first one will be kicked
    int textWidthInPixels; //width of character
    int nrCharOnLine;
    int pixelsInBetween; // pixels in between lines
    int activeLine;
    vector<string> history;
    vector<string> searchHistory;
    vector<Rect*> rects;
    Font* font;
    float time; // used for cursor flashing

    void setVisibilityConsole(bool flag);
    void addTextLine(string textToBeAdded);
    //bool searchKeyword(vector<string> searchKey);
  };
}

