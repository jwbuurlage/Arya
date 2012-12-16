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
    bool visibility;
    string currentLine;

    void toggleVisbilityConsole();
    void enterInput();

    private:
    void consoleInfo();
    int nrLines;
    int searchNrLines;
    int textWidthInPixels;
    int nrCharOnLine;
    int pixelsInBetween;
    int activeLine;
    vector<string> history;
    vector<string> searchHistory;
    vector<Rect*> rects;
    Font* font;
    float time;

    void setVisibilityConsole(bool flag);
    void addTextLine(string textToBeAdded);
    //bool searchKeyword(vector<string> searchKey);
  };
}

