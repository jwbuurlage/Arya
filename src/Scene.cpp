#include <iostream>
#include <string.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "common/Listeners.h"
#include "common/Logger.h"
#include "Objects.h"
#include "Models.h"
#include "Scene.h"
#include "Fonts.h"
#include "Terrain.h"
#include "Textures.h"
#include "Camera.h"
#include "Shaders.h"
#include "Interface.h"
#include "Decals.h"

#include "Overlay.h"

using std::string;
using std::cerr;
using std::endl;
using std::vector;
using glm::vec3;

const float PI = 3.14159265358979323846264338327950288f;

namespace Arya
{	
    Scene::Scene()
    {
        initialized = false;
        currentTerrain = 0; 
        camera = 0;
        basicProgram = 0;
        lightDirection=glm::normalize(vec3(0.7,0.7,0.2));
        init();
    }

    Scene::~Scene()
    {
        cleanup();
    }

    Object* Scene::createObject()
    {
        Object* obj = new Object;
        objects.push_back(obj);
        return obj;
    }

    bool Scene::init()
    {
        if(!initShaders()) return false;

        if(!initShadowSupport()) return false;

        LOG_INFO("Loading scene");

        if(!camera)
        {
            camera = new Camera;
            camera->setProjectionMatrix(45.0f, Root::shared().getAspectRatio(), 0.1f, 2000.0f);
        }

        initialized = true;

        return true;
    }

    bool Scene::initShaders()
    {
        Shader* basicVertex = new Shader(VERTEX);
        if(!(basicVertex->addSourceFile("../shaders/staticmodel.vert"))) return false;
        if(!(basicVertex->compile())) return false;

        Shader* basicFragment = new Shader(FRAGMENT);
        if(!(basicFragment->addSourceFile("../shaders/staticmodel.frag"))) return false;
        if(!(basicFragment->compile())) return false;

        basicProgram = new ShaderProgram("basic");
        basicProgram->attach(basicVertex);
        basicProgram->attach(basicFragment);
        if(!(basicProgram->link())) return false;

        return true;
    }

    bool Scene::initShadowSupport()
    {
        if(glGetError() != GL_NO_ERROR)
            LOG_ERROR("GL error before FBO");

        int shadowFBOSize = 2048;
        glGenFramebuffers(1, &shadowFBOHandle);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOHandle);

        glGenTextures(1, &shadowDepthTextureHandle);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTextureHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowFBOSize, shadowFBOSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glDrawBuffer(GL_NONE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureHandle, 0);

        switch(glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
            case GL_FRAMEBUFFER_COMPLETE:
                LOG_INFO("Framebuffer is complete");
                break;

            case GL_FRAMEBUFFER_UNSUPPORTED:
                LOG_ERROR("Framebuffer is unsupported");
                return false;

                break;

            case GL_FRAMEBUFFER_UNDEFINED:
                LOG_ERROR("Framebuffer is undefined");
                return false;
                break;

            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
                LOG_ERROR("Framebuffer has incomplete attachment");
                return false;
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
                LOG_ERROR("Framebuffer is missing attachment");
                return false;
                break;

            case 0:
                LOG_ERROR("FBO: An error occured");
                return false;
                break;

            default:
                LOG_ERROR("Unknown FBO status");
                return false;
                break;
        }

