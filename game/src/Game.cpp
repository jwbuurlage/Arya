#include "../include/Game.h"
#include "common/Logger.h"

Game::Game()
{
    root = 0;
    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
    specPos = vec3(0.0f);
}

Game::~Game()
{

}

void Game::run()
{
    root = new Root;

    if(!(root->init(false, 1378, 1024))) {
        LOG_ERROR("Unable to init root");
    }
    else
    {
        root->addInputListener(this);
        root->addFrameListener(this);
        root->startRendering();
    }

    delete root;
}

void Game::onFrame(float elapsedTime)
{
    Camera* cam = root->getScene()->getCamera();

    //For normal speeds, friction force proportional to the speed
    if(glm::length2(specMovement) > 1.0f) {
        vec3 frictionVec( -5.0f * specMovement );
        specMovement += frictionVec * elapsedTime;
    } else { //For low speeds, constant friction force
        if(specMovement != vec3(0.0f))
        {
            vec3 frictionVec( -3.0f * glm::normalize(specMovement) );
            specMovement += frictionVec * elapsedTime;
        }
    }

    if(cam != 0)
    {
        cam->setTargetLocation(specPos, false);

        if( rotatingLeft && !rotatingRight )
            cam->rotateCamera( 90.0f * elapsedTime , 0.0f );
        else if( rotatingRight && !rotatingLeft )
            cam->rotateCamera( -90.0f * elapsedTime , 0.0f );

        vec3 force = forceDirection;
        force = glm::rotateY(force, cam->getYaw());
        specMovement += force * 2000.0f * elapsedTime;
    }

    specPos += specMovement * elapsedTime;
    return;
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    bool DirectionChanged = false;

    switch(key) {
        case 'W': goingForward = keyDown;	DirectionChanged = true; break;
        case 'S': goingBackward = keyDown;	DirectionChanged = true; break;
        case 'Q': rotatingLeft = keyDown;	break;
        case 'E': rotatingRight = keyDown;	break;
        case 'A': goingLeft = keyDown;		DirectionChanged = true; break;
        case 'D': goingRight = keyDown;		DirectionChanged = true; break;
        case 'Z': goingDown = keyDown;		DirectionChanged = true; break;
        case 'X': goingUp = keyDown;		DirectionChanged = true; break;
        case GLFW_KEY_F11: root->setFullscreen(!root->getFullscreen()); break;
        case GLFW_KEY_ESC: root->stopRendering(); break;
        default: keyHandled = false; break;
    }

    if( DirectionChanged ){
        forceDirection = vec3(0.0f);
        if(goingForward || mouseTop)    forceDirection.z -= 1.0f;
        if(goingBackward || mouseBot)   forceDirection.z += 1.0f;
        if(goingLeft || mouseLeft)      forceDirection.x -= 1.0f;
        if(goingRight || mouseRight)    forceDirection.x  = 1.0f;
        if(goingUp)         forceDirection.y  = 1.0f;
        if(goingDown)       forceDirection.y -= 1.0f;
        if(forceDirection != vec3(0.0f))
            forceDirection = glm::normalize(forceDirection);
    }
    return keyHandled;
}

bool Game::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    //TODO: Send to UI manager and see if it was handled. If not, then do this:
    if( button == Arya::BUTTON_LEFT ){
        draggingLeftMouse = (buttonDown == true);
    }else if( button == Arya::BUTTON_RIGHT ){
        draggingRightMouse = (buttonDown == true);
    }
    return false;
}

bool Game::mouseWheelMoved(int delta)
{
    if( delta > 0 ){
        Camera* cam = root->getScene()->getCamera();
        if( cam ) cam->camZoomSpeed -= 50.0f;
    }else{
        Camera* cam = root->getScene()->getCamera();
        if( cam ) cam->camZoomSpeed += 50.0f;
    }
    return false;
}

bool Game::mouseMoved(int x, int y, int dx, int dy)
{
    bool handled = false;
    int padding = 10;

    if(x < padding)
        mouseLeft = true, handled = true;
    else
        mouseLeft = false;

    if(y < padding)
        mouseTop = true, handled = true;
    else
        mouseTop = false;

    if(x > root->getWindowWidth() - padding)
        mouseRight = true, handled = true;
    else
        mouseRight = false;

    if(y > root->getWindowHeight() - padding)
        mouseBot = true, handled = true;
    else
        mouseBot = false;

    forceDirection = vec3(0.0f);
    if(goingForward || mouseTop)    forceDirection.z -= 1.0f;
    if(goingBackward || mouseBot)   forceDirection.z += 1.0f;
    if(goingLeft || mouseLeft)      forceDirection.x -= 1.0f;
    if(goingRight || mouseRight)    forceDirection.x  = 1.0f;
    if(goingUp)         forceDirection.y  = 1.0f;
    if(goingDown)       forceDirection.y -= 1.0f;
    if(forceDirection != vec3(0.0f))
        forceDirection = glm::normalize(forceDirection);

    return handled; 
}

