#include "Models.h"
#include "Primitives.h"
#include "Files.h"
#include "Materials.h"
#include "common/Logger.h"

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
    Model::Model()
    {

    }

    Model::~Model()
    {
        for(int i = 0; i < meshes.size(); ++i)
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
        //If bone, create BoneAnimationState
        //If vertex, create VertexAnimationState
        //else return 0
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

    Model* ModelManager::loadResource(const char* filename)
    {
        File* modelfile = FileSystem::shared().getFile(filename);
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

            model = new Model;

            model->modelType = (ModelType)header->modeltype;

            LOG_INFO("Loading model " << filename);
            LOG_INFO("Model has " << header->submeshCount << " meshes.");

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

                LOG_INFO("Loading vertex data. frameBytes: " << frameBytes << ". frameCount: " << mesh->frameCount);
                LOG_INFO("Buffer offset: " << header->submesh[s].bufferOffset);
                LOG_INFO("Model filesize: " << modelfile->getSize());

                glGenBuffers(1, &mesh->vertexBuffer);
                glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
                glBufferData(GL_ARRAY_BUFFER,
                        mesh->frameCount * frameBytes,
                        modelfile->getData() + header->submesh[s].bufferOffset,
                        GL_STATIC_DRAW);
                if( header->submesh[s].indexCount > 0 )
                {
                    LOG_INFO("Mesh has index buffer");
                    mesh->indexCount = header->submesh[s].indexCount;
                    glGenBuffers(1, &mesh->indexBuffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                            sizeof(GLuint) * mesh->indexCount,
                            modelfile->getData() + header->submesh[s].indexbufferOffset,
                            GL_STATIC_DRAW);
                }

                LOG_INFO("Creating VAOs");

                //Create a VAO for every frame
                mesh->createVAOs(mesh->frameCount);
                for(int f = 0; f < mesh->frameCount; ++f)
                {
                    glBindVertexArray(mesh->vaoHandles[f]);
                    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);

                    glEnableVertexAttribArray(0); //pos
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLubyte*>(f*frameBytes + 0));
                    glEnableVertexAttribArray(1); //tex
                    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLubyte*>(f*frameBytes + 12));
                    if(header->submesh[s].hasNormals)
                    {
                        glEnableVertexAttribArray(2); //norm
                        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLubyte*>(f*frameBytes + 20));
                    }
                }
            }

            LOG_INFO("Vertex data loaded. Loading materials");

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
                LOG_INFO("Loading material: " << nameBuf);
                Material* mat = TextureManager::shared().getTexture(nameBuf);
                model->addMaterial(mat);
            }
            delete[] nameBuf;

            //Parse animations
            int animationCount = *(int*)pointer;
            LOG_INFO("Model has " << animationCount << " animations.");

            addResource(filename, model);
        }while(0);

        FileSystem::shared().releaseFile(modelfile);
        return model;
    }
}
