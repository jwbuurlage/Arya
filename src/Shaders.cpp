#include <iostream>
#include <string.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Shaders.h"
#include "Files.h"

using std::string;
using std::cerr;
using std::endl;

using glm::mat4;
using glm::vec2;

namespace Arya
{	
    //---------------------------------------------------------
    // SHADER
    //---------------------------------------------------------

    Shader::Shader(ShaderType type)
    {
        this->type = type;
        handle = 0;
        compiled = false;
    }

    Shader::~Shader()
    {
        if(handle)
            glDeleteShader(handle);
    }

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

            switch(type)
            {
                case VERTEX:    handle = glCreateShader(GL_VERTEX_SHADER); break;
                case FRAGMENT:  handle = glCreateShader(GL_FRAGMENT_SHADER); break;
                case GEOMETRY:  handle = glCreateShader(GL_GEOMETRY_SHADER); break;
                default: cerr << "Error compiling shader: Invalid type." << endl; break;
            }

            const GLchar* pFile = sources[0]->getData();
            const GLint pSize  = sources[0]->getSize();
            glShaderSource(handle, 1, &pFile, &pSize);

            glCompileShader(handle);

            GLint result;
            glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
            if(result == GL_FALSE)
            {
                cerr << "Error compiling shader, log:" << endl;
                GLint logLength;
                glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
                if(logLength > 0)
                {
                    char* log = new char[logLength];
                    GLsizei written;
                    glGetShaderInfoLog(handle, logLength, &written, log);
                    cerr << log << endl;
                    delete[] log;
                }
                else 
                {
                    cerr << "No Log available.." << endl;
                }

                glDeleteShader(handle);
                compiled = false;
                handle = 0;

                return false;
            }

            compiled = true;
        }

        return true;
    }

    //---------------------------------------------------------
    // SHADERPROGRAM
    //---------------------------------------------------------

    ShaderProgram::ShaderProgram(string name)
    {
        handle = 0;
        linked = false;
        this->name = name;
        init();
    }

    ShaderProgram::~ShaderProgram() { 
        if(handle)
            glDeleteShader(handle);
    }

    bool ShaderProgram::init()
    {
        handle = glCreateProgram();
        if(handle == 0) { 
            cerr << "Error creating program" << endl; 
            return false;
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
        glGetProgramiv(handle, GL_LINK_STATUS, &result);
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
        return true;
    }

    void ShaderProgram::use()
    {
        glUseProgram(handle);
    }

    //---------------------------
    // Uniforms
    //---------------------------

    GLuint ShaderProgram::getUniformLocation(const char* name)
    {
        return glGetUniformLocation(handle, name);
    }

    void ShaderProgram::setUniform1i(const char* name, int val)
    {
        glUniform1i(getUniformLocation(name), val);
    }

    void ShaderProgram::setUniform2fv(const char* name, vec2 values)
    {
        glUniform2fv(getUniformLocation(name), 1, &values[0]);
    }

    void ShaderProgram::setUniformMatrix4fv(const char* name, mat4 matrix)
    {
        glUniformMatrix4fv(getUniformLocation(name), 1, false, &matrix[0][0]);
    }
}
