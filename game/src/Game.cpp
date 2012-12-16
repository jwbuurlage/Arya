#include "../include/Game.h"
#include "Console.h"
#include "../include/GameSession.h"

#include "common/Logger.h"

Game::Game()
{
    root = 0;
    session = 0;
}

Game::~Game()
{
    if(session) delete session;
    if(root) delete &Root::shared();
}

void Game::run()
{
    root = new Root;

    if(!(root->init(true, 800, 600))) {
        LOG_ERROR("Unable to init root");
    }
    else
    {
        root->addInputListener(this);

        if(session) delete session;
        session = new GameSession;
        if(!session->init()) {
            LOG_ERROR("Could not start a new session");
            Root::shared().stopRendering();
        }

        root->startRendering();
    }
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;
    if(Arya::Console::shared().visibility == false)
    {
      switch(key) {
          case 'P':
              if(keyDown) {
                  if(session) delete session;
                  session = new GameSession;
                  if(!session->init()) {
                      LOG_ERROR("Could not start a new session");
                      Root::shared().stopRendering();
                  }
              }
              break;

          case 'L':
              if(keyDown)
              {
                  if(session) delete session;
                  session = 0;
              }
              break;

          case 'O': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
          case 'I': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
          case GLFW_KEY_F6: if(keyDown) Arya::Console::shared().toggleVisbilityConsole(); break;
          case GLFW_KEY_F11: Root::shared().setFullscreen(!Root::shared().getFullscreen()); break;
          case GLFW_KEY_ESC: Root::shared().stopRendering(); break;
          default: keyHandled = false; break;
      }
    }
    else
    {
      if(key >= 'A' && key <= 'Z' )
      {
        if(keyDown) Arya::Console::shared().currentLine.push_back(key);
      }
      else
      {
        switch(key)
        {
          case GLFW_KEY_F6: if(keyDown) Arya::Console::shared().toggleVisbilityConsole(); break;
          case GLFW_KEY_F11: Root::shared().setFullscreen(!Root::shared().getFullscreen()); break;
          case GLFW_KEY_ESC: Root::shared().stopRendering(); break;
          case GLFW_KEY_BACKSPACE: if(keyDown)
          {
            Arya::Console::shared().currentLine.erase(Arya::Console::shared().currentLine.end() - 1);
          }break;
          case GLFW_KEY_SPACE: if(keyDown) Arya::Console::shared().currentLine.push_back(key); break;
          case GLFW_KEY_ENTER: if(keyDown) Arya::Console::shared().enterInput(); break;
          default: keyHandled = false; break;}
        }
      return keyHandled;
      }
}

bool Game::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    return false;
}

bool Game::mouseWheelMoved(int delta)
{
    return false;
}

bool Game::mouseMoved(int x, int y, int dx, int dy)
{
    return false; 
}

