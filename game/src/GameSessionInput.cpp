#include "../include/GameSessionInput.h"
#include "../include/GameSession.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "common/Logger.h"

GameSessionInput::GameSessionInput(GameSession* ses)
{
    session = ses;

    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    rotatingRight = rotatingLeft = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    slowMode = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
    specPos = vec3(0.0f, 150.0f, 0.0f);
    originalMousePos = vec2(0.0);

    doUnitMovementNextFrame = false;
}

GameSessionInput::~GameSessionInput()
{
    Root::shared().getOverlay()->removeRect(&selectionRect); 
}

void GameSessionInput::init()
{
    Root::shared().getOverlay()->addRect(&selectionRect); 
}

void GameSessionInput::onFrame(float elapsedTime)
{
    Camera* cam = Root::shared().getScene()->getCamera();

    specMovement*=pow(.002f,elapsedTime);

    if(cam != 0)
    {
        cam->setTargetLocation(specPos, false);

        if( rotatingLeft && !rotatingRight )
            cam->rotateCamera( 90.0f * elapsedTime , 0.0f );
        else if( rotatingRight && !rotatingLeft )
            cam->rotateCamera( -90.0f * elapsedTime , 0.0f );

        vec3 force = forceDirection;
        force = glm::rotateY(force, cam->getYaw());
        float speedFactor=4000.0f;
        if(slowMode) speedFactor=500.0f;
        specMovement += force * speedFactor * elapsedTime;
    }

    specPos += specMovement * elapsedTime;

    if(doUnitMovementNextFrame)
    {
        doUnitMovementNextFrame = false;
        moveSelectedUnits();
    }
    return;
}

bool GameSessionInput::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    bool DirectionChanged = false;

    switch(key) {
        case GLFW_KEY_LSHIFT: slowMode = keyDown; break;
        case 'W': goingForward = keyDown;	DirectionChanged = true; break;
        case 'S': goingBackward = keyDown;	DirectionChanged = true; break;
        case 'Q': rotatingLeft = keyDown;	break;
        case 'E': rotatingRight = keyDown;	break;
        case 'A': goingLeft = keyDown;		DirectionChanged = true; break;
        case 'D': goingRight = keyDown;		DirectionChanged = true; break;
        case 'Z': goingDown = keyDown;		DirectionChanged = true; break;
        case 'X': goingUp = keyDown;		DirectionChanged = true; break;
        default: keyHandled = false; break;
    }

    if( DirectionChanged ){
        forceDirection = vec3(0.0f);
        if(goingForward || mouseTop)    forceDirection.z -= 1.0f;
        if(goingBackward || mouseBot)   forceDirection.z += 1.0f;
        if(goingLeft || mouseLeft)      forceDirection.x -= 1.0f;
        if(goingRight || mouseRight)    forceDirection.x  = 1.0f;
        if(goingUp)         forceDirection.y  = 2.0f;
        if(goingDown)       forceDirection.y -= 2.0f;
    }
    return keyHandled;
}

bool GameSessionInput::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    //TODO: Send to UI manager and see if it was handled. If not, then do this:
    if(button == Arya::BUTTON_LEFT)
    {
        draggingLeftMouse = (buttonDown == true);

        if(draggingLeftMouse)
        {
            originalMousePos = vec2(x, y);
        }
        else
        {
            selectUnits(-1.0 + 2.0 * selectionRect.pixelOffset.x / Root::shared().getWindowWidth(), 
                    -1.0 + 2.0 * (selectionRect.pixelOffset.x + selectionRect.pixelSize.x) / Root::shared().getWindowWidth(),
                    -1.0 + 2.0 * selectionRect.pixelOffset.y / Root::shared().getWindowHeight(), 
                    -1.0 + 2.0 * (selectionRect.pixelOffset.y + selectionRect.pixelSize.y) / Root::shared().getWindowHeight());

            selectionRect.pixelOffset = vec2(0.0);
            selectionRect.pixelSize = vec2(0.0);
        }
    }
    else if(button == Arya::BUTTON_RIGHT)
    {
        draggingRightMouse = (buttonDown == true);

        Root::shared().readDepthNextFrame(x, y);
        doUnitMovementNextFrame = true;
    }

    return false;
}

