#include "Arya.h"
#include "../include/common/GameLogger.h"
#include "../include/Game.h"
#include "../include/ClientGameSession.h"
#include "../include/GameSessionInput.h"
#include "../include/Map.h"
#include "../include/MapInfo.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/common/QuadTree.h"
#include "../include/common/Cells.h"

ClientGameSession::ClientGameSession() : GameSession(Game::shared().getScripting(), false)
{
	unitCells = 0;
	input = 0;
	map = 0;
	localFaction = 0;

	decalVao = 0;
	decalProgram = 0;
	selectionDecalHandle = 0;
	pathfindingMap = 0;
}

ClientGameSession::~ClientGameSession()
{
	if(decalProgram)
		delete decalProgram;

	if(unitCells) delete unitCells;

	// TODO: delete vertex buffers

	if(input) {
		Root::shared().removeInputListener(input);
		Root::shared().removeFrameListener(input);
		delete input;
	}

	for(unsigned int i = 0; i < factions.size(); ++i)
		delete factions[i];
	factions.clear();

	if(map) delete map;

	Root::shared().removeFrameListener(this);

	Root::shared().removeScene();

	Game::shared().getEventManager()->removeEventHandler(this);

	if(pathfindingMap) delete pathfindingMap;

	GAME_LOG_INFO("Ended session");
}

bool ClientGameSession::init()
{
	Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_CONNECTED, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_DISCONNECTED, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_GAME_FULLSTATE, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_MOVE_UNIT, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_ATTACK_MOVE_UNIT, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_UNIT_DIED, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_UNIT_SPAWNED, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_PLAYER_DEFEAT, this);
	Game::shared().getEventManager()->addEventHandler(EVENT_PLAYER_VICTORY, this);

	input = new GameSessionInput(this);
	input->init();

	if(!initShaders()) return false;
	if(!initVertices()) return false;

	Root::shared().addInputListener(input);
	Root::shared().addFrameListener(input);
	Root::shared().addFrameListener(this);

	Scene* scene = Root::shared().makeDefaultScene();
	if(!scene)
		return false;

	Camera* cam = scene->getCamera();

	cam->setPosition(vec3(0.0f, 150.0f, 0.0f));
	cam->setCameraAngle(0.0f, -60.0f);
	cam->setZoom(300.0f);

	if(!map) map = new Map(theMap);

	if(!map->initHeightData())
		return false;
	if(!map->initGraphics(scene))
		return false;

	selectionDecalHandle = 0;
	Texture* selectionTex = TextureManager::shared().getTexture("selection.png");
	if(selectionTex) selectionDecalHandle = selectionTex->handle;

	unitCells = new CellList(64, map->getSize());

	initPathfinding();

	return true;
}

bool ClientGameSession::initShaders()
{
	Shader* decalVertex = new Shader(Arya::VERTEX);
	if(!(decalVertex->addSourceFile("../shaders/terraindecal.vert"))) return false;
	if(!(decalVertex->compile())) return false;

	Shader* decalFragment = new Shader(Arya::FRAGMENT);
	if(!(decalFragment->addSourceFile("../shaders/terraindecal.frag"))) return false;
	if(!(decalFragment->compile())) return false;

	decalProgram = new ShaderProgram("terraindecal");
	decalProgram->attach(decalVertex);
	decalProgram->attach(decalFragment);
	if(!(decalProgram->link())) return false;

	return true;
}

bool ClientGameSession::initVertices()
{
	GLfloat vertices[] = {
		0.0, 0.0,
		1.0, 0.0,
		0.0, 1.0,
		1.0, 1.0
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); 

	glGenVertexArrays(1, &decalVao);
	glBindVertexArray(decalVao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);

	glBindVertexArray(0);

	return true;
}

void ClientGameSession::rebuildCellList()
{
	unitCells->clear();
	// loop through units and insert them into cell list
	for(unsigned int i = 0; i < factions.size(); ++i)
	{
		for(list<Unit*>::iterator it = factions[i]->getUnits().begin();
				it != factions[i]->getUnits().end(); ++it)
		{
			(*it)->setCellFromList(unitCells);
		}
	}
}

