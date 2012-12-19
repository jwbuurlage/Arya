#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <map>
#include <algorithm>
#include <GL/glew.h>

using namespace std;

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

typedef struct {
    //FILE INFO:
    int identity;       //this should be equal to "IDP2" if it's an md2 file
    int version;        //this should be equal to 8, this is the md2 version

    //MODEL INFO:
    int textureWidth;
    int textureHeight;
    int frameSize;      //size in bytes of each frame in an animation

    //NUMBER OF:
    int nTex;           //textures
    int nVertices;      //vertices
    int nTexCoo;        //texture coordinates
    int nTriangles;     //triangles
    int nGLCommands;    //OpenGL commands (GL_TRIANGLE_STRIP or FAN)
    int nFrames;        //frames (you have x frames per animation)

    //OFFSETS:
    int oTexNames;      //texture names
    int oTexCoo;        //texture coordinates
    int oTriangles;     //triangles
    int oFrames;        //frames
    int oGLCommands;    //OpenGL commands
    int oEOF;           //end-of-file

} ModelHeader;

typedef struct
{
    short    s;
    short    t;
} texCoo;

typedef struct {
    unsigned char   v[3];                // compressed vertex (x, y, z) coordinates
    unsigned char   lightnormalindex;    // index to a normal vector for the lighting
} vertex;

typedef struct
{
    unsigned short   vert[3];    // indexes to triangle's vertices
    unsigned short   tex[3];     // indexes to vertices' texture coorinates
} triangle;

typedef struct {
    float scale[3];         // scale values
    float translate[3];     // translation vector
    char name[16];          // frame name
    vertex verts[1];        // first vertex of this frame
} frame;

typedef struct {
    int firstFrame; // first frame of the animation
    int lastFrame;  // number of frames
    int fps;        // number of frames per second
} animation;

animation MD2animationlist[21] = 
{
    // first, last, fps
    {   0,  39,  9 },   // STAND
    {  40,  45, 10 },   // RUN
    {  46,  53, 10 },   // ATTACK
    {  54,  57,  7 },   // PAIN_A
    {  58,  61,  7 },   // PAIN_B
    {  62,  65,  7 },   // PAIN_C
    {  66,  71,  7 },   // JUMP
    {  72,  83,  7 },   // FLIP
    {  84,  94,  7 },   // SALUTE
    {  95, 111, 10 },   // FALLBACK
    { 112, 122,  7 },   // WAVE
    { 123, 134,  6 },   // POINT
    { 135, 153, 10 },   // CROUCH_STAND
    { 154, 159,  7 },   // CROUCH_WALK
    { 160, 168, 10 },   // CROUCH_ATTACK
    { 196, 172,  7 },   // CROUCH_PAIN
    { 173, 177,  5 },   // CROUCH_DEATH
    { 178, 183,  7 },   // DEATH_FALLBACK
    { 184, 189,  7 },   // DEATH_FALLFORWARD
    { 190, 197,  7 },   // DEATH_FALLBACKSLOW
    { 198, 198,  5 },   // BOOM
};

const char* MD2animationNameList[21] =
{
    "stand",
    "run",
    "attack",
    "pain_a",
    "pain_b",
    "pain_c",
    "jump",
    "flip",
    "salute",
    "fallback",
    "wave",
    "point",
    "crouch_stand",
    "crouch_walk",
    "crouch_attack",
    "crouch_pain",
    "crouch_death",
    "death_fallback",
    "death_fallforward",
    "death_fallbackslow",
    "boom"
};

#define MD2_IDENTITY (('2'<<24) + ('P'<<16) + ('D'<<8) + 'I')
#define MD2_VERSION 8

typedef GLfloat vec3[3];

vec3 anorms[] = {
#include "anorms.h"
};

