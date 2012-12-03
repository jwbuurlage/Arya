#include "../include/Game.h"
#include "common/Logger.h"

Game::Game()
{
    root = 0;
    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
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

    if(!(root->init(false))) {
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
        vec3 frictionVec( -3.0f * specMovement );
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
            cam->rotateCamera( 10.0f * elapsedTime , 0.0f );
        else if( rotatingRight && !rotatingLeft )
            cam->rotateCamera( -10.0f * elapsedTime , 0.0f );

        vec3 force(forceDirection);
        force = glm::rotateY(force, cam->getYaw());
        specMovement += force * 10.0f * elapsedTime;
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
        case 'A': rotatingLeft = keyDown;	break;
        case 'D': rotatingRight = keyDown;	break;
        case 'Q': goingLeft = keyDown;		DirectionChanged = true; break;
        case 'E': goingRight = keyDown;		DirectionChanged = true; break;
        case 'Z': goingDown = keyDown;		DirectionChanged = true; break;
        case 'X': goingUp = keyDown;		DirectionChanged = true; break;
        case GLFW_KEY_F11: root->setFullscreen(!root->getFullscreen()); break;
        case GLFW_KEY_ESC: root->stopRendering(); break;
        default: keyHandled = false; break;
    }

    if( DirectionChanged ){
        forceDirection.x =  forceDirection.y = forceDirection.z = 0.0f;
        if(goingForward)    forceDirection.z -= 1.0f;
        if(goingBackward)   forceDirection.z += 1.0f;
        if(goingLeft)       forceDirection.x -= 1.0f;
        if(goingRight)      forceDirection.x  = 1.0f;
        if(goingUp)         forceDirection.y  = 1.0f;
        if(goingDown)       forceDirection.y -= 1.0f;
        if(forceDirection != vec3(0.0f))
            forceDirection = glm::normalize(forceDirection);
    }
    return keyHandled;
}
