#include "Arya.h"
#include "../include/GameSession.h"

GameSession::GameSession()
{
    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
	slowMode = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
    specPos = vec3(0.0f);
    originalMousePos = vec2(0.0);
}

GameSession::~GameSession()
{
    Root::shared().removeInputListener(this);
    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();
    Root::shared().getOverlay()->removeRect(&selectionRect); 

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    Root::shared().addInputListener(this);
    Root::shared().addFrameListener(this); 

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;
    Object* obj;

    obj = scene->createObject();
    obj->setModel(ModelManager::shared().getModel("triangle"));
    obj->setPosition(vec3(0, 0, 0));

    obj = scene->createObject();
    obj->setModel(ModelManager::shared().getModel("quad"));
    obj->setPosition(vec3(0, 0, 5));

    vector<Texture*> tileSet;
    tileSet.push_back(TextureManager::shared().getTexture("grass.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("rock.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("dirt.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("snow.tga"));
    if(!scene->setMap("heightmap.raw", tileSet, TextureManager::shared().getTexture("splatmap.tga")))
        return false;

    Root::shared().getOverlay()->addRect(&selectionRect);

    return true;
}

void GameSession::onFrame(float elapsedTime)
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
    return;
}

bool GameSession::keyDown(int key, bool keyDown)
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

bool GameSession::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    //TODO: Send to UI manager and see if it was handled. If not, then do this:
    if( button == Arya::BUTTON_LEFT ){
        draggingLeftMouse = (buttonDown == true);
    }else if( button == Arya::BUTTON_RIGHT ){
        draggingRightMouse = (buttonDown == true);
    }

    if(draggingLeftMouse)
    {
        originalMousePos = vec2(x, y);
    }
    else
    {
        selectionRect.pixelOffset = vec2(0.0);
        selectionRect.pixelSize = vec2(0.0);
    }

    return false;
}

bool GameSession::mouseWheelMoved(int delta)
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

bool GameSession::mouseMoved(int x, int y, int dx, int dy)
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
            selectionRect.pixelOffset.x = x;
            selectionRect.pixelOffset.y = originalMousePos.y;
            selectionRect.pixelSize.x = originalMousePos.x - x;
            selectionRect.pixelSize.y = y - originalMousePos.y;
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