bool GameSessionInput::mouseWheelMoved(int delta)
{
    if( delta > 0 ){
        Camera* cam = Root::shared().getScene()->getCamera();
        if( cam ) cam->camZoomSpeed -= 50.0f;
    }else{
        Camera* cam = Root::shared().getScene()->getCamera();
        if( cam ) cam->camZoomSpeed += 50.0f;
    }
    return false;
}

bool GameSessionInput::mouseMoved(int x, int y, int dx, int dy)
{
    bool handled = false;
    int padding = 10;

    if(x < padding)
        mouseLeft = true, handled = true;
    else
        mouseLeft = false;

    if(y < padding)
        mouseBot = true, handled = true;
    else
        mouseBot = false;

    if(x > Root::shared().getWindowWidth() - padding)
        mouseRight = true, handled = true;
    else
        mouseRight = false;

    if(y > Root::shared().getWindowHeight() - padding)
        mouseTop = true, handled = true;
    else
        mouseTop = false;

    forceDirection = vec3(0.0f);
    if(goingForward || mouseTop)    forceDirection.z -= 1.0f;
    if(goingBackward || mouseBot)   forceDirection.z += 1.0f;
    if(goingLeft || mouseLeft)      forceDirection.x -= 1.0f;
    if(goingRight || mouseRight)    forceDirection.x  = 1.0f;
    if(goingUp)         forceDirection.y  = 1.0f;
    if(goingDown)       forceDirection.y -= 1.0f;
    if(forceDirection != vec3(0.0f))
        forceDirection = glm::normalize(forceDirection);

    if(draggingLeftMouse)
    {
        int deltaX = x - originalMousePos.x;
        int deltaY = y - originalMousePos.y;

        if(deltaX < 0 && deltaY > 0) {
            selectionRect.pixelOffset.x = x;
            selectionRect.pixelOffset.y = originalMousePos.y;
            selectionRect.pixelSize.x = originalMousePos.x - x;
            selectionRect.pixelSize.y = y - originalMousePos.y;
        }
        else if(deltaX > 0 && deltaY < 0) {
            selectionRect.pixelOffset.x = originalMousePos.x;
            selectionRect.pixelOffset.y = y;
            selectionRect.pixelSize.x = x - originalMousePos.x;
            selectionRect.pixelSize.y = originalMousePos.y - y;
        }
        else if(deltaX > 0 && deltaY > 0) {
            selectionRect.pixelOffset.x = originalMousePos.x;
            selectionRect.pixelOffset.y = originalMousePos.y;
            selectionRect.pixelSize.x = x - originalMousePos.x;
            selectionRect.pixelSize.y = y - originalMousePos.y;
        }
        else if(deltaX < 0 && deltaY < 0) {
            selectionRect.pixelOffset.x = x;
            selectionRect.pixelOffset.y = y;
            selectionRect.pixelSize.x = originalMousePos.x - x;
            selectionRect.pixelSize.y = originalMousePos.y - y;
        }
    }

    return handled;
}

void GameSessionInput::unselectAll()
{
    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(int i = 0; i < lf->getUnits().size(); ++i)
        lf->getUnits()[i]->setSelected(false);
}

void GameSessionInput::selectAll()
{
    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(int i = 0; i < lf->getUnits().size(); ++i)
        lf->getUnits()[i]->setSelected(true);
}

void GameSessionInput::selectUnits(float x_min, float x_max, float y_min, float y_max)
{
    unselectAll();

    mat4 vpMatrix = Root::shared().getScene()->getCamera()->getVPMatrix();

    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(int i = 0; i < lf->getUnits().size(); ++i)
    {
        vec4 onScreen(lf->getUnits()[i]->getObject()->getPosition(), 1.0);
        onScreen = vpMatrix * onScreen;
        onScreen.x /= onScreen.w;
        onScreen.y /= onScreen.w;

        if((onScreen.x > x_min && onScreen.x < x_max) && (onScreen.y > y_min && onScreen.y < y_max)) {
            lf->getUnits()[i]->setSelected(true);
        }
    }
}

void GameSessionInput::moveSelectedUnits()
{
    vec3 clickPos = Root::shared().getDepthResult();

    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(int i = 0; i < lf->getUnits().size(); ++i)
    {
        if(lf->getUnits()[i]->isSelected())
            lf->getUnits()[i]->setTargetPosition(vec2(clickPos.x, clickPos.z));
    }
}