void ClientGameSession::onFrame(float elapsedTime)
{
	if(!localFaction) return;
	// update units
	mat4 vpMatrix = Root::shared().getScene()->getCamera()->getVPMatrix();
	for(unsigned int i = 0; i < factions.size(); ++i)
	{
		for(list<Unit*>::iterator it = factions[i]->getUnits().begin();
				it != factions[i]->getUnits().end(); )
		{
			if((*it)->readyToDelete())
			{
				(*it)->setCell(0);
				delete *it;
				it = factions[i]->getUnits().erase(it);
			}
			else
			{
				vec4 onScreen((*it)->getObject()->getPosition(), 1.0);
				onScreen = vpMatrix * onScreen;
				onScreen.x /= onScreen.w;
				onScreen.y /= onScreen.w;
				(*it)->setScreenPosition(vec2(onScreen.x, onScreen.y));

				(*it)->update(elapsedTime);

				++it;
			}
		}
	}

	for(list<Unit*>::iterator it = localFaction->getUnits().begin();
			it != localFaction->getUnits().end(); ++it)
	{
		(*it)->checkForEnemies();
	}
}

void ClientGameSession::onRender()
{
	return;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	decalProgram->use();
	glBindVertexArray(decalVao);

	decalProgram->setUniform1f("yOffset", 2.0);
	decalProgram->setUniformMatrix4fv("vpMatrix", Root::shared().getScene()->getCamera()->getVPMatrix());
	decalProgram->setUniformMatrix4fv("scaleMatrix", Root::shared().getScene()->getTerrain()->getScaleMatrix());

	decalProgram->setUniform1i("selectionTexture", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, selectionDecalHandle);

	decalProgram->setUniform3fv("uColor", localFaction->getColor());

	for(list<Unit*>::iterator it = localFaction->getUnits().begin();
			it != localFaction->getUnits().end(); ++it)
	{
		decalProgram->setUniform1f("unitRadius", (*it)->getRadius());
		if(!((*it)->isSelected()))
			continue;

		vec3 groundPos = vec3((*it)->getObject()->getPosition().x,
				map->heightAtGroundPosition((*it)->getObject()->getPosition().x, (*it)->getObject()->getPosition().z),
				(*it)->getObject()->getPosition().z);
		decalProgram->setUniform3fv("groundPosition", groundPos);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void ClientGameSession::handleEvent(Packet& packet)
{
	int id = packet.getId();
	switch(id)
	{
		case EVENT_GAME_FULLSTATE:
			{
				GAME_LOG_DEBUG("Full game state received!");

				int count;
				packet >> count;
				for(int i = 0; i < count; ++i)
				{
					int clientId;
					packet >> clientId;

					int factionId;
					packet >> factionId;

					Faction* faction = getFactionById(factionId);
					if(!faction)
					{
						faction = createFaction(factionId);
						factions.push_back(faction);
					}

					faction->deserialize(packet);
					faction->setClientId(clientId);

					if(clientId == Game::shared().getClientId())
						localFaction = faction;

					//If any of the units that we have was NOT sent in this list they must be deleted
					//So we keep a list of IDs that we have and check which ones are in the packet
					//This method might be a bit slow but this process only happens in rare situations
					//so we do not bother adding extra member variables to the unit class to accomplish this
					vector<int> allIDs;
					for(list<Unit*>::iterator it = factions[i]->getUnits().begin(); it != factions[i]->getUnits().end(); ++it)
						allIDs.push_back((*it)->getId());

					int unitCount;
					packet >> unitCount;
					for(int i = 0; i < unitCount; ++i)
					{
						int id;
						packet >> id;

						for(vector<int>::iterator iter = allIDs.begin(); iter != allIDs.end(); ++iter)
							if( *iter == id ){ allIDs.erase(iter); break; }

						Unit* unit = getUnitById(id);
						bool newUnit = false;
						if(!unit)
						{
							newUnit = true;
							unit = createUnit(id, 0);
						}
						unit->deserialize(packet);

						if(faction == localFaction) unit->setLocal(true);

						Object* obj = unit->getObject();
						if(!obj) obj = Root::shared().getScene()->createObject();

						obj->setModel(ModelManager::shared().getModel(unit->getInfo()->modelname + ".aryamodel"));
						obj->setAnimation("stand");

						unit->setObject(obj);

						float heightModel = map->heightAtGroundPosition(unit->getPosition().x, unit->getPosition().z);

						unit->setPosition(vec3(unit->getPosition().x,
									heightModel,
									unit->getPosition().z));

						if(newUnit) faction->addUnit(unit);
						if(unit->getType() == 2 && faction == localFaction)
						{
							input->setSpecPos(unit->getPosition());
						}

						unit->getInfo()->onSpawn(unit);
					}

					//now allIDs contains a list of units that were not in the packet so they must be deleted
					//note that we can not just delete them because of reference counts and so on.
					//we make them obsolte so that they are deleted next frame
					for(vector<int>::iterator iter = allIDs.begin(); iter != allIDs.end(); ++iter)
					{
						Unit* unit = getUnitById(*iter); //if unit == 0 then there are some serious issues ;)
						if(unit) unit->markForDelete();
					}

				}
				rebuildCellList();
			}
			break;

		case EVENT_CLIENT_CONNECTED:
			{
				int clientId;
				packet >> clientId;

				int factionId;
				packet >> factionId;

				Faction* faction = createFaction(clientId);
				faction->deserialize(packet);
				faction->setClientId(clientId);
				factions.push_back(faction);

				if(clientId == Game::shared().getClientId())
					localFaction = faction;

				int unitCount;
				packet >> unitCount;
				for(int i = 0; i < unitCount; ++i)
				{
					int id;
					packet >> id;
					Unit* unit = createUnit(id, 0);
					unit->deserialize(packet);

					if(faction == localFaction) unit->setLocal(true);

					Object* obj = Root::shared().getScene()->createObject();
					obj->setModel(ModelManager::shared().getModel(unit->getInfo()->modelname + ".aryamodel"));
					obj->setAnimation("stand");

					unit->setObject(obj);

					float heightModel = map->heightAtGroundPosition(unit->getPosition().x, unit->getPosition().z);

					unit->setPosition(vec3(unit->getPosition().x,
								heightModel,
								unit->getPosition().z));

					faction->addUnit(unit);
				}
			}
			break;

		case EVENT_CLIENT_DISCONNECTED:
			{
				int id;
				packet >> id;
				GAME_LOG_INFO("Client " << id << " disconnected.");
				for(vector<Faction*>::iterator iter = factions.begin(); iter != factions.end(); ++iter)
				{
					if( (*iter)->getClientId() == id )
					{
						GAME_LOG_INFO("Client " << id << " removed from game session. NOT removing faction!");
						//delete *iter;
						//iter = factions.erase(iter);
						break;
					}
				}
			}
			break;

		case EVENT_MOVE_UNIT: {
								  int numUnits;
								  packet >> numUnits;

								  int unitId;
								  vec2 unitTargetPosition;
								  for(int i = 0; i < numUnits; ++i) {
									  packet >> unitId;
									  packet >> unitTargetPosition;
									  Unit* unit = getUnitById(unitId);
									  if(unit) unit->setTargetPosition(unitTargetPosition);
								  }

								  break;
							  }

		case EVENT_ATTACK_MOVE_UNIT:
							  {
								  int numUnits;
								  packet >> numUnits;

								  int unitId, targetUnitId;
								  for(int i = 0; i < numUnits; ++i) {
									  packet >> unitId >> targetUnitId;
									  Unit* unit = getUnitById(unitId);
									  Unit* targetUnit = getUnitById(targetUnitId);
									  if(unit && targetUnit) unit->setTargetUnit(targetUnit);
								  }

								  break;
							  }

		case EVENT_UNIT_DIED:
							  {
								  int id;
								  packet >> id;
								  Unit* unit = getUnitById(id);
								  if(unit)
								  {
									  unit->makeDead();
									  unit->getInfo()->onDeath(unit);
								  }
							  }
							  break;

		case EVENT_UNIT_SPAWNED:
							  {
								  int factionId, unitId;
								  packet >> factionId >> unitId;
								  Faction* faction = getFactionById(factionId);
								  if(!faction)
								  {
									  GAME_LOG_WARNING("Unit spawn packet received for invalid faction!");
								  }
								  else
								  {
									  Unit* unit = getUnitById(unitId);
									  bool newUnit = false;
									  if(unit)
										  GAME_LOG_WARNING("Spawn packet for unit that already existed");
									  else
									  {
										  newUnit = true;
										  unit = createUnit(unitId, 0);
									  }
									  unit->deserialize(packet);
									  if(faction == localFaction) unit->setLocal(true);
									  if(newUnit) unit->setCellFromList(unitCells);

									  Object* obj = unit->getObject();
									  if(!obj) obj = Root::shared().getScene()->createObject();
									  obj->setModel(ModelManager::shared().getModel(unit->getInfo()->modelname + ".aryamodel"));
									  obj->setAnimation("stand");
									  unit->setObject(obj);

									  float heightModel = map->heightAtGroundPosition(unit->getPosition().x, unit->getPosition().z);
									  unit->setPosition(vec3(unit->getPosition().x, heightModel, unit->getPosition().z));

									  //This must happen after the object is set, because
									  //then it will set the correct tint color
									  if(newUnit) faction->addUnit(unit);

									  unit->getInfo()->onSpawn(unit);
								  }
							  }
							  break;

		case EVENT_PLAYER_DEFEAT:
							  {
								  int factionID;
								  packet >> factionID;
								  LOG_INFO("Player lost: " << factionID + 1);
								  Arya::SoundManager::shared().play("defeat.wav");
							  }
							  break;

		case EVENT_PLAYER_VICTORY:
							  {
								  int factionID;
								  packet >> factionID;
								  LOG_INFO("Game won by player: " << factionID + 1);
								  Arya::SoundManager::shared().play("victory.wav");
							  }
							  break;

		default:
							  GAME_LOG_INFO("ClientGameSession: unknown event received! (" << id << ")");
							  break;
	}
}
void ClientGameSession::initPathfinding()
{
	if(!map) return;
	const int amountOfPixels = 400;

	const int amountOfPixelsSquared = amountOfPixels * amountOfPixels;
	float xChange = map->getSize()/amountOfPixels;
	float zChange = map->getSize()/amountOfPixels;
	float heightMap[amountOfPixelsSquared] = {0};
	pathfindingMap = new unsigned char[amountOfPixelsSquared];
	for(int i = 0; i < amountOfPixels; i++ )
	{
		for(int j = 0; j < amountOfPixels; j++)
		{
			if(i == 0 || i == amountOfPixels - 1 || j == 0 || j == amountOfPixels - 1)
			{
				heightMap[i * amountOfPixels + j] = 1000.f;
			}
			else
			{
				heightMap[i * amountOfPixels + j] = map->heightAtGroundPosition(i*xChange,j*zChange);
			}
		}
	}

	//7 8 9
	//4 5 6
	//1 2 3
	int j = 0;
	for(int i = 0; i < amountOfPixelsSquared; i++)
	{
		j = 0;
		for(int dx = -1; dx <= 1; dx++)
		{
			for(int dz = -1; dz <= 1; dz++)
			{
				if(dx == 0 && dz == 0) continue;
				j += 1;
				if(glm::abs(dx) == 1 && glm::abs(dz) == 1)
				{
					if(1 > glm::abs(heightMap[i + dx + dz] - heightMap[i])/(glm::sqrt(xChange * xChange + zChange * zChange)))
					{
						pathfindingMap[i] |= (1 << (8-j));
					}
				}
				if(glm::abs(dx) == 1 && dz == 0)
				{
					if(1 > glm::abs(heightMap[i + dx + dz] - heightMap[i])/(xChange))
					{
						pathfindingMap[i] |= (1 << (8-j));
					}
				}
				if(glm::abs(dz) == 1 && dx == 0)
				{
					if(1 > glm::abs(heightMap[i + dx + dz] - heightMap[i])/(zChange))
					{
						pathfindingMap[i] |= (1 << (8-j));
					}
				}
			}
		}
	}
}
