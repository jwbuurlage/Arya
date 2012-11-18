#include <iostream>
#include "shaders.h"

using std::string;
using std::cerr;
using std::endl;

namespace Arya
{
  bool Shader::loadFromString(string sourceString, ShaderType type)
  {
    source = sourceString;

    return false;
  }

  bool Shader::loadFromFile(string fileName, ShaderType type)
  {
    return false;
  }

  bool Shader::compile()
  {
    if(source == "") {
      cerr << "No source set, cannot compile shader" << endl;
      return false;
    }
    return false;
  }

  bool ShaderManager::init()
  {
    return true;
  }
}
