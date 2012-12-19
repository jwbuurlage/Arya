#include "Models.h"
#include "Primitives.h"
#include "Files.h"
#include "Materials.h"
#include "common/Logger.h"
#include <string>
#include <map>

typedef struct{
    int materialIndex;
    int primitiveType;
    int vertexCount; //per frame
    int hasNormals;
    int indexCount;
    int bufferOffset;
    int indexbufferOffset;
} SubmeshInfo;

typedef struct{
    int magic;
    int modeltype; //1 static, 2 vertex animated, 3 bone animated
    int submeshCount;
    int materialCount;
    int frameCount; //1 for static meshes
    SubmeshInfo submesh[0];
} AryaHeader;

#define ARYAMAGICINT (('A' << 0) | ('r' << 8) | ('M' << 16) | ('o' << 24))

namespace Arya
{
    //The different animation states
    //These classes are declared here
    //so that the rest of the engine
    //only has to know the base class

    struct VertexAnim
    {
        int startFrame; //inclusive
        int endFrame; //inclusive
        vector<float> frameTimes; //size = end - start + 1
    };

    class VertexAnimationData : public AnimationData
    {
        public:
            map<string, VertexAnim> animations;
    };
    typedef std::pair<string,VertexAnim> animMapType;
    typedef std::map<string,VertexAnim>::iterator animMapIterator;

    class VertexAnimationState : public AnimationState
    {
        public:
            VertexAnimationState(VertexAnimationData* data)
            {
                animData = data;
                curAnim = 0;
                startFrame = 0;
                curFrame = 0;
                timer = 0.0f;
                interpolation = 0.0f;
            }
            ~VertexAnimationState() {}

            //Base class overloads
            void setAnimation(std::string name)
            {
                if(!animData) return;
                animMapIterator anim = animData->animations.find(name);
                if(anim == animData->animations.end())
                {
                    LOG_WARNING("Animation not found: " << name);
                    return;
                }
                if(curAnim == &anim->second) return;
                curAnim = &anim->second;
                startFrame = curAnim->startFrame;
                curFrame = 0;
                timer = 0.0f;
                interpolation = 0.0f;
            }

            void updateAnimation(float elapsedTime)
            {
                if(!curAnim) return;

                timer += elapsedTime;

                //Advance frames
                //It is a loop because it is possible to skip more
                //frames if frametimes are short, or the update is
                //over a large time
                while(timer > curAnim->frameTimes[curFrame])
                {
                    timer -= curAnim->frameTimes[curFrame];
                    curFrame++;
                    if(curFrame > (curAnim->endFrame - curAnim->startFrame)) curFrame = 0;
                }
                interpolation = timer / curAnim->frameTimes[curFrame];
            }

            int getCurFrame(){ return startFrame + curFrame; }
            float getInterpolation(){ return interpolation; }

        private:
            VertexAnimationData* animData;

            VertexAnim* curAnim; //one of the animations in animData

            int startFrame;
            int curFrame; //current frame RELATIVE TO STARTFRAME
            float timer;
            float interpolation; //in range [0,1]
    };

    Model::Model()
    {
        refCount = 0;
        animationData = 0;
    }

    Model::~Model()
    {
        for(unsigned int i = 0; i < meshes.size(); ++i)
        {
            meshes[i]->release();
            if( meshes[i]->getRefCount() <= 0 )
            {
                delete meshes[i];
            }
        }
    }

    AnimationState* Model::createAnimationState()
    {
        if(animationData == 0) return 0;
        //If bone, create BoneAnimationState
        //If vertex, create VertexAnimationState
        //else return 0
        if(modelType == VertexAnimated) return new VertexAnimationState((VertexAnimationData*)animationData);
        return 0;
    }

    void Model::addMesh(Mesh* mesh)
    {
        meshes.push_back(mesh);
        mesh->addRef();
    }

    void Model::addMaterial(Material* mat)
    {
        //Note: if mat is zero we still want
        //to put it in the array because
        //Meshes refer to this array by index!
        materials.push_back(mat);
        //if(mat) mat->addRef();
    }

    Mesh* Model::createAndAddMesh()
    {
        Mesh* newMesh = new Mesh;
        addMesh(newMesh);
        return newMesh;
    }

    template<> ModelManager* Singleton<ModelManager>::singleton = 0;

    ModelManager::ModelManager()
    {
    }

    ModelManager::~ModelManager()
    {
        cleanup();
    }

    bool ModelManager::initialize()
    {
        addResource("triangle", new Triangle);
        addResource("quad", new Quad);
        return true;
    }

    void ModelManager::cleanup()
    {
        unloadAll();
    }

