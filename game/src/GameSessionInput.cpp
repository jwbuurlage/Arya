#include "../include/GameSessionInput.h"
#include "../include/ClientGameSession.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/Game.h"
#include "../include/Events.h"
#include "../include/common/GameLogger.h"
#include <GLFW/glfw3.h>

GameSessionInput::GameSessionInput(ClientGameSession* ses)
{
    session = ses;

    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    rotatingRight = rotatingLeft = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    slowMode = false;
    awaitingPlacement = false;
    leftShiftPressed = false;
	leftControlPressed = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
	specPos = vec3(0.0f,150.0f,0.0f);
    originalMousePos = vec2(0.0);

    unitMovementNextFrame = false;
    unitSelectionNextFrame = false;
    buildingPlacementNextFrame = false;

    selectionRect = Root::shared().getOverlay()->createRect();
}

GameSessionInput::~GameSessionInput()
{
    Root::shared().getOverlay()->removeRect(selectionRect);
}

void GameSessionInput::init()
{
    selectionRect->fillColor = vec4(0.5, 1.0, 0.5, 0.1);
}

void GameSessionInput::setSpecPos(vec3 pos)
{
	specPos = pos; 
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
        if(slowMode) speedFactor *= 0.125f;
		float zoomSpeedFactor = cam->getZoom()/300.0f;
        specMovement += force * speedFactor * zoomSpeedFactor * elapsedTime;
    }

    specPos += specMovement * elapsedTime;

    if(unitMovementNextFrame)
        moveSelectedUnits();
    unitMovementNextFrame = false;

    if(unitSelectionNextFrame)
        selectUnit();
    unitSelectionNextFrame = false;

    if(buildingPlacementNextFrame)
        placeBuilding();
    buildingPlacementNextFrame = false;

    return;
}

bool GameSessionInput::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    bool DirectionChanged = false;

        if(key == GLFW_KEY_LEFT_SHIFT) leftShiftPressed = keyDown;
		else if(key == GLFW_KEY_LEFT_CONTROL) leftControlPressed = keyDown;
        else if(key == GLFW_KEY_RIGHT_SHIFT) slowMode = keyDown;
		else if(key == Config::shared().getCvarString("goingforwardgame")[0])
		{
			goingForward = keyDown;
			DirectionChanged = true;
		}
        else if(key == Config::shared().getCvarString("goingbackwardgame")[0])
		{
			goingBackward = keyDown;
			DirectionChanged = true;
		}
		else if(key == Config::shared().getCvarString("rotatingleftgame")[0])
		{
        	 rotatingLeft = keyDown;
		}
		else if(key == Config::shared().getCvarString("rotatingrightgame")[0])
		{
			rotatingRight = keyDown;
		}
		else if(key == Config::shared().getCvarString("goingleftgame")[0])
		{
			goingLeft = keyDown;
			DirectionChanged = true;
		}
		else if(key == Config::shared().getCvarString("goingrightgame")[0])
		{
			goingRight = keyDown;
			DirectionChanged = true;
		}
		else if(key == Config::shared().getCvarString("goingdowngame")[0])
		{
			goingDown = keyDown;
			DirectionChanged = true;
		}
		else if(key == Config::shared().getCvarString("goingupgame")[0])
		{
			goingUp = keyDown;
			DirectionChanged = true;
		}		
		else if(key == Config::shared().getCvarString("synchronizegame")[0])
		{
			Event& ev = Game::shared().getEventManager()->createEvent(EVENT_GAME_FULLSTATE_REQUEST);
			ev.send();
		}
        else if(key == 'B')
        {
            if(!draggingLeftMouse)
            {
                awaitingPlacement = true;
            }
        }
        else keyHandled = false;

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
    if(awaitingPlacement)
    {
        if(button == Arya::BUTTON_LEFT)
        {
            awaitingPlacement = false;
            buildingPlacementNextFrame = true;
            Root::shared().readDepth();
        }
    }
    else
    {
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
                    unitSelectionNextFrame = true;
                    Root::shared().readDepth();
                }
                else
                {
                    selectUnits(-1.0 + 2.0 * selectionRect->offsetInPixels.x / Root::shared().getWindowWidth(), 
                            -1.0 + 2.0 * (selectionRect->offsetInPixels.x + selectionRect->sizeInPixels.x) / Root::shared().getWindowWidth(),
                            -1.0 + 2.0 * selectionRect->offsetInPixels.y / Root::shared().getWindowHeight(), 
                            -1.0 + 2.0 * (selectionRect->offsetInPixels.y + selectionRect->sizeInPixels.y) / Root::shared().getWindowHeight());

                    selectionRect->offsetInPixels = vec2(0.0);
                    selectionRect->sizeInPixels = vec2(0.0);
                }
            }
        }
        else if(button == Arya::BUTTON_RIGHT)
        {
            draggingRightMouse = (buttonDown == true);

            if(!draggingRightMouse)
            {
                unitMovementNextFrame = true;
                Root::shared().readDepth();
            }
        }
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
            selectionRect->offsetInPixels.x = x;
            selectionRect->offsetInPixels.y = originalMousePos.y;
            selectionRect->sizeInPixels.x = originalMousePos.x - x;
            selectionRect->sizeInPixels.y = y - originalMousePos.y;
        }
        else if(deltaX > 0 && deltaY < 0) {
            selectionRect->offsetInPixels.x = originalMousePos.x;
            selectionRect->offsetInPixels.y = y;
            selectionRect->sizeInPixels.x = x - originalMousePos.x;
            selectionRect->sizeInPixels.y = originalMousePos.y - y;
        }
        else if(deltaX > 0 && deltaY > 0) {
            selectionRect->offsetInPixels.x = originalMousePos.x;
            selectionRect->offsetInPixels.y = originalMousePos.y;
            selectionRect->sizeInPixels.x = x - originalMousePos.x;
            selectionRect->sizeInPixels.y = y - originalMousePos.y;
        }
        else if(deltaX < 0 && deltaY < 0) {
            selectionRect->offsetInPixels.x = x;
            selectionRect->offsetInPixels.y = y;
            selectionRect->sizeInPixels.x = originalMousePos.x - x;
            selectionRect->sizeInPixels.y = originalMousePos.y - y;
        }
    }

    return handled;
}

