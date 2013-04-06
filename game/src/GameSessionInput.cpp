#include "../include/GameSessionInput.h"
#include "../include/ClientGameSession.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/Game.h"
#include "../include/Events.h"
#include "../include/common/GameLogger.h"

// for sprintf
#include <stdio.h>

GameSessionInput::GameSessionInput(ClientGameSession* ses)
{
    session = ses;

    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    rotatingRight = rotatingLeft = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    slowMode = false;
    leftShiftPressed = false;
	leftControlPressed = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
	specPos = vec3(0.0f,150.0f,0.0f);
    originalMousePos = vec2(0.0);

    doUnitMovementNextFrame = false;
    doUnitSelectionNextFrame = false;

	unitWindow = 0;
	damageLabel = 0;
	healthLabel = 0;

    selectionRect = Root::shared().getOverlay()->createRect();
}

GameSessionInput::~GameSessionInput()
{
    Root::shared().getOverlay()->removeRect(selectionRect);
}

void GameSessionInput::init()
{
    selectionRect->fillColor = vec4(0.5, 1.0, 0.5, 0.1);

	initUnitInfoWindow();
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

        if(key == GLFW_KEY_LSHIFT) leftShiftPressed = keyDown;
		else if(key == GLFW_KEY_LCTRL) leftControlPressed = keyDown;
        else if(key == GLFW_KEY_RSHIFT) slowMode = keyDown;
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
				updateUnitInfoWindow(*it);
			}
            firstUnitSelected = true;
			if(leftControlPressed) (*it)->getDebugText();
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
    //Faction* from_faction = 0;

    float dist;
    for(unsigned int j = 0; j < session->getFactions().size(); ++j) {
        if(session->getFactions()[j] == lf) continue;

        for(list<Unit*>::iterator it = session->getFactions()[j]->getUnits().begin();
                it != session->getFactions()[j]->getUnits().end(); ++it)
        {
            dist = glm::distance((*it)->getPosition(), clickPos);
            if(dist < 2.0 * (*it)->getInfo()->radius
                    && dist < best_distance) {
                best_distance = dist; 
                best_unit = (*it);
            }
        }
    }

    vec2 centerPos(0,0);
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

	//From centerPos to clickPos
	vec2 target(clickPos.x, clickPos.z);
	if(best_unit) target = best_unit->getPosition2();
	std::vector<vec2> centerNodes;
	findPath(centerPos,target,centerNodes);
	for(list<Unit*>::iterator it = lf->getUnits().begin(); it != lf->getUnits().end(); ++it)
		if((*it)->isSelected())
		{
			(*it)->getPathNodes().clear();
			for(int i = 0; i < centerNodes.size(); i++)
			{
				(*it)->getPathNodes().push_back((*it)->getPosition2() - centerPos + centerNodes[i]);
			}
		}
	
	vec2 direction = target - centerPos;
	if(best_unit && glm::length(direction) < 30)
	{
		Event& ev = Game::shared().getEventManager()->createEvent(EVENT_ATTACK_MOVE_UNIT_REQUEST);

		ev << numSelected;
		for(unsigned int i = 0; i < unitIds.size(); ++i)
			ev << unitIds[i] << best_unit->getId();

		ev.send();
	}
	if((!best_unit && glm::length(direction) > 0.01) || (best_unit && glm::length(direction) > 30))
	{
		direction = glm::normalize(direction);
		vec2 perpendicular(-direction.y, direction.x); //right hand rule

		//TODO: Bipartite matching that also takes into account collissions when units want to change their relative positions
		//For now the algorithm is greedy: starts at the point the furthest away and takes the nearest unit to that point

		float spread = 20.0f;
		int perRow = (int)(glm::sqrt((float)numSelected) + 0.99);

		direction *= spread;
		perpendicular *= spread;

		Event& ev = Game::shared().getEventManager()->createEvent(EVENT_MOVE_UNIT_REQUEST);
		ev << numSelected;
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
			for(unsigned int j = 0; j < unitIds.size(); ++j)
			{
				float dist = glm::distance(unitPositions[j],targetSpot);
				if(bestIndex == -1 || dist < bestDistance)
				{
					bestIndex = j;
					bestDistance = dist;
				}
			}
			ev << unitIds[bestIndex] << targetSpot;
			//Remove the unit from the list. This is how we mark it as used.
			//The list is not needed after this
			unitIds.erase(unitIds.begin()+bestIndex);
			unitPositions.erase(unitPositions.begin()+bestIndex);
		}
		ev.send();
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
    {
        SoundManager::shared().play(best_unit->getInfo()->selectionSound);
        best_unit->setSelected(true);
		if(leftControlPressed) best_unit->getDebugText();
    }
}


//////////////////////////
// Unit window
//////////////////////////

void GameSessionInput::initUnitInfoWindow()
{
	vec2 windowSize = vec2(150.0f, 80.0f);
	unitWindow = new Arya::Window(vec2(1.0f, -1.0f), vec2(-20.0f -windowSize.x, 20.0f), windowSize, 
			TextureManager::shared().getTexture("white"), Arya::WINDOW_DRAGGABLE, "Unit Info",
			vec4(0.0f, 0.0f, 0.0f, 0.6f));

	Arya::Font* f = Arya::FontManager::shared().getFont("DejaVuSans-Bold.ttf");
	healthLabel = new Arya::Label(vec2(-1.0f, 1.0f), vec2(20.0f, -30.0f), f, "Health: lalalal");
	unitWindow->addChild(healthLabel);

	damageLabel = new Arya::Label(vec2(-1.0f, 1.0f), vec2(20.0f, -50.0f), f, "Name: lalalal");
	unitWindow->addChild(damageLabel);

	Arya::Interface::shared().makeActive(unitWindow);
}

void GameSessionInput::updateUnitInfoWindow(Unit* unit)
{
	if(!damageLabel || !healthLabel)
	{
		GAME_LOG_WARNING("Updating uninitialized unit info window");
		return;
	}

	char buffer [50];
    sprintf (buffer, "Max Health: %.2f", unit->getInfo()->maxHealth);
	healthLabel->setText(string(buffer));
    sprintf (buffer, "Damage: %.2f", unit->getInfo()->damage);
	damageLabel->setText(string(buffer));
}
