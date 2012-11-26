#include <iostream>
#include <string.h>
#include <GL/glew.h>

#include "Shaders.h"
#include "Files.h"

using std::string;
using std::cerr;
using std::endl;

namespace Arya
{	
    //---------------------------------------------------------
    // SHADER
    //---------------------------------------------------------

    bool Shader::addSourceFile(string f)
    {
        File* source = FileSystem::shared().getFile(f);
        if(!source) return false;
        sources.push_back(source);
        return true;
    }

    bool Shader::compile()
    {
        if(sources.empty()) 
        {
            cerr << "No sources set, cannot compile shader" << endl;
            return false;
        }
        else 
        {
            GLchar** gl_sources = new GLchar*[sources.size()];
            for(int i = 0; i < sources.size(); ++i)
                gl_sources[i] = sources[i]->getData();

            GLuint shaderHandle;
            switch(type)
            {
                case VERTEX:    shaderHandle = glCreateShader(GL_VERTEX_SHADER); break;
                case FRAGMENT:  shaderHandle = glCreateShader(GL_FRAGMENT_SHADER); break;
                case GEOMETRY:  shaderHandle = glCreateShader(GL_GEOMETRY_SHADER); break;
                default: cerr << "Error compiling shader: Invalid type." << endl; break;
            }

            const GLchar* pFile = sources[0]->getData();
            const GLint pSize  = sources[0]->getSize();
            glShaderSource(shaderHandle, 1, &pFile, &pSize);

            GLint result;
            glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
            if(result == GL_FALSE)
            {
                cerr << "Error compiling shader, log:" << endl;
                GLint logLength;
                glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
                if(logLength > 0)
                {
                    char* log = new char[logLength];
                    GLsizei written;
                    glGetShaderInfoLog(shaderHandle, logLength, &written, log);
                    cerr << log << endl;
                    delete[] log;
                }
                else 
                {
                    cerr << "No Log available.." << endl;
                }

                glDeleteShader(shaderHandle);
                compiled = false;
                handle = 0;

                return false;
            }

            compiled = true;
            handle = shaderHandle;
        }

        return true;
    }

    //---------------------------------------------------------
    // SHADERPROGRAM
    //---------------------------------------------------------

    ShaderProgram::ShaderProgram(string name)
    {
        init();
        this->name = name;
    }

    ShaderProgram::~ShaderProgram() { 
        glDeleteShader(handle);
    }

    bool ShaderProgram::init()
    {
        GLuint programHandle = glCreateProgram();
        if(programHandle == 0)
        {
            cerr << "Error creating program" << endl; return false;
        }

        return true;
    }

    void ShaderProgram::attach(Shader* shader)
    {
        glAttachShader(handle, shader->getHandle());
    }

    bool ShaderProgram::link()
    {
        glLinkProgram(handle);

        GLint result;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE)
        {
            cerr << "Error linking program, log:" << endl;
            GLint logLength;
            glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &logLength);
            if(logLength > 0)
            {
                char* log = new char[logLength];
                GLsizei written;
                glGetProgramInfoLog(handle, logLength, &written, log);
                cerr << log << endl;
                delete[] log;
            }

            return false;
        }
        return false;
    }

    void ShaderProgram::use()
    {
        glUseProgram(handle);
    }
}
