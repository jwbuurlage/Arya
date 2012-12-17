#include "../include/GameSessionInput.h"
#include "../include/GameSession.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/Game.h"
#include "../include/Events.h"
#include "common/Logger.h"

GameSessionInput::GameSessionInput(GameSession* ses)
{
    session = ses;

    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    rotatingRight = rotatingLeft = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    slowMode = false;
    leftShiftPressed = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
    specPos = vec3(0.0f, 150.0f, 0.0f);
    originalMousePos = vec2(0.0);

    doUnitMovementNextFrame = false;
    doUnitSelectionNextFrame = false;
}

GameSessionInput::~GameSessionInput()
{
    Root::shared().getOverlay()->removeRect(&selectionRect); 
}

void GameSessionInput::init()
{
    selectionRect.fillColor = vec4(1.0, 1.0, 1.0, 0.2);
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
            cam->rotateCamera(90.0f * elapsedTime, 0.0f);
        else if( rotatingRight && !rotatingLeft )
            cam->rotateCamera(-90.0f * elapsedTime, 0.0f);

        vec3 force = forceDirection;
        force = glm::rotateY(force, cam->getYaw());
        float speedFactor = 4000.0f;
        if(slowMode) speedFactor = 500.0f;
        specMovement += force * speedFactor * elapsedTime;
    }

    specPos += specMovement * elapsedTime;

    if(doUnitMovementNextFrame)
    {
        doUnitMovementNextFrame = false;
        moveSelectedUnits();
    }

    if(doUnitSelectionNextFrame)
    {
        doUnitSelectionNextFrame = false;
        selectUnit();
    }

    return;
}

bool GameSessionInput::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    bool DirectionChanged = false;

    switch(key) {
        case GLFW_KEY_LSHIFT: leftShiftPressed = keyDown; break;
        case GLFW_KEY_RSHIFT: slowMode = keyDown; break;
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
            if(originalMousePos == vec2(x, y))
            {
                doUnitSelectionNextFrame = true;
            }
            else
            {
                selectUnits(-1.0 + 2.0 * selectionRect.offsetInPixels.x / Root::shared().getWindowWidth(), 
                        -1.0 + 2.0 * (selectionRect.offsetInPixels.x + selectionRect.sizeInPixels.x) / Root::shared().getWindowWidth(),
                        -1.0 + 2.0 * selectionRect.offsetInPixels.y / Root::shared().getWindowHeight(), 
                        -1.0 + 2.0 * (selectionRect.offsetInPixels.y + selectionRect.sizeInPixels.y) / Root::shared().getWindowHeight());

                selectionRect.offsetInPixels = vec2(0.0);
                selectionRect.sizeInPixels = vec2(0.0);
            }
        }
    }
    else if(button == Arya::BUTTON_RIGHT)
    {
        draggingRightMouse = (buttonDown == true);

        doUnitMovementNextFrame = true;
    }

    return false;
}

bool GameSessionInput::mouseWheelMoved(int delta)
{
    if(delta > 0) {
        Camera* cam = Root::shared().getScene()->getCamera();
        if(cam) cam->camZoomSpeed -= 50.0f;
    } else {
        Camera* cam = Root::shared().getScene()->getCamera();
        if(cam) cam->camZoomSpeed += 50.0f;
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
            selectionRect.offsetInPixels.x = x;
            selectionRect.offsetInPixels.y = originalMousePos.y;
            selectionRect.sizeInPixels.x = originalMousePos.x - x;
            selectionRect.sizeInPixels.y = y - originalMousePos.y;
        }
        else if(deltaX > 0 && deltaY < 0) {
            selectionRect.offsetInPixels.x = originalMousePos.x;
            selectionRect.offsetInPixels.y = y;
            selectionRect.sizeInPixels.x = x - originalMousePos.x;
            selectionRect.sizeInPixels.y = originalMousePos.y - y;
        }
        else if(deltaX > 0 && deltaY > 0) {
            selectionRect.offsetInPixels.x = originalMousePos.x;
            selectionRect.offsetInPixels.y = originalMousePos.y;
            selectionRect.sizeInPixels.x = x - originalMousePos.x;
            selectionRect.sizeInPixels.y = y - originalMousePos.y;
        }
        else if(deltaX < 0 && deltaY < 0) {
            selectionRect.offsetInPixels.x = x;
            selectionRect.offsetInPixels.y = y;
            selectionRect.sizeInPixels.x = originalMousePos.x - x;
            selectionRect.sizeInPixels.y = originalMousePos.y - y;
        }
    }

    return handled;
}

void GameSessionInput::unselectAll()
{
    Faction* lf = session->getLocalFaction();
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        (*it)->setSelected(false);
}

void GameSessionInput::selectAll()
{
    Faction* lf = session->getLocalFaction();
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        (*it)->setSelected(true);
}

void GameSessionInput::selectUnits(float x_min, float x_max, float y_min, float y_max)
{
    if(!leftShiftPressed)
        unselectAll();

    mat4 vpMatrix = Root::shared().getScene()->getCamera()->getVPMatrix();

    Faction* lf = session->getLocalFaction();

    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
    {
        vec2 onScreen = (*it)->getScreenPosition();

        if((onScreen.x > x_min && onScreen.x < x_max) && (onScreen.y > y_min && onScreen.y < y_max)) {
            (*it)->setSelected(true);
        }
    }
}

void GameSessionInput::moveSelectedUnits()
{
    vec3 clickPos = Root::shared().getDepthResult();

    Faction* lf = session->getLocalFaction();
    if(!lf) return;

    // did we click on an enemy unit
    Unit* best_unit = 0;
    float best_distance = 100.0;
    Faction* from_faction = 0;


    float dist;
    for(int j = 0; j < session->getFactions().size(); ++j) {
        if(session->getFactions()[j] == lf) continue;

        for(list<Unit*>::iterator it = session->getFactions()[j]->getUnits().begin();
                it != session->getFactions()[j]->getUnits().end(); ++it)
        {
            dist = glm::distance((*it)->getPosition(), clickPos);
            if(dist < 2.0 * (*it)->getInfo()->radius
                    && dist < best_distance) {
                best_distance = dist; 
                best_unit = (*it);
                from_faction = session->getFactions()[j];
            }
        }
    }

    if(best_unit)
    {
        for(list<Unit*>::iterator it = lf->getUnits().begin();
                it != lf->getUnits().end(); ++it)
            if((*it)->isSelected())
                (*it)->setTargetUnit(best_unit);

        return;
    }

    int numSelected = 0;
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        if((*it)->isSelected())
            ++numSelected;

    int perRow = (int)(glm::sqrt((float)numSelected));
    int currentIndex = 0;
    float spread = 10.0f;

    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        if((*it)->isSelected()) {
            (*it)->setTargetPosition(vec2(clickPos.x + spread*((currentIndex % perRow) - perRow / 2), 
                        clickPos.z + spread*(currentIndex / perRow - perRow / 2)));
            ++currentIndex;
        }
}

void GameSessionInput::selectUnit()
{
    if(!leftShiftPressed)
        unselectAll();

    vec3 clickPos = Root::shared().getDepthResult();

    Faction* lf = session->getLocalFaction();
    if(!lf) return;

    Unit* best_unit = 0;
    float best_distance = 100.0;

    int numSelected = 0;
    float dist;

    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
    {
        dist = glm::distance((*it)->getPosition(), clickPos);
        if(dist < 2.0 * (*it)->getInfo()->radius
                && dist < best_distance) {
            best_distance = dist; 
            best_unit = (*it);
        }
    }

    if(best_unit)
        best_unit->setSelected(true);
}