    Model* ModelManager::loadResource(std::string filename)
    {
        File* modelfile = FileSystem::shared().getFile(string("models/") + filename);
        if( modelfile == 0 ) return 0;

        //Note: except for the first magic int
        //this loader does not check the integrity of the data
        //This means that it could crash on invalid files

        char* pointer = modelfile->getData();

        AryaHeader* header = (AryaHeader*)pointer;

        Model* model = 0;

        do{ //for easy break on errors

            if( (modelfile->getSize() < sizeof(AryaHeader) + sizeof(SubmeshInfo)) || header->magic != ARYAMAGICINT )
            {
                LOG_ERROR("Not a valid Arya model file: " << filename);
                break;
            }

            if( header->modeltype < 1 || header->modeltype > 2 )
            {
                LOG_ERROR("Arya model with unkown modeltype: " << header->modeltype);
                break;
            }

            if( header->frameCount < 1 )
            {
                LOG_ERROR("Arya model with invalid number of frames: " << header->frameCount);
                break;
            }

            model = new Model;

            model->modelType = (ModelType)header->modeltype;
            LOG_INFO("Loading model " << filename << " with " << header->submeshCount << " meshes.");
            //Parse all materials
            pointer += sizeof(AryaHeader);
            pointer += header->submeshCount*sizeof(SubmeshInfo);

            char* nameBuf = new char[512];
            for(int m = 0; m < header->materialCount; ++m)
            {
                //Get string
                int count = 0;
                nameBuf[0] = *pointer++;
                while(nameBuf[count]){ ++count; nameBuf[count] = *pointer++; }
                nameBuf[count++] = '.';
                nameBuf[count++] = 't';
                nameBuf[count++] = 'g';
                nameBuf[count++] = 'a';
                nameBuf[count++] = 0;
                
                Material* mat = MaterialManager::shared().getMaterial(nameBuf);
                model->addMaterial(mat);
            }

            //Parse animations
            VertexAnimationData* animData = 0;

            int animationCount = *(int*)pointer; pointer += 4;
            if(!animationCount)
            {
                model->animationData = 0;
                LOG_INFO("Model has no animations");
            }
            else
            {
                Logger::shared() << Logger::L_INFO << "Model has " << animationCount << " animations: ";

                animData = new VertexAnimationData;
                model->animationData = animData;

                VertexAnim newAnim;
                for(int anim = 0; anim < animationCount; ++anim)
                {
                    //Get string
                    int count = 0;
                    nameBuf[0] = *pointer++;
                    while(nameBuf[count]){ ++count; nameBuf[count] = *pointer++; }

                    Logger::shared() << nameBuf << " ";

                    newAnim.frameTimes.clear();
                    newAnim.startFrame = *(int*)pointer; pointer += 4;
                    newAnim.endFrame = *(int*)pointer; pointer += 4;
                    for(int i = 0; i <= (newAnim.endFrame-newAnim.startFrame); ++i)
                    {
                        newAnim.frameTimes.push_back( *(float*)pointer );
                        pointer += 4;
                    }

                    //Only add the animation if there are actually enough frames
                    if( newAnim.startFrame < header->frameCount && newAnim.endFrame < header->frameCount )
                        animData->animations.insert(animMapType(nameBuf, newAnim));
                }

                Logger::shared() << endLog;
            }

            delete[] nameBuf;

            //Parse all meshes
            for(int s = 0; s < header->submeshCount; ++s)
            {
                Mesh* mesh = model->createAndAddMesh();

                mesh->primitiveType = header->submesh[s].primitiveType;
                mesh->vertexCount = header->submesh[s].vertexCount;
                mesh->frameCount = header->frameCount;
                mesh->materialIndex = header->submesh[s].materialIndex;

                int floatCount = header->submesh[s].hasNormals ? 8 : 5;
                int frameBytes = mesh->vertexCount * floatCount * sizeof(GLfloat);

                glGenBuffers(1, &mesh->vertexBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER,
                        mesh->frameCount * frameBytes,
                        modelfile->getData() + header->submesh[s].bufferOffset,
                        GL_STATIC_DRAW);
                if( header->submesh[s].indexCount > 0 )
                {
                    mesh->indexCount = header->submesh[s].indexCount;
                    glGenBuffers(1, &mesh->indexBuffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                            sizeof(GLuint) * mesh->indexCount,
                            modelfile->getData() + header->submesh[s].indexbufferOffset,
                            GL_STATIC_DRAW);
                }
                else
                {
                    mesh->indexCount = 0;
                    mesh->indexBuffer = 0;
                }

                //Create a VAO for every frame
                mesh->createVAOs(mesh->frameCount);
                int stride = floatCount * sizeof(GLfloat);

                if( mesh->frameCount == 1 )
                {
                    //Not animated
                    glBindVertexArray(mesh->vaoHandles[0]);
                    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

                    glEnableVertexAttribArray(0); //pos
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(0));
                    glEnableVertexAttribArray(1); //tex
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(12));
                    if(header->submesh[s].hasNormals)
                    {
                        glEnableVertexAttribArray(2); //norm
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(20));
                    }
                    if(mesh->indexCount > 0)
                        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
                }
                else
                {
                    //Animated
                    //We actually have to parse the list of animations here
                    //because the endFrame of one animation should have startFrame as 'nextFrame'
                    for(int f = 0; f < mesh->frameCount; ++f)
                    {
                        int nextf = (f+1)%mesh->frameCount;
                        if(animData)
                        {
                            animMapIterator iter;
                            for(iter = animData->animations.begin(); iter != animData->animations.end(); ++iter)
                            {
                                if( iter->second.endFrame == f )
                                {
                                    nextf = iter->second.startFrame;
                                    break;
                                }
                            }
                        }

                        glBindVertexArray(mesh->vaoHandles[f]);
                        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);

                        glEnableVertexAttribArray(0); //pos
                        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 0));

                        glEnableVertexAttribArray(3); //next pos
                        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(nextf*frameBytes + 0));

                        glEnableVertexAttribArray(1); //tex
                        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 12));

                        if(header->submesh[s].hasNormals)
                        {
                            glEnableVertexAttribArray(2); //norm
                            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(f*frameBytes + 20));

                            glEnableVertexAttribArray(4); //next norm
                            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<GLubyte*>(nextf*frameBytes + 20));
                        }
                        if(mesh->indexCount > 0)
                            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
                    }
                }
            }

            addResource(filename, model);
        }while(0);

        FileSystem::shared().releaseFile(modelfile);
        return model;
    }
}