int main(int argc, char* argv[])
{
    //
    // Generic file opening
    //

    if( argc < 3 )
    {
        cout << "Usage: " << argv[0] << " md2file.md2 outputfile.aryamodel [scalefactor translatevector]" << endl;
        cout << "Example: " << argv[0] << " ogros.md2 ogros.aryamodel" << endl;
        cout << "Example: " << argv[0] << " ogros.md2 ogros.aryamodel 0.2 0 -5 0" << endl;
        return 0;
    }

    string inputfilename;
    string outputfilename;
    float scaleFactor = 1.0f;
    float transX = 0.0f, transY = 0.0f, transZ = 0.0f;

    inputfilename = argv[1];
    outputfilename = argv[2];
    if( argc >= 7 )
    {
        stringstream parser;
        parser << argv[3] << " " << argv[4] << " " << argv[5] << " " << argv[6];
        parser >> scaleFactor >> transX >> transY >> transZ;
    }

    ifstream inputfile;
    ofstream outputfile;

    inputfile.open(inputfilename.c_str(), ios::binary);
    if(!inputfile.is_open())
    {
        cerr << "File not found: " << inputfilename << endl;
        return -1;
    }

    outputfile.open(outputfilename.c_str(), ios::binary);
    if(!outputfile.is_open())
    {
        cerr << "Unable to open output file: " << outputfilename << endl;
        return -1;
    }

    int inputFilesize;

    //Get filesize
    inputfile.seekg(0, ios::end);
    inputFilesize = inputfile.tellg();
    inputfile.seekg(0, ios::beg);

    char* inputData = new char[inputFilesize+1];
    inputData[inputFilesize] = 0; //terminate 0 to be sure

    inputfile.read(inputData, inputFilesize);

    //
    // File has been read into memory
    //

    //Parse MD2 data

    ModelHeader* header = (ModelHeader*)inputData;

    if( (unsigned int)inputFilesize < sizeof(ModelHeader) || header->identity != MD2_IDENTITY || header->version != MD2_VERSION )
    {
        cerr << "File is not a valid MD2 model file: " << argv[1] << endl;
        return -1;
    }

    cout << "Scale factor " << scaleFactor << endl;
    cout << "Translation: (" << transX << "," << transY << "," << transZ << ")" << endl;
    cout << "Parsing input file..." << endl;

    bool animated = header->nFrames > 1 ? true : false;

    //TODO: calculate needed size
    //Just take a huge upper bound :D
    char* outBuffer = new char[inputFilesize * 300];

    char* pointer = outBuffer;

    //Output data start
    AryaHeader* outHeader = (AryaHeader*)pointer;

    outHeader->magic = ARYAMAGICINT;
    outHeader->modeltype = (animated ? 2 : 1);
    outHeader->submeshCount = 1;
    outHeader->materialCount = 1;
    outHeader->frameCount = header->nFrames;

    //Submesh info (only one submesh here)
    outHeader->submesh[0].materialIndex = 0;
    outHeader->submesh[0].primitiveType = GL_TRIANGLES;
    outHeader->submesh[0].hasNormals = 1;

    pointer += sizeof(AryaHeader);
    pointer += 1*sizeof(SubmeshInfo);
    //end of submesh info

    //material list: only one material
    int namelen = inputfilename.length() - 4; //remove the .md2
    for(int i = 0; i < namelen; ++i) *pointer++ = inputfilename[i];
    *pointer++ = 0; //zero terminated
    cout << "Saving material " << (pointer - namelen - 1) << endl;

    //Animation info
    //The animation info does not come from the source file. It is static MD2 animation data
    if( animated )
    {
        cout << "Model is animated. Storing animation info" << endl;

        *(int*)pointer = 21; pointer += 4; //21 animations
        for(int i = 0; i < 21; ++i)
        {
            //name, startframe, endframe, (end-start) times the FPS
            int len = strlen(MD2animationNameList[i]);
            for(int j = 0; j < len; ++j) *pointer++ = MD2animationNameList[i][j];
            *pointer++ = 0; //0 terminate

            *(int*)pointer = MD2animationlist[i].firstFrame; pointer += 4;
            *(int*)pointer = MD2animationlist[i].lastFrame; pointer += 4;
            for(int j = 0; j <= MD2animationlist[i].lastFrame - MD2animationlist[i].firstFrame; ++j)
            {
                *(float*)pointer = 1.0f/((float)MD2animationlist[i].fps);
                pointer += 4;
            }
        }
    }
    else
    {
        *(int*)pointer = 0; pointer += 4; //zero animations
    }
    //End of animation info

    //Now we will create a new index buffer

    //for(int tri = 0; tri < header->nTriangles; ++tri)
    //{
    //    for(int m = 0; m < 3; ++m)
    //    {
    //        int index = triangleInput[tri].vert[m];
    //        int texIndex = triangleInput[tri].tex[m];
    //    }
    //}

    //Vertex data
    cout << "Building vertex buffer" << endl;

    //We want the vertex data 4-byte aligned
    int offset = (int)(pointer - outBuffer);
    offset = (offset + 3) & (~3);
    pointer = outBuffer + offset;

    outHeader->submesh[0].bufferOffset = offset; //set offset in header

    triangle* triangleInput = (triangle*)(inputData + header->oTriangles);
    texCoo* texCooInput = (texCoo*)(inputData + header->oTexCoo);

    float* floatOutput = (float*)pointer;

    float maxX = -10000.0f, maxY = -10000.0f, maxZ = -10000.0f;
    float minX = 10000.0f, minY = 10000.0f, minZ = 10000.0f;

    for(int fr = 0; fr < header->nFrames; ++fr)
    {
        frame* inputFrame = (frame*)(inputData + header->oFrames + fr * header->frameSize);

        for(int tri = 0; tri < header->nTriangles; ++tri)
        {
            for(int m = 0; m < 3; ++m)
            {
                int index = triangleInput[tri].vert[m];
                int texIndex = triangleInput[tri].tex[m];
                int normIndex = inputFrame->verts[index].lightnormalindex;

                float x = scaleFactor*(transX + (float)((inputFrame->verts[index].v[1] * inputFrame->scale[1]) + inputFrame->translate[1]));
                float y = scaleFactor*(transY + (float)((inputFrame->verts[index].v[2] * inputFrame->scale[2]) + inputFrame->translate[2]));
                float z = scaleFactor*(transZ - (float)((inputFrame->verts[index].v[0] * inputFrame->scale[0]) + inputFrame->translate[0]));
                *floatOutput++ = x;
                *floatOutput++ = y;
                *floatOutput++ = z;

                *floatOutput++ = (float)(texCooInput[texIndex].s) / ((float)header->textureWidth);
                *floatOutput++ = (float)(texCooInput[texIndex].t) / ((float)header->textureHeight);

                *floatOutput++ = (float)(anorms[normIndex][1]);
                *floatOutput++ = (float)(anorms[normIndex][2]);
                *floatOutput++ = (float)(anorms[normIndex][0]);

                if(x>maxX) maxX = x;
                else if(x<minX) minX = x;
                if(y>maxY) maxY = y;
                else if(y<minY) minY = y;
                if(z>maxZ) maxZ = z;
                else if(z<minZ) minZ = z;
            }
        }
    }

    int vertexCount = header->nTriangles * 3; //vertex count per frame
    pointer += header->nFrames * vertexCount * 8 * sizeof(float);

    outHeader->submesh[0].vertexCount = vertexCount;
    //End of vertex data
    cout << "Vertex buffer done. " << header->nFrames << " frames with " << vertexCount << " vertices each written." << endl;
    cout << "min X, min Y, min Z : " << minX << "," << minY << "," << minZ << endl;
    cout << "max X, max Y, max Z : " << maxX << "," << maxY << "," << maxZ << endl;

    //Index data
    outHeader->submesh[0].indexCount = 0;
    outHeader->submesh[0].indexbufferOffset = (int)(pointer - outBuffer); //set offset in header
    //End of index data

    int totalFileSize = (int)(pointer - outBuffer);
    outputfile.write(outBuffer, totalFileSize);
    outputfile.close();

    delete[] outBuffer;

    return 0;
}
