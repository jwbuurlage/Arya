#include <iostream>
#include <GL/glew.h>
#include "../include/Shaders.h"
#include "../include/Files.h"

using std::string;
using std::cerr;
using std::endl;

namespace Arya
{
  bool Shader::addSourceFile(string f)
  {
    AFile source = FileSystem::shared()->getFile(f);
    if(source.empty()) return false;
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
        gl_sources[i] = sources[i].data;
    }
    return false;
  }

  bool ShaderManager::init()
  {
    return true;
  }
}
