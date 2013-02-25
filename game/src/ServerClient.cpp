#include "../include/common/GameLogger.h"
#include "../include/ServerClient.h"
#include "../include/ServerGameSession.h"
#include "../include/Units.h"

ServerClient::~ServerClient()
{
    if(gameSession) gameSession->removeClient(this);
	//dont remove the faction, it is managed by the session
}
