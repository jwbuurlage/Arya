#pragma once
#include "common/Listeners.h"
#include "common/Singleton.h"
#include <vector>
#include <string>
#include <map>
#include "Overlay.h"
#include "Fonts.h"

using std::vector;
using std::string;
using std::multimap;
using std::pair;

namespace Arya
{
	class Window;
	class Label;

    class Console : public Singleton<Console>, public FrameListener, public InputListener, public CommandListener
    {
        public:
            //!! CommandHandler must be created before Console
            Console();
            virtual ~Console();

            bool isInitialized() const { return initialized; }
            void cleanup();
            void onFrame(float elapsedTime);
            bool keyDown(int key, bool keyDown);
            bool init();
            bool visibility; //visibility of the kernel
            string currentLine;

            void updateCurrentLine();
            void toggleVisibilityConsole();
            void enterInput();
            void addOutputText(string textToBeAdded);
            bool changeConsoleColor(float r, float g, float b);

        private:
            bool initialized;

            int nrLines; //visible number of lines of console
            int searchNrLines; //number of lines in which you can search, if more the first one will be kicked
            int textWidthInPixels; //width of character
            int pixelsInBetween; // pixels in between lines
            bool lShift; // used for key input
            bool rShift; // used for key input
            bool rAlt; // used for key input
            bool cLock; // used for key input
            vector<string> history;
            vector<string> searchHistory;
            Font* font;
            int upCount; // counts how many times we pressed up before pressing enter
            vec4 consoleColor;
            void goBackInHistory();
            void addTextLine(string textToBeAdded);
			bool handleCommand(string command);
			Window* consoleWindow;
			vector<Label*> lines;
			void addLine(string text);
    };
}