void GameSessionInput::unselectAll()
{
    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        (*it)->setSelected(false);
}

void GameSessionInput::selectAll()
{
    Faction* lf = session->getLocalFaction();
    if(!lf) return;
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
        (*it)->setSelected(true);
}

void GameSessionInput::selectUnits(float x_min, float x_max, float y_min, float y_max)
{
    if(!leftShiftPressed)
        unselectAll();

    Faction* lf = session->getLocalFaction();

    bool firstUnitSelected = false;
    if(!lf) return;
    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
    {
        vec2 onScreen = (*it)->getScreenPosition();

        if((onScreen.x > x_min && onScreen.x < x_max) && (onScreen.y > y_min && onScreen.y < y_max))
	   	{
            (*it)->setSelected(true);
            if(!firstUnitSelected) 
			{
				SoundManager::shared().play((*it)->getInfo()->selectionSound);
			}
            firstUnitSelected = true;
			if(leftControlPressed) (*it)->getDebugText();
        }
    }
}

void GameSessionInput::moveSelectedUnits()
{
    vec3 clickPos = Root::shared().getDepthResult();
    vec2 clickPos2(clickPos.x, clickPos.z);

    Faction* lf = session->getLocalFaction();
    if(!lf) return;

    // did we click on an enemy unit
    Unit* best_unit = 0;
    float best_distance = 100.0;
    //Faction* from_faction = 0;

    float dist;
    for(unsigned int j = 0; j < session->getFactions().size(); ++j) {
        if(session->getFactions()[j] == lf) continue;

        for(list<Unit*>::iterator it = session->getFactions()[j]->getUnits().begin();
                it != session->getFactions()[j]->getUnits().end(); ++it)
        {
            dist = glm::distance((*it)->getPosition2(), clickPos2);
            if(dist < (*it)->getInfo()->radius && dist < best_distance)
            {
                best_distance = dist; 
                best_unit = (*it);
            }
        }
    }

    vec2 centerPos(0,0); //average position of selected units
    vector<int> unitIds;
    vector<vec2> unitPositions;
    for(list<Unit*>::iterator it = lf->getUnits().begin(); it != lf->getUnits().end(); ++it)
        if((*it)->isSelected())
        {
            centerPos += (*it)->getPosition2();
            unitPositions.push_back((*it)->getPosition2());
            unitIds.push_back((*it)->getId());
        }

    int numSelected = unitIds.size();

    if(!numSelected) return;

    centerPos /= (float)numSelected;

    //FOR NOW: we only use pathfinding for normal walking, not for attacking
	if(best_unit)
	{
		Event& ev = Game::shared().getEventManager()->createEvent(EVENT_ATTACK_MOVE_UNIT_REQUEST);

		ev << numSelected;
		for(unsigned int i = 0; i < unitIds.size(); ++i)
			ev << unitIds[i] << best_unit->getId();

		ev.send();
	}
    else
    {
        //Movement from centerPos to clickPos
        vec2 target(clickPos.x, clickPos.z);

        //Find a path
        std::vector<vec2> pathNodes;
        //session->findPath(centerPos,target,pathNodes);
        if(pathNodes.empty()) pathNodes.push_back(target);

        //Now calculate the position of each unit relative to each other
        vector<vec2> relativePositions(unitIds.size());

        vec2 direction = glm::normalize(target - centerPos);
		vec2 perpendicular(-direction.y, direction.x); //right hand rule

		float spread = 20.0f;
		int perRow = (int)(glm::sqrt((float)numSelected) + 0.99);

		direction *= spread;
		perpendicular *= spread;

        vector<bool> unitTaken(unitIds.size(),false);
		for(int i = 0; i < numSelected; ++i)
		{
			//This loops over the target spots in such a way that it first loops the points that are furthest away.
			//When the units are coming from the BOTTOM the order is like this:
			//1 2 3
			//4 5 6
			//7 8 9
			vec2 targetSpot = target + float(perRow/2 - i/perRow)*direction + float(i%perRow - perRow/2)*perpendicular;
			//Select closest unit
			int bestIndex = -1;
			float bestDistance = 0;
			for(int j = 0; j < numSelected; ++j)
			{
                if(unitTaken[j]) continue;
				float dist = glm::distance(unitPositions[j],targetSpot);
				if(bestIndex == -1 || dist < bestDistance)
				{
					bestIndex = j;
					bestDistance = dist;
				}
			}
            relativePositions[bestIndex] = targetSpot - target;
            unitTaken[bestIndex] = true;
		}

        //Relative positions to center have been calculated. Now send the packet
        Event& ev = Game::shared().getEventManager()->createEvent(EVENT_MOVE_UNIT_REQUEST);
        ev << session->getGameTime();
        ev << numSelected;
        for(int i = 0; i < numSelected; ++i)
        {
            ev << unitIds[i];
            ev << (int)pathNodes.size();
            for(unsigned int j = 0; j < pathNodes.size(); ++j)
            {
                ev << pathNodes[j] + relativePositions[i];
            }
        }
		ev.send();
	}
}

