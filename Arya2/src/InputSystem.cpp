#include "InputSystem.h"
#include "common/Logger.h"
#include <SDL2/SDL.h>

namespace Arya
{
    InputSystem::InputSystem()
    {
    }

    InputSystem::~InputSystem()
    {
    }

    void InputSystem::bind(INPUTEVENT event, function<void(int)> f)
    {
        if( event == INPUT_KEYDOWN )
            bindingKeyDown = f;
        else if( event == INPUT_KEYUP )
            bindingKeyUp = f;
        else if( event == INPUT_MOUSEWHEEL )
            bindingMouseWheel = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }

    void InputSystem::bind(INPUTEVENT event, function<void(MOUSEBUTTON,int,int)> f)
    {
        if( event == INPUT_MOUSEBUTTONDOWN )
            bindingMouseDown = f;
        else if( event == INPUT_MOUSEBUTTONUP )
            bindingMouseUp = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }


    void InputSystem::bind(INPUTEVENT event, function<void(int,int,int,int)> f)
    {
        if( event == INPUT_MOUSEMOVEMENT )
            bindingMouseMovement = f;
        else
            LogWarning << "Trying to bind event with invalid function signature or invalid event type" << endLog;
    }

    void InputSystem::unbind(INPUTEVENT event)
    {
        switch(event) {
            case INPUT_KEYDOWN:
                bindingKeyDown = nullptr;
                break;
            case INPUT_KEYUP:
                bindingKeyUp = nullptr;
                break;
            case INPUT_MOUSEBUTTONDOWN:
                bindingMouseDown = nullptr;
                break;
            case INPUT_MOUSEBUTTONUP:
                bindingMouseUp = nullptr;
                break;
            case INPUT_MOUSEMOVEMENT:
                bindingMouseMovement = nullptr;
                break;
            case INPUT_MOUSEWHEEL:
                bindingMouseWheel = nullptr;
                break;
            default:
                break;
        }
    }

    MOUSEBUTTON translateButton(Uint8 btn)
    {
        if( btn == SDL_BUTTON_LEFT )   return MOUSEBUTTON_LEFT;
        if( btn == SDL_BUTTON_MIDDLE ) return MOUSEBUTTON_MIDDLE;
        if( btn == SDL_BUTTON_RIGHT )  return MOUSEBUTTON_RIGHT;
        return MOUSEBUTTON_LEFT;
    }

    void InputSystem::handleInputEvent(const SDL_Event& event)
    {
        switch(event.type) {
            case SDL_KEYDOWN:
                if( bindingKeyDown )
                    bindingKeyDown((int)event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                if( bindingKeyUp )
                    bindingKeyUp((int)event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if( bindingMouseDown ) {
                    bindingMouseDown(translateButton(event.button.button),
                            event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if( bindingMouseUp ) {
                    bindingMouseUp(translateButton(event.button.button),
                            event.button.x, event.button.y);
                }
                break;
            case SDL_MOUSEMOTION:
                if( bindingMouseMovement ) {
                    bindingMouseMovement(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                }
                break;
            case SDL_MOUSEWHEEL:
                if( bindingMouseWheel ) {
                    bindingMouseWheel(event.wheel.y);
                }
                break;
            default:
                LogWarning << "Unkown event received in InputSystem" << endLog;
                break;
        }

    }
}
