#include <iostream>
#include <string.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

#include "common/Listeners.h"
#include "common/Logger.h"
#include "Objects.h"
#include "Models.h"
#include "Scene.h"
#include "Fonts.h"
#include "Map.h"
#include "Terrain.h"
#include "Textures.h"
#include "Camera.h"
#include "Shaders.h"
#include "Interface.h"

#include "Overlay.h"

using std::string;
using std::cerr;
using std::endl;
using std::vector;
using glm::vec3;

namespace Arya
{	
    Scene::Scene()
    {
        initialized = false;
        currentMap = 0; 
        camera = 0;
        basicProgram = 0;
        lightDirection=vec3(0.7,0.7,0.0);
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
        glGenFramebuffers(1, &shadowFBOHandle);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOHandle);

        glGenTextures(1, &shadowDepthTextureHandle);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTextureHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTextureHandle, 0);

        glDrawBuffer(GL_NONE);

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

            case 0:
                LOG_ERROR("FBO: An error occured");
                return false;
                break;

            default:
                LOG_ERROR("Unknown FBO status");
                return false;
                break;
        }

        orthoShadowCubeMatrix = glm::ortho(-100.0, 100.0, 0.0, 200.0, -100.0, 100.0);

        rotateToLightDirMatrix = glm::rotate(mat4(1.0),
                glm::acos(glm::dot(glm::normalize(lightDirection),
                               vec3(0.0, 0.0, 1.0))) * (180.0f / 3.141592653589793f),
                glm::cross(lightDirection, vec3(0.0, 0.0, 1.0)));

        lightOrthoMatrix = orthoShadowCubeMatrix * rotateToLightDirMatrix;

        return true;
    }

    bool Scene::setMap(const char* hm, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm)
    {
        if(currentMap)
            delete currentMap;
        currentMap = new Map();
        if(!currentMap->init(hm, wm, ts, cm, sm)) return false;
        return true;
    }

    void Scene::cleanup()
    {
        if(camera) delete camera;
        camera = 0;

        if(currentMap) delete currentMap;
        currentMap = 0;

        if(basicProgram) delete basicProgram;
        basicProgram = 0;

        for(int i = 0; i < objects.size(); ++i)
            delete objects[i];
        objects.clear();

        initialized = false;
    }

    void Scene::onFrame(float elapsedTime)
    {
        camera->update(elapsedTime);
        currentMap->update(elapsedTime, this);
        //We might want to let the Game loop on all
        //the objects and animate them. This way the game
        //has more control: it could slow animations down for example
        for(int i = 0; i < objects.size(); ++i)
        {
            objects[i]->updateAnimation(elapsedTime);
        }

        GLfloat depth;
        glReadPixels(0, 0, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        vec4 resultBL = camera->getInverseVPMatrix() * vec4(-1.0, -1.0, depth, 1.0);
        resultBL /= resultBL.w;
        glReadPixels(Root::shared().getWindowWidth() - 1, Root::shared().getWindowHeight() - 1, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        vec4 resultTR = camera->getInverseVPMatrix() * vec4(1.0, 1.0, depth, 1.0);
        resultTR /= resultBL.w;

        LOG_INFO("resultBL: " << resultBL.x << " " << resultBL.y << " " << resultBL.z);
        LOG_INFO("resultTR: " << resultTR.x << " " << resultTR.y << " " << resultTR.z);

        // shadow stuff
        orthoShadowCubeMatrix = glm::ortho(-200.0, 200.0, 0.0, 200.0, -200.0, 0.0);

        rotateToLightDirMatrix = glm::rotate(mat4(1.0),
                glm::acos(glm::dot(glm::normalize(lightDirection),
                               vec3(0.0, 0.0, 1.0))) * (180.0f / 3.141592653589793f),
                glm::cross(lightDirection, vec3(0.0, 0.0, 1.0)));

        lightOrthoMatrix = orthoShadowCubeMatrix * rotateToLightDirMatrix;


    }

    void Scene::render()
    {
        //------------------------------
        // SHADOW PASS
        //------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBOHandle);
        glViewport(0, 0, 1024, 1024);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        basicProgram->use();

        basicProgram->setUniformMatrix4fv("vpMatrix", lightOrthoMatrix);
        basicProgram->setUniformMatrix4fv("viewMatrix", camera->getVMatrix());

        for(int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            if(objects[i]->isObsolete()) continue;
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
            for(int j = 0; j < model->getMeshes().size(); ++j)
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


        //------------------------------
        // NORMAL PASS
        //------------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Root::shared().getWindowWidth(), Root::shared().getWindowHeight());

        basicProgram->use();

        basicProgram->setUniformMatrix4fv("vpMatrix", camera->getVPMatrix());
        basicProgram->setUniformMatrix4fv("viewMatrix", camera->getVMatrix());

        for(int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            if(objects[i]->isObsolete()) continue;
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
            for(int j = 0; j < model->getMeshes().size(); ++j)
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

        currentMap->render(camera);
    }
}