void GameSessionInput::selectUnit()
{
    if(!leftShiftPressed)
        unselectAll();

    vec3 clickPos = Root::shared().getDepthResult();
    vec2 clickPos2(clickPos.x, clickPos.z);

    Faction* lf = session->getLocalFaction();
    if(!lf) return;

    Unit* best_unit = 0;
    float best_distance = 100.0;

    float dist;

    for(list<Unit*>::iterator it = lf->getUnits().begin();
            it != lf->getUnits().end(); ++it)
    {
        dist = glm::distance((*it)->getPosition2(), clickPos2);
        if(dist < 2.0 * (*it)->getInfo()->radius
                && dist < best_distance) {
            best_distance = dist; 
            best_unit = (*it);
        }
    }

    if(best_unit)
    {
        SoundManager::shared().play(best_unit->getInfo()->selectionSound);
        best_unit->setSelected(true);
		if(leftControlPressed) best_unit->getDebugText();
    }
}

void GameSessionInput::placeBuilding()
{
    vec3 clickPos = Root::shared().getDepthResult();
    vec2 clickPos2(clickPos.x, clickPos.z);

    Event& ev = Game::shared().getEventManager()->createEvent(EVENT_SPAWN_REQUEST);
    ev << 3; //UnitTypeId
    ev << clickPos2;
    ev.send();
}