        orthoShadowCubeMatrix = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);
        lightOrthoMatrix = orthoShadowCubeMatrix;

        return true;
    }

    bool Scene::setTerrain(char* heightData, int terrainSize, const char* waterMap, const vector<Material*>& tileSet, Texture* cloudMap, Texture* splatMap)
    {
        if(currentTerrain) delete currentTerrain;
        currentTerrain = 0;

        if(!heightData || !terrainSize || !waterMap || tileSet.empty() || !cloudMap || !splatMap)
        {
            return false;
        }

        currentTerrain = new Terrain(heightData, terrainSize, waterMap, tileSet, cloudMap, splatMap);
        if(!currentTerrain->init())
        {
            delete currentTerrain;
            currentTerrain = 0;
            return false;
        }
        return true;
    }

    void Scene::cleanup()
    {
        if(camera) delete camera;
        camera = 0;

        if(currentTerrain) delete currentTerrain;
        currentTerrain = 0;

        if(basicProgram) delete basicProgram;
        basicProgram = 0;

        for(unsigned int i = 0; i < objects.size(); ++i)
            delete objects[i];
        objects.clear();

        initialized = false;
    }

    void Scene::onFrame(float elapsedTime)
    {
        //We might want to let the Game loop on all
        //the objects and animate them. This way the game
        //has more control: it could slow animations down for example
        for(vector<Object*>::iterator iter = objects.begin(); iter != objects.end(); )
        {
            if( (*iter)->isObsolete() )
            {
                delete *iter;
                iter = objects.erase(iter);
            }
            else
            {
                (*iter)->updateAnimation(elapsedTime);
                ++iter;
            }
        }
        camera->update(elapsedTime);
        currentTerrain->update(elapsedTime, this);

        //
        // Calculate the shadow projection matrix
        //

        //assumes lightDirection is normalized
        float lightPitch = 90.0f - (180.0f/PI)*glm::acos(lightDirection.y); //[-180,180] positive means pointing upward
        float lightYaw = (180.0f/PI)*glm::atan(lightDirection.x , lightDirection.z);
        rotateToLightDirMatrix = mat4(1.0f);
        rotateToLightDirMatrix = glm::rotate( rotateToLightDirMatrix, lightPitch, vec3(1.0, 0.0, 0.0) );
        rotateToLightDirMatrix = glm::rotate( rotateToLightDirMatrix, -lightYaw, vec3(0.0, 1.0, 0.0) );
        rotateToLightDirMatrix = glm::translate( rotateToLightDirMatrix, -camera->getPosition() );
        
        float shadowBoxSize = 2.0f*camera->getZoom();
        //left,right,bottom,top,near,far
        orthoShadowCubeMatrix = glm::ortho(-shadowBoxSize, shadowBoxSize, -shadowBoxSize, shadowBoxSize, -shadowBoxSize, shadowBoxSize);

        lightOrthoMatrix = orthoShadowCubeMatrix * rotateToLightDirMatrix;
    }

    void Scene::render()
    {
        Root::shared().checkForErrors("scene render start");

        //------------------------------
        // SHADOW PASS
        //------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOHandle);
        glViewport(0, 0, 2048, 2048);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        basicProgram->use();

        basicProgram->setUniformMatrix4fv("vpMatrix", lightOrthoMatrix);
        //basicProgram->setUniformMatrix4fv("viewMatrix", camera->getVMatrix());

        for(unsigned int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            if(objects[i]->isObsolete()) continue;
 
			mat4 totalMatrix = camera->getVPMatrix() * objects[i]->getMoveMatrix();
			bool flag = false;
			for(int j = 0; j < 8; j++)
			{
				vec4 onScreen(objects[i]->model->getBoundingBoxVertex(j), 1.0);
				onScreen = totalMatrix * onScreen;
				onScreen /= onScreen.w;
				if(!(onScreen.x < -2.0 || onScreen.x > 2.0 || onScreen.y < -2.0 || onScreen.y > 2.0))
				{
					flag = true;
					break;
				}
			}
			if(flag == false) continue;           

            basicProgram->setUniform3fv("tintColor", objects[i]->getTintColor());

            basicProgram->setUniformMatrix4fv("mMatrix", objects[i]->getMoveMatrix());

            int frame = 0;
            float interpolation = 0.0f;
            AnimationState* animState = objects[i]->getAnimationState(); //can be zero
            if(animState)
            {
                frame = animState->getCurFrame();
                interpolation = animState->getInterpolation();
                basicProgram->setUniform1f("interpolation", interpolation);
            }

            Model* model = objects[i]->model;
            for(unsigned int j = 0; j < model->getMeshes().size(); ++j)
            {
                Mesh* mesh = model->getMeshes()[j];
                if(mesh->frameCount > 0)
                {
                    Material* mat = model->getMaterials()[mesh->materialIndex];
                    basicProgram->setUniform4fv("parameters", vec4(mat->specAmp,mat->specPow,mat->ambient,mat->diffuse));
                    basicProgram->setUniform1i("tex", 0);
                    glActiveTexture(GL_TEXTURE0);
                    if(mat) glBindTexture(GL_TEXTURE_2D, mat->texture->handle);

                    glBindVertexArray(mesh->vaoHandles[frame]);
                    glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Root::shared().getWindowWidth(), Root::shared().getWindowHeight());

        //------------------------------
        // TERRAIN AND DECALS
        //------------------------------

        currentTerrain->render(camera);
		Decals::shared().render();

        //------------------------------
        // NORMAL PASS
        //------------------------------

        basicProgram->use();

        basicProgram->setUniformMatrix4fv("vpMatrix", camera->getVPMatrix());
        //basicProgram->setUniformMatrix4fv("viewMatrix", camera->getVMatrix());

        for(unsigned int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            if(objects[i]->isObsolete()) continue;

			mat4 totalMatrix = camera->getVPMatrix() * objects[i]->getMoveMatrix();
			bool flag = false;
			for(int j = 0; j < 8; j++)
			{
				vec4 onScreen(objects[i]->model->getBoundingBoxVertex(j), 1.0);
				onScreen = totalMatrix * onScreen;
				onScreen /= onScreen.w;
				if(!(onScreen.x < -1.0 || onScreen.x > 1.0 || onScreen.y < -1.0 || onScreen.y > 1.0))
				{
					flag = true;
					break;
				}
			}
			if(flag == false) continue;

            basicProgram->setUniform3fv("tintColor", objects[i]->getTintColor());

            basicProgram->setUniformMatrix4fv("mMatrix", objects[i]->getMoveMatrix());

            int frame = 0;
            float interpolation = 0.0f;
            AnimationState* animState = objects[i]->getAnimationState(); //can be zero
            if(animState)
            {
                frame = animState->getCurFrame();
                interpolation = animState->getInterpolation();
                basicProgram->setUniform1f("interpolation", interpolation);
            }

            Model* model = objects[i]->model;
            for(unsigned int j = 0; j < model->getMeshes().size(); ++j)
            {
                Mesh* mesh = model->getMeshes()[j];
                if(mesh->frameCount > 0)
                {
                    Material* mat = model->getMaterials()[mesh->materialIndex];
                    basicProgram->setUniform4fv("parameters", vec4(mat->specAmp,mat->specPow,mat->ambient,mat->diffuse));
                    basicProgram->setUniform1i("tex", 0);
                    glActiveTexture(GL_TEXTURE0);
                    if(mat) glBindTexture(GL_TEXTURE_2D, mat->texture->handle);

                    glBindVertexArray(mesh->vaoHandles[frame]);
                    glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
                }
            }
        }

        Root::shared().checkForErrors("scene render end");

    }
}
